#include "server/ApiServer.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <unordered_set>
using std::unordered_set;

#include <memory>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <sstream>

#include "json.hpp"
using json = nlohmann::json;

#include <cppkafka/cppkafka.h>

#include "observability/prometheus/Registry.h"
using jetstream::observability::prometheus::Registry;
using jetstream::observability::prometheus::Metric;

#include "rapidcsv.h"

#include "Common.h"


#include "server/model/Model.h"
using ::jetstream::server::model::Model;

#include "server/collection/ModelSet.h"
using ::jetstream::server::collection::ModelSet;


#include "server/search/SearchQuery.h"
using ::jetstream::server::search::SearchQuery;

#include "frontend/view/SearchView.h"

#include <thread>
#include "stopwatch.h"

#include <chrono>


namespace jetstream{
namespace server{

    ApiServer::ApiServer( JetStream* jetstream_app, ::jetstream::config::IngestWriterConfig config )
            :jetstream_app(jetstream_app), config(config)
    {

        this->typesense_client = std::make_shared<TypesenseClient>( this->config.getConfigSetting("destination_hostname"), this->config.getConfigSetting("destination_token") );
        this->postgres_client = std::make_shared<PostgresClient>( this->config.getConfigSetting("postgres_url") );

        this->postgres_service = std::make_shared<PostgresService>( this->postgres_client );
        this->typesense_service = std::make_shared<TypesenseService>( this->typesense_client );

    }


    void ApiServer::run(){

        cout << "Starting API Server" << endl;

        httplib::Server http_server;

        Registry registry;
        registry.addMetric( { "api_total_http_requests", "Number of HTTP Requests for API." } );

        http_server.set_payload_max_length(1024 * 1024 * 512); // 512MB

        http_server.set_logger([](const auto& request, const auto& response ) {
            cout << request.method << " " << request.path << endl;
            cout << response.status << " " << response.reason << " " << response.location << endl;
        });

        http_server.set_exception_handler([](const auto&, auto& response, std::exception &e) {
            response.status = 500;
            auto fmt = "<h1>Error 500</h1><p>%s</p>";
            char buf[BUFSIZ];
            snprintf(buf, sizeof(buf), fmt, e.what());
            response.set_content(buf, "text/html");
        });


        ApiServer* api_server_ptr = this;






        //setup kafka consumer

            using cppkafka::Consumer;
            using cppkafka::Configuration;
            using cppkafka::Message;
            using cppkafka::TopicPartitionList;

            // Construct the configuration
            cppkafka::Configuration kafka_config = {
                { "metadata.broker.list", this->config.getConfigSetting("brokers") },
                { "group.id", this->config.getConfigSetting("consumer_group") },
                // Disable auto commit
                { "enable.auto.commit", false },
                { "auto.offset.reset", "latest" } //earliest or latest
            };


            // Create the consumer
            cppkafka::Consumer kafka_consumer( kafka_config );

            // Print the assigned partitions on assignment
            kafka_consumer.set_assignment_callback([](const TopicPartitionList& partitions) {
                cout << "JetStream: Got assigned partitions: " << partitions << endl;
            });

            // Print the revoked partitions on revocation
            kafka_consumer.set_revocation_callback([](const TopicPartitionList& partitions) {
                cout << "JetStream: Got revoked partitions: " << partitions << endl;
            });

            // Subscribe
            kafka_consumer.subscribe( { this->config.getConfigSetting("topic") } );




        //setup kafka producer

            cppkafka::Configuration kafka_events_producer_config = {
                { "metadata.broker.list", this->config.getConfigSetting("brokers") }
            };
            cppkafka::Producer events_kafka_producer( kafka_events_producer_config );
            //events_kafka_producer.set_payload_policy( cppkafka::PayloadPolicy::BLOCK_ON_FULL_QUEUE );
            cppkafka::MessageBuilder events_message_builder( this->config.getConfigSetting("topic") );


            httplib::Headers common_response_headers;
            //common_response_headers.insert( {"Access-Control-Allow-Credentials", "true"} );
            //common_response_headers.insert( {"Access-Control-Allow-Headers", "*"} );
            //common_response_headers.insert( {"Access-Control-Allow-Methods", "GET, PUT, POST, DELETE, PATCH, OPTIONS"} );


        //common variables
            bool keep_running = true;


        auto merge_headers = []( const httplib::Request& request, const httplib::Headers& source_headers, httplib::Headers& destination_headers ){

            string request_origin;

            for( const auto& [ key, value ] : request.headers ){
                if( to_lower_string(key) == "origin" ){
                    request_origin = value;
                }
            }

            for( const auto& pair : source_headers ){
                destination_headers.insert( pair );
            }

            //destination_headers.insert( { "Access-Control-Allow-Origin", request_origin } );

        };






        http_server.Get("/", [&]( const httplib::Request& request, httplib::Response& response ){

            auto session = api_server_ptr->getSessionFromRequest( request );

            registry.incrementMetric( "api_total_http_requests", {
                { "method", "GET" },
                { "path", "/" }
                //{ "organization", session->getOrganizationSlug() }
            });

            merge_headers( request, common_response_headers, response.headers );
            response.set_content( "hello", "text/plain" );

        });




        http_server.Get("/api/v1/build-indexes", [&]( const httplib::Request& request, httplib::Response& response ){

            auto session = api_server_ptr->getSessionFromRequest( request );

            registry.incrementMetric( "api_total_http_requests", {
                { "method", "GET" },
                { "path", "/api/v1/build-indexes" }
                //{ "organization", session->getOrganizationSlug() }
            });


            json response_json = json::object();
            response_json["operation"] = "indexer";

            bool success = false;

            try{

                std::scoped_lock<std::mutex> lock{ api_server_ptr->global_mutex };
                //api_server_ptr->getModelSetCollection(session)->indexAndLoadAll();
                success = true;

            }catch( pqxx::sql_error const &e ){

                std::cerr << "SQL error: " << e.what() << std::endl;
                std::cerr << "Query was: " << e.query() << std::endl;

            }catch( std::exception const &e ){

                std::cerr << "Error: " << e.what() << std::endl;

            }

            response_json["success"] = success;

            merge_headers( request, common_response_headers, response.headers );
            response.set_content( response_json.dump(4), "application/json" );

        });





        http_server.Post("/api/v1/gizmo/multi", [&]( const httplib::Request& request, httplib::Response& response, const httplib::ContentReader& content_reader ){

            auto session = api_server_ptr->getSessionFromRequest( request );

            registry.incrementMetric( "api_total_http_requests", {
                { "method", "POST" },
                { "path", "/api/v1/gizmo/multi" }
                //{ "organization", session->getOrganizationSlug() }
            });

            std::ostringstream output_stream;


            string response_body;

            {
                std::scoped_lock<std::mutex> lock{ api_server_ptr->global_mutex };

                //auto all_modelsets = api_server_ptr->getModelSetCollection(session);

                if( request.is_multipart_form_data() ){
                    // NOTE: `content_reader` is blocking until every form data field is read
                    httplib::MultipartFormDataItems files;
                    content_reader(
                        [&](const httplib::MultipartFormData &file){
                            files.push_back(file);
                            return true;
                        },
                        [&](const char *data, size_t data_length){
                            files.back().content.append(data, data_length);
                            return true;
                        }
                    );

                    vector<json> documents;

                    int file_number = 0;
                    for( const auto& file : files ){

                        file_number++;

                        try{

                            auto gizmo_set = std::make_shared<GizmoSet>();
                            //gizmo_set->setOrganizationSlug(session->getOrganizationSlug());
                            //gizmo_set->setOrganizationId(session->getOrganizationId());
                            gizmo_set->loadFromCsv( file.content );

                            //all_modelsets->importGizmos( gizmo_set, events_kafka_producer, events_message_builder );

                            output_stream << json(*gizmo_set).dump(4) << endl;

                        }catch( std::exception& e ){
                            output_stream << "Error: " << e.what() << endl;
                            response.status = 400;
                        }

                    }



                } else {

                    std::string body;
                    content_reader([&](const char *data, size_t data_length){
                        body.append(data, data_length);
                        return true;
                    });

                    output_stream << "<p>File 1:<br/>";
                    output_stream << "&nbsp;&nbsp;&nbsp;&nbsp;content(size): " << body.size() << ":<br/>";

                }

                response_body = output_stream.str();

            }

            merge_headers( request, common_response_headers, response.headers );
            response.set_content( response_body, "text/html" );

        });








        http_server.Post("/api/v1/events", [&]( const httplib::Request& request, httplib::Response& response ){

            registry.incrementMetric( "api_total_http_requests", {
                { "method", "POST" },
                { "path", "/api/v1/events" }
            });

            json response_json = json::object();
            try{
                json request_body = json::parse(request.body);

                if( !request_body.is_array() ){
                    throw std::runtime_error( "HTTP Body must be a JSON Array.");
                }

                for( const json& event : request_body ){

                    if( !event.is_object() ){
                        cerr << "Dropped object: " << event.dump();
                    }else{
                        string event_str = event.dump();
                        events_message_builder.payload( event_str );
                        events_kafka_producer.produce( events_message_builder );
                    }

                }

                response_json["response_type"] = "events_accepted";
                response_json["objects"] = json::array();

            }catch( std::exception& e ){

                response_json["response_type"] = "events_rejected";
                response_json["objects"] = json::array( { string(e.what()) });
                response.status = 400;
                cerr << response_json.dump() << endl;

            }

            std::ostringstream output_stream;
            output_stream << response_json.dump(4);

            string response_body = output_stream.str();
            merge_headers( request, common_response_headers, response.headers );
            response.set_content( response_body, "application/json" );

        });




        http_server.Get("/metrics", [&registry]( const httplib::Request&, httplib::Response& response ){

            std::ostringstream output_stream;
            output_stream << registry;

            string response_body = output_stream.str();
            response.set_content( response_body, "text/plain; version=0.0.4; charset=utf-8" );

        });



        http_server.Get("/api/search", [&]( const httplib::Request& request, httplib::Response& response ){

            auto session = api_server_ptr->getSessionFromRequest( request );

            registry.incrementMetric( "api_total_http_requests", {
                { "method", "GET" },
                { "path", "/api/search" }
                //{ "organization", session->getOrganizationSlug() }
            });

            ::jetstream::frontend::view::SearchView view;
            response.set_content( view.render(), view.getMimeType().c_str() );

        });








        auto listen_for_kafka_messages = [&]() {

            const size_t max_batch_size = 1;
            const std::chrono::milliseconds poll_timeout_ms{1000};


            while( keep_running ){

                precise_stopwatch stopwatch;

                try{

                    vector<Message> messages = kafka_consumer.poll_batch(max_batch_size, poll_timeout_ms);
                    size_t total_messages = messages.size();

                    //poll the producer (required)
                    events_kafka_producer.poll( std::chrono::milliseconds(1) );


                    if( total_messages > 0 ){

                        cout << "End poll_batch: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                        int x = 0;
                        for( auto &message: messages ){

                            // If we managed to get a message
                            if( message.get_error() ){

                                // Ignore EOF notifications from rdkafka
                                if( !message.is_eof() ){
                                    cerr << "JetStream: [+] Received error notification: " + message.get_error().to_string() << endl;
                                }

                            }else{

                                try{
                                    const string payload = message.get_payload();

                                    json event = json::parse(payload);
                                    cout << event.dump() << endl;

                                    api_server_ptr->event_router.route( event, api_server_ptr, events_kafka_producer, events_message_builder );

                                }catch( std::exception &e ){
                                    cerr << "JetStream: failed to route event: " + string(e.what()) << endl;
                                }catch( ... ){
                                    cerr << "JetStream: failed to route event (unknown exception)" << endl;
                                }

                            }

                        }

                        kafka_consumer.commit();
                        cout << "End consume: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                    }

                } catch (std::exception &e) {

                    cerr << "JetStream: general exception caught with ingest writer: " + string(e.what()) << endl;
                    cout << "End exception: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                }


            } // end while run

        };

        std::thread kafka_consumer_thread( listen_for_kafka_messages );

        http_server.listen("0.0.0.0", 8080);
        kafka_consumer_thread.join();

    }





    string ApiServer::getCookieValue( const httplib::Request& request, const string& cookie_name ) const{


        vector<string> cookies;

        for( const auto& [ key, value ] : request.headers ){
            if( to_lower_string(key) == "cookie" ){
                cookies = split_string( value, ';');
            }
        }

        if( cookies.empty() ) throw std::runtime_error("No cookies found.");


        for( const string& cookie : cookies ){
            vector<string> cookie_parts = split_string( cookie, '=' );

            if( cookie_parts.size() < 2 ) continue;

            const string cookie_name_stripped = strip_string(cookie_parts[0]);

            if( cookie_name_stripped == cookie_name ){
                return cookie_parts[1];
            }

        }

        throw std::runtime_error("Cookie value not found.");

    }


    session_ptr ApiServer::getSessionFromRequest( const httplib::Request& request ){

        std::scoped_lock<std::mutex> lock{ this->global_mutex };

        string request_session_id;

        try{
            //read from cookie
            request_session_id = this->getCookieValue( request, "sessionid" );

        }catch( std::exception& e ){

            //read from request header
            for( const auto& [ key, value ] : request.headers ){
                if( to_lower_string(key) == "x-session-id" ){
                    request_session_id = value;
                }
            }

        }

        //if( this->active_sessions.count(request_session_id) ){
        //    return this->active_sessions.at(request_session_id);
        //}

        session_ptr session = this->postgres_service->getSessionBySessionId( request_session_id );
        this->active_sessions[ request_session_id ] = session;

        return session;

    }



}
}