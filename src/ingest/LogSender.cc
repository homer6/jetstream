#include "ingest/LogSender.h"
#include "ingest/LogSenderAttempt.h"

#include "ingest/IngestWriterExporter.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <chrono>
#include <memory>

#include "json.hpp"
using json = nlohmann::json;

#include "JetStream.h"

#include "stopwatch.h"

namespace jetstream{
namespace ingest{


    LogSender::LogSender( const ::jetstream::config::IngestWriterConfig& config, IngestWriterExporter& exporter )
        :config(config),
         error_producer(config.jetstream->observer, config.getConfigSetting("brokers"), "ingest-errors"),
         rejected_error_producer(config.jetstream->observer, config.getConfigSetting("brokers"), "ingest-errors-rejected"),
         exporter(exporter)
    {



    }


    void LogSender::poll( int timeout_ms ){

        this->error_producer.poll( timeout_ms );
        this->rejected_error_producer.poll( timeout_ms );

    }



    const string LogSender::getIngestApiPathByLogType( const string& log_type ) const{

        const string api_prefix = "/api/v1";

        if( log_type == "gizmo1" ) return api_prefix + "/gizmo1";
        if( log_type == "gizmo2" ) return api_prefix + "/gizmo2";

        if( log_type.size() < 1000 ){
            throw std::runtime_error( "Unknown API mapping for log_type: " + log_type );
        }
        throw std::runtime_error( "Unknown API mapping for log_type." );

    }


    void LogSender::send( const LogSplitter& splitter, const string& organization_id ) noexcept{

        LogSender* sender_ptr = this;

        for( const auto& [log_type, logs_json_const] : splitter.logs ){

            //note: attempts below may modify the logs_json payload as they drop erroneous messages

            //TODO: refactor to remove unneeded copy
            json logs_json = logs_json_const;

            if( logs_json.size() == 0 ) continue;

            //intentially copy the logs_json payload
            //this->pool.push_task( [sender_ptr, log_type, logs_json](){

                const int max_attempts = 3;
                int attempt_number = 0;
                bool success = false;

                const double total_record_count = logs_json.size();

                this->exporter.registry.incrementMetric( "ingest_sink_events_total", {
                    { "organization_id", organization_id },
                    { "log_type", log_type }
                }, total_record_count );

                this->exporter.registry.incrementMetric( "ingest_sink_batches_total", {
                    { "organization_id", organization_id },
                    { "log_type", log_type }
                });


                while( attempt_number < max_attempts ){

                    attempt_number++;

                    if( logs_json.size() == 0 ){
                        cout << "Gave up on retries. No logs to send. Skipping attempt " << attempt_number << "." << endl;
                        break;
                    }

                    this->exporter.registry.incrementMetric( "ingest_sink_http_requests_total", {
                        { "organization_id", organization_id },
                        { "log_type", log_type }
                    });

                    LogSenderAttempt attempt( sender_ptr, log_type, logs_json, attempt_number );
                    if( attempt.send(organization_id) ){
                        success = true;
                        if( attempt.last_error_report.size() ){
                            this->rejected_error_producer.produce( attempt.last_error_report );
                        }
                        break;
                    }else{
                        if( attempt.last_error_report.size() ){
                            this->rejected_error_producer.produce( attempt.last_error_report );
                        }
                    }

                }

                const double sent_record_count = logs_json.size();

                if( success ){

                    cout << "Successfully sent on attempt " << attempt_number << "." << endl;

                    if( sent_record_count > 0 ){
                        this->exporter.registry.incrementMetric( "ingest_sink_accepted_events_total", {
                            { "organization_id", organization_id },
                            { "log_type", log_type }
                        }, sent_record_count );
                    }

                    if( sent_record_count < total_record_count ){

                        this->exporter.registry.incrementMetric( "ingest_sink_dropped_events_total", {
                            { "organization_id", organization_id },
                            { "log_type", log_type }
                        }, total_record_count - sent_record_count );

                        this->exporter.registry.incrementMetric( "ingest_sink_rejected_events_total", {
                            { "organization_id", organization_id },
                            { "log_type", log_type }
                        }, total_record_count - sent_record_count );

                    }

                }else{

                    cout << "Failed to send after " << attempt_number << " attempts." << endl;
                    this->exporter.registry.incrementMetric( "ingest_sink_dropped_events_total", {
                        { "organization_id", organization_id },
                        { "log_type", log_type }
                    }, total_record_count );

                }

            //});

        }

    }




    void LogSender::updateJwtToken( const string& user_id ){

        if( this->jwt_token.size() > 0 ) return;

        //sleeping to prevent hammering of the auto API
        //using namespace std::chrono_literals;
        //std::this_thread::sleep_for(10ms);

        this->exporter.registry.incrementMetric( "ingest_sink_jwt_refresh_count", {
            { "user_id", user_id }
        });

        std::scoped_lock<std::mutex> lock{ this->jwt_token_mutex };
        if( this->jwt_token.size() > 0 ) return;

        HttpConnection connection( this->config.getConfigSetting("destination_auth_url") );


        /*
        const string destination_username = config.getConfigSetting( "destination_username" );
        const string destination_password = config.getConfigSetting( "destination_password" );
        if( destination_username.size() ){
            const string basic_auth_credentials = encodeBase64( destination_username + ":" + destination_password );
            connection.setHeader( "Authorization", "Basic " + basic_auth_credentials );
        }
        */

        json payload = json::object();
        payload["username"] = this->config.getConfigSetting( "destination_username" );
        payload["password"] = this->config.getConfigSetting( "destination_password" );

        try{

            httplib::Result auth_result = connection.http_client->Post( connection.full_path_template.c_str(), connection.request_headers, payload.dump(), "application/json" );

            if( auth_result ){

                const auto& auth_response = auth_result.value();

                if( auth_response.status >= 200 && auth_response.status < 300 ){

                    try{

                        json response_object = json::parse( auth_response.body );

                        if( !response_object.is_object() ){
                            throw std::runtime_error("Auth: response object expected.");
                        }

                        if( !response_object.count("token") ){
                            throw std::runtime_error("Auth: response object expects 'token' key.");
                        }

                        if( !response_object.at("token").is_string() ){
                            throw std::runtime_error("Auth: response object 'token' key should be a string.");
                        }

                        this->jwt_token = response_object.at("token").get<string>();
                        //cout << this->jwt_token << endl;
                        //this->jwt_token = "zzz";
                        //this->ingest_connection.setHeader( "Authorization", this->jwt_token );

                        for( auto& [log_type, connection] : this->ingest_connections ){
                            connection.setHeader( "Authorization", this->jwt_token );
                        }


                    }catch( std::exception& e ){

                        json bad_response_object = json::object();

                        bad_response_object["description"] = "Auth: Malformed 200-level response.";
                        bad_response_object["details"] = string(e.what());
                        bad_response_object["body"] = auth_response.body;
                        bad_response_object["path"] = connection.full_path_template;
                        bad_response_object["status"] = auth_response.status;
                        bad_response_object["headers"] = json::object();

                        for( auto &header : auth_response.headers ){
                            bad_response_object["headers"][header.first] = header.second;
                        }

                        cerr << bad_response_object.dump() << endl;

                    }


                }else{

                    json bad_response_object = json::object();

                    bad_response_object["description"] = "Auth: Ingest non-200 response.";
                    bad_response_object["body"] = auth_response.body;
                    bad_response_object["path"] = connection.full_path_template;
                    bad_response_object["status"] = auth_response.status;
                    bad_response_object["headers"] = json::object();

                    for( auto &header : auth_response.headers ){
                        bad_response_object["headers"][header.first] = header.second;
                    }

                    cerr << bad_response_object.dump() << endl;

                }

            }else{

                json error_object = json::object();
                error_object["description"] = "Auth Http Error";
                error_object["error"] = httplib::to_string( auth_result.error() );
                error_object["path"] = connection.full_path_template;
                cerr << error_object.dump() << endl;

            }

        }catch( const std::exception& e ){

            cerr << "JetStream Auth: failed to send log lines to ingest: " + string(e.what()) << endl;

        }


    }


    void LogSender::invalidateJwtToken(){

        std::scoped_lock<std::mutex> lock{ this->jwt_token_mutex };

        this->jwt_token = "";

        for( auto& [log_type, connection] : this->ingest_connections ){
            connection.setHeader( "Authorization", "invalid" );
        }

    }


    HttpConnection& LogSender::useConnection( const string& log_type ){

        if( !this->ingest_connections.count(log_type) ){
            this->ingest_connections.emplace( log_type, this->config.getConfigSetting("destination_hostname") );
            this->ingest_connections.at(log_type).setHeader( "Authorization", this->jwt_token );
        }

        return this->ingest_connections.at(log_type);

    }


}
}

