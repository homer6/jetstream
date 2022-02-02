#include "writer/ElasticSearchWriter.h"


#include <iostream>
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;

#include <unistd.h>
#include <signal.h>

#include "KafkaProducer.h"

#include <cstdio>
#include <stdexcept>
#include <memory>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Common.h"

#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <errno.h>
#include <signal.h>

#include "json.hpp"
using json = nlohmann::json;

#include <cppkafka/cppkafka.h>

#include <httplib.h>

#include <chrono>


#include <map>
using std::map;



namespace jetstream{
namespace writer{

    ElasticSearchWriter::ElasticSearchWriter( const ::jetstream::config::ElasticSearchWriterConfig& config )
        :config(config)
    {

    }


	void ElasticSearchWriter::run( const bool& keep_running ){

		//setup kafka consumer

			using cppkafka::Consumer;
			using cppkafka::Configuration;
			using cppkafka::Message;
			using cppkafka::TopicPartitionList;

			// Construct the configuration
				cppkafka::Configuration kafka_config = {
				    { "metadata.broker.list", config.getConfigSetting("brokers") },
				    { "group.id", config.getConfigSetting("consumer_group") },
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
			    kafka_consumer.subscribe( { config.getConfigSetting("topic") } );


		// connect to elasticsearch

			int destination_port = 9200;
            const string destination_hostname = config.getConfigSetting( "destination_hostname" );
            const string destination_secure = config.getConfigSetting( "destination_secure" );
			string destination_hostname_host;

			vector<string> hostname_parts = jetstream::split_string( destination_hostname, ':' );
			if( hostname_parts.size() == 2 ){
				destination_hostname_host = hostname_parts[0];
				destination_port = std::stoi( hostname_parts[1] );
			}else if( hostname_parts.size() == 1 ){
				destination_hostname_host = hostname_parts[0];
			}else{
				throw std::runtime_error( "Unexpected elasticsearch target hostname: " + destination_hostname );
			}

			std::unique_ptr<httplib::Client> http_client;

			if( destination_secure == "true" ){
				http_client.reset( new httplib::SSLClient( destination_hostname_host.c_str(), destination_port ) );
			}else{
				http_client.reset( new httplib::Client( destination_hostname_host.c_str(), destination_port ) );
			}

			//int x = 0;


            const string destination_index = config.getConfigSetting( "destination_index" );
			const string post_path = "/" + destination_index + "/_bulk";



		// connect to elasticsearch

			//int x = 0;

			httplib::Headers request_headers{
				{ "Host", destination_hostname },
				{ "User-Agent", "jetstream" }
			};

            const string destination_username = config.getConfigSetting( "destination_username" );
            const string destination_password = config.getConfigSetting( "destination_password" );
			if( destination_username.size() ){
				const string basic_auth_credentials = encodeBase64( destination_username + ":" + destination_password );
				request_headers.insert( { "Authorization", "Basic " + basic_auth_credentials } );
			}



		// consume from kafka
			while( keep_running ){


				try{

					// Try to consume a message
			        //Message message = kafka_consumer.poll();

					size_t max_batch_size = 20000;
					std::chrono::milliseconds poll_timeout_ms{1000};

					vector<Message> messages = kafka_consumer.poll_batch( max_batch_size, poll_timeout_ms );


				    if( messages.size() ){

				    	string batch_payload;

				    	for( auto& message : messages ){

					        // If we managed to get a message
					        if( message.get_error() ){

					            // Ignore EOF notifications from rdkafka
					            if( !message.is_eof() ){
					            	cerr << "JetStream: [+] Received error notification: " + message.get_error().to_string() << endl;
					            }

					        } else {

					            // Print the key (if any)
					            //if( message.get_key() ){
					            //    cout << "JetStream: message key: " + string(message.get_key()) << endl;
					            //}

					            const string payload = message.get_payload();


					            string request_body = "{\"index\":{\"_index\":\"" + destination_index + "\",\"_type\":\"_doc\"}}\n";

					            json json_object;
					            try{

					            	json_object = json::parse( payload );

					            	if( json_object.count("@timestamp") ){

					            		if( json_object["@timestamp"].is_number() ){

						            		double timestamp_double = json_object["@timestamp"].get<double>();

						            		//converts localtime to gmtime
						            		const string timestamp_str = format_timestamp( timestamp_double, "%Y/%m/%d %H:%M:%S" );

						            		json_object["@ts"] = timestamp_str;

					            		}

					            	}

					            	request_body += json_object.dump() + "\n";

					            }catch( const std::exception& e ){

					            	//cerr << "JetStream: failed to parse payload: " + string(e.what()) << endl;
							        string json_meta = "{\"@timestamp\":" + get_timestamp() + ",\"@ts\":\"" + get_timestamp("%Y/%m/%d %H:%M:%S") + "\",\"host\":\"" + config.getConfigSetting("hostname") + "\",\"source\":\"" + config.getConfigSetting("topic") + "\",\"prd\":\"" + config.getConfigSetting("product_code") + "\"";
					            	request_body += json_meta + ",\"log\":\"" + escape_to_json_string(payload) + "\"}\n";

					            }

					            batch_payload += request_body;


					            kafka_consumer.commit(message);

					        } // end message.get_error()


					    } //end foreach message




					    if( batch_payload.size() ){

			        		try{

		        				std::shared_ptr<httplib::Response> es_response = http_client->Post( post_path.c_str(), request_headers, batch_payload, "application/x-ndjson" );

		        				if( es_response ){

			        				if( es_response->status >= 200 && es_response->status < 300 ){

										// Now commit the message (ack kafka)
							            //kafka_consumer.commit(message);

			        				}else{

			        					json bad_response_object = json::object();

			        					bad_response_object["description"] = "Elasticsearch non-200 response.";
			        					bad_response_object["body"] = es_response->body;
			        					bad_response_object["status"] = es_response->status;
			        					bad_response_object["headers"] = json::object();

			        					for( auto &header : es_response->headers ){
			        						bad_response_object["headers"][header.first] = header.second;
			        					}

			        					cerr << bad_response_object.dump() << endl;

			        				}

		        				}else{

		        					cerr << "No response object." << endl;

		        				}

			                }catch( const std::exception& e ){

			                	cerr << "JetStream: failed to send log lines to elasticsearch: " + string(e.what()) << endl;

			                }


					    }


				    } // end messages.size()

				}catch( std::exception &e ){

					cerr << "JetStream: general exception caught with elasticsearch writer: " + string(e.what()) << endl;

				}



			} // end while run

			cout << "JetStream: exiting." << endl;

	}



}
}
