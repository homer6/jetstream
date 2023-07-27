#include "server/HttpServer.h"

#include "httplib.h"

#include <string>
using std::string;

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


#include "Common.h"

namespace jetstream{
namespace server{

    HttpServer::HttpServer( JetStream* jetstream_app, ::jetstream::config::IngestWriterConfig config )
        :jetstream_app(jetstream_app), config(config)
    {


    }


    void HttpServer::run(){

        cout << "starting jetstream proxy" << endl;

        httplib::Server http_server;

        cppkafka::Configuration kafka_producer_config = {
            { "metadata.broker.list", this->config.getConfigSetting("brokers") }
        };
        cppkafka::Producer all_events_kafka_producer( kafka_producer_config );
        //kafka_producer.set_payload_policy( cppkafka::PayloadPolicy::BLOCK_ON_FULL_QUEUE );

        cppkafka::Producer conn_events_kafka_producer( kafka_producer_config );


        //config.getConfigSetting("destination_topic")
        cppkafka::MessageBuilder all_events_message_builder( "all-ingest-events" );

        //message_builder.payload( "starting jetstream proxy" );
        //all_events_kafka_producer.produce( message_builder );

        //cout << "writing to topic: " << destination_topic << endl;




        Registry registry;
        registry.addMetric( { "ingest_events_total", "Number of ingest events." } );


        http_server.Get("/hi", []( const httplib::Request&, httplib::Response& response ){
            response.set_content( "Hello World!", "text/plain" );
        });


        http_server.Post("/ingest", [&all_events_kafka_producer, &conn_events_kafka_producer, &all_events_message_builder, &registry]( const httplib::Request& request, httplib::Response& response ){

            try{

                string app_id;
                string log_type;

                for( const auto& [ key, value ] : request.headers ){

                    if( key == "x-app-id" ){
                        app_id = value;
                    }

                }


                vector<string> lines = split_string( request.body, '\n' );

                for( const string& line : lines ){

                    try{
                        json wrapped_object = json::object();
                        wrapped_object["app_id"] = app_id;

                        if( log_type.size() == 0 ) continue;
                        json json_object = json::parse( line );

                        json wrapped_object_all = wrapped_object;
                        wrapped_object_all["log_obj"] = json_object;
                        wrapped_object_all["log_type"] = log_type;
                        string single_message_all = wrapped_object_all.dump();
                        all_events_message_builder.payload( single_message_all );
                        all_events_kafka_producer.produce( all_events_message_builder );

                        registry.incrementMetric( "ingest_events_total", {
                            { "app_id", app_id },
                            { "log_type", log_type }
                        });

                    }catch(std::exception& e){
                        cerr << "exception: " << e.what() << endl;
                    }

                }


            }catch( std::runtime_error& e ){

                cerr << "exception: " << e.what() << endl;

            }

            string response_body = "success"; 
            cout << response_body << endl;
            response.set_content( response_body, "application/json" );

        });



        http_server.Get("/metrics", [&registry]( const httplib::Request&, httplib::Response& response ){

            std::ostringstream output_stream;
            output_stream << registry;

            string response_body = output_stream.str();
            response.set_content( response_body, "text/plain; version=0.0.4; charset=utf-8" );

        });



        http_server.listen("0.0.0.0", 8080);

    }

}
}