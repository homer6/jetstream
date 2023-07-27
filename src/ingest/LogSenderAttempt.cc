#include "ingest/LogSenderAttempt.h"
#include "ingest/LogSender.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "stopwatch.h"

#include "client/HttpConnection.h"
using jetstream::client::HttpConnection;

#include "client/KafkaProducer.h"
using jetstream::client::KafkaProducer;

#include "Common.h"
#include <set>
using std::set;


namespace jetstream{
namespace ingest {

    LogSenderAttempt::LogSenderAttempt( LogSender *sender_ptr, const string &log_type, json &logs_json, const int attempt_number )
        :sender_ptr(sender_ptr), log_type(log_type), logs_json(logs_json), attempt_number(attempt_number)
    {


    }


    bool LogSenderAttempt::send( const string& user_id ) noexcept{

        precise_stopwatch stopwatch;
        bool success = false;

        try{

            cout << "log_type: " << this->log_type << "(" << this->logs_json.size() << ")" << endl;
            cout << "attempt: " << this->attempt_number << endl;

            const string batch_payload = logs_json.dump();
            cout << "End serialize: " << stopwatch.elapsed_time_ms() << "ms" << endl;

            const string http_path = this->sender_ptr->getIngestApiPathByLogType(log_type); //throws on unknown type

            HttpConnection &http_connection = this->sender_ptr->useConnection(log_type);

            httplib::Result ingest_result = http_connection.http_client->Post( http_path.c_str(), http_connection.request_headers, batch_payload, "application/json");
            cout << "End request: " << stopwatch.elapsed_time_ms() << "ms" << endl;

            if( ingest_result ){

                const auto ingest_response = ingest_result.value();

                if( ingest_response.status >= 200 && ingest_response.status < 300 ){

                    // Now commit the message (ack kafka)
                    //kafka_consumer.commit(message);
                    success = true;

                }else if( ingest_response.status == 401 ){

                    // 400-level response; get new JWT token
                    const auto headers_copy = http_connection.request_headers;  //copy these before invalidating the token
                    this->sender_ptr->invalidateJwtToken();

                }else if( ingest_response.status == 400 || (ingest_response.status > 401 && ingest_response.status < 500) ){

                    // 400-level response;

                    const auto headers_copy = http_connection.request_headers;  //copy these before invalidating the token

                    //cout << "End invalidate token: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                    json bad_response_object = json::object();

                    bad_response_object["description"] = "Ingest 400-level response.";
                    bad_response_object["log_type"] = log_type;
                    bad_response_object["attempt"] = this->attempt_number;
                    bad_response_object["user_id"] = user_id;

                    json request_json = json::object();
                    request_json["path"] = http_path;
                    bad_response_object["request"] = request_json;

                    bool can_filter_and_retry = false;
                    json response_body_json = json::object();


                    json response_json = json::object();
                    try{
                        response_body_json = json::parse( ingest_response.body );
                        response_json["json_body"] = response_body_json;
                        can_filter_and_retry = true;
                    }catch( std::exception &e ){
                        response_json["body"] = ingest_response.body;
                    }
                    response_json["status"] = ingest_response.status;
                    response_json["headers"] = json::object();
                    for( auto &header: ingest_response.headers ){
                        response_json["headers"][header.first] = header.second;
                    }
                    bad_response_object["response"] = response_json;

                    //dump the response to cerr (without the request payload)
                    cerr << bad_response_object.dump() << endl;
                    cout << "End bad_response_object serialize: " << stopwatch.elapsed_time_ms() << "ms" << endl;


                    //push the request payload (along with the response) to the kafka topic: ingest-errors

                    json request_obj = request_json;

                    json headers_obj = json::object();
                    for( const auto &[key, value]: headers_copy ){
                        if( key == "Authorization" ){
                            headers_obj[key] = "REDACTED";
                        }else{
                            headers_obj[key] = value;
                        }
                    }
                    request_obj["headers"] = headers_obj;

                    request_obj["payload"] = this->logs_json;
                    //request_obj["payload"] = json::parse(batch_payload);

                    bad_response_object["request"] = request_obj;

                    string ingest_error_response = bad_response_object.dump(4);
                    this->last_error_report = ingest_error_response;
                    cout << "End ingest_error_response serialize: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                    this->sender_ptr->error_producer.produce(ingest_error_response);
                    cout << "End ingest_error_response produce: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                    if( can_filter_and_retry ){
                        size_t events_count_total = this->logs_json.size();
                        json dropped_json = this->removeErroneousRecords( response_body_json );
                        size_t events_count_rejected = dropped_json.size();
                        size_t events_count_accepted = events_count_total - events_count_rejected;

                        bad_response_object["events_count_total"] = events_count_total;
                        bad_response_object["events_count_rejected"] = events_count_rejected;
                        bad_response_object["events_count_accepted"] = events_count_accepted;

                        request_obj["payload"] = dropped_json;
                        bad_response_object["request"] = request_obj;
                        this->last_error_report = bad_response_object.dump(4);
                    }

                }else{

                    json bad_response_object = json::object();

                    bad_response_object["description"] = "Ingest general non-200 response.";
                    bad_response_object["attempt"] = this->attempt_number;
                    bad_response_object["user_id"] = user_id;

                    try{
                        bad_response_object["response_json_body"] = json::parse(ingest_response.body);
                    }catch( std::exception &e ){
                        bad_response_object["response_body"] = ingest_response.body;
                    }
                    bad_response_object["path"] = http_path;
                    bad_response_object["status"] = ingest_response.status;
                    bad_response_object["response_headers"] = json::object();

                    for( auto &header: ingest_response.headers ){
                        bad_response_object["response_headers"][header.first] = header.second;
                    }

                    string ingest_error_response = bad_response_object.dump(4);
                    this->last_error_report = ingest_error_response;

                    cerr << ingest_error_response << endl;
                    cout << "End general non-200 ingest_error_response serialize: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                    this->sender_ptr->error_producer.produce(ingest_error_response);

                }

            }else{

                json error_object = json::object();
                error_object["description"] = "Ingest Http Error";
                error_object["error"] = httplib::to_string( ingest_result.error() );
                error_object["path"] = http_path;
                error_object["attempt"] = this->attempt_number;
                error_object["user_id"] = user_id;

                string ingest_error_response = error_object.dump(4);
                this->last_error_report = ingest_error_response;

                cerr << ingest_error_response << endl;
                cout << "End Ingest Http Error serialize: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                this->sender_ptr->error_producer.produce(ingest_error_response);

            }

        }catch( const std::exception &e ){

            this->last_error_report = "JetStream: failed to send log lines to ingest: " + string(e.what());
            cerr << this->last_error_report << endl;
            this->sender_ptr->error_producer.produce(this->last_error_report);

        }catch( ... ){

            this->last_error_report = "JetStream: caught unknown exception on LogSender::send()";
            cerr << this->last_error_report << endl;
            this->sender_ptr->error_producer.produce(this->last_error_report);

            cerr << "JetStream: caught unknown exception on LogSender::send()" << endl;

        }

        cout << "End task: " << stopwatch.elapsed_time_ms() << "ms" << endl;

        return success;

    }



    json LogSenderAttempt::removeErroneousRecords( const json& response_body_json ){

        /*
         *
         *  @staticmethod
            def _extract_error_indices(errors: Collection[Mapping]) -> Collection[int]:
                """Parse errors for offending index from a list of error tuples."""
                result = []
                for err in errors:
                    parts = err[0].split(".")
                    if parts[0] == "#0":
                        idx = int(parts[1])
                        result.append(idx)
                return result
         *
         */

        // copy the original request payload; we will modify this below to remove the accepted events, leaving only the dropped events (for error reporting)
            json returned_dropped_request = this->logs_json;
            size_t original_event_count = returned_dropped_request.size();


        // determine the record indexes to drop
            set<int> indexes_to_drop; //erroneous events

            if( !response_body_json.is_object() ){
                throw std::runtime_error( "LogSenderAttempt::removeErroneousRecords(): response_body_json was not an object." );
            }

            if( !response_body_json.count("errors") ){
                throw std::runtime_error( "LogSenderAttempt::removeErroneousRecords(): response_body_json did not have an 'errors' key." );
            }

            json errors = response_body_json.at("errors");

            if( !errors.is_array() ){
                throw std::runtime_error( "LogSenderAttempt::removeErroneousRecords(): response_body_json.errors was not an array." );
            }

            int error_message_index = -1;

            for( const json& error : errors ){

                error_message_index++;

                if( !error.is_array() ){
                    throw std::runtime_error( "LogSenderAttempt::removeErroneousRecords(): error message " + std::to_string(error_message_index) + " was not an array." );
                }

                if( error.size() < 1 ){
                    throw std::runtime_error( "LogSenderAttempt::removeErroneousRecords(): error message " + std::to_string(error_message_index) + " size was less than 1." );
                }

                json error_address_json = error.at(0);
                if( !error_address_json.is_string() ){
                    throw std::runtime_error( "LogSenderAttempt::removeErroneousRecords(): error message " + std::to_string(error_message_index) + " error_address was not a string." );
                }

                string error_address = error_address_json.get<string>();

                vector<string> address_parts = split_string( error_address, '.' );

                if( address_parts.size() < 2 ){
                    continue;
                    //throw std::runtime_error( "LogSenderAttempt::removeErroneousRecords(): error message " + std::to_string(error_message_index) + " address_parts size was less than 2." );
                }

                string first_address_part = address_parts[0];
                if( first_address_part == "#0" ){
                    string second_address_part = address_parts[1];
                    if( second_address_part.size() ){
                        if( second_address_part[0] != '#' ){
                            try{
                                int record_index = std::stoi( second_address_part );
                                indexes_to_drop.insert( record_index );
                            }catch(...){

                            }
                        }
                    }
                }

            }

        // drop the indexes
            set<int>::reverse_iterator rit;
            // iterate in reverse order so that the indexes don't shift
            for( rit = indexes_to_drop.rbegin(); rit != indexes_to_drop.rend(); rit++ ){
                try{
                    this->logs_json.erase( *rit );
                }catch( ... ){

                }
            }


        // modifying returned_dropped_request to remove the accepted events, leaving only the dropped events (for error reporting)
            set<int> indexes_accepted;
            for( int x = 0; x < original_event_count; x++ ){
                if( indexes_to_drop.count(x) ){
                    //event index is an erroneous event
                }else{
                    //accepted event (we want to remove these below)
                    indexes_accepted.insert(x);
                }
            }
            // iterate in reverse order so that the indexes don't shift
            for( rit = indexes_accepted.rbegin(); rit != indexes_accepted.rend(); rit++ ){
                try{
                    returned_dropped_request.erase( *rit );
                }catch( ... ){

                }
            }

            return returned_dropped_request;

    }



}
}
