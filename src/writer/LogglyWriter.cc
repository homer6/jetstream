#include "writer/LogglyWriter.h"


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

    LogglyWriter::LogglyWriter( const ::jetstream::config::LogglyWriterConfig& config )
        :config(config)
    {

    }


	void LogglyWriter::run( const bool& keep_running ){

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


		// connect to loggly

			int destination_port = 443;
			string destination_hostname_host = "logs-01.loggly.com";

			httplib::SSLClient http_client( destination_hostname_host.c_str(), destination_port );

			const string post_path = "/bulk/" + config.getConfigSetting("destination_token") + "/tag/bulk/";

			httplib::Headers request_headers{
				{ "Host", "logs-01.loggly.com" },
				{ "User-Agent", "jetstream" }
			};


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
					            //   cout << "JetStream: message key: " + string(message.get_key()) << endl;
					            //}

					            const string payload = message.get_payload();


					            string request_body;

					            json json_object;
					            try{

					            	json_object = json::parse( payload );
					            	request_body = json_object.dump() + "\n";

					            }catch( const std::exception& e ){

					            	//cerr << "JetStream: failed to parse payload: " + string(e.what()) << endl;
							        string json_meta = "{\"@timestamp\":" + get_timestamp() + ",\"host\":\"" + config.getConfigSetting("hostname") + "\",\"source\":\"" + config.getConfigSetting("topic") + "\",\"prd\":\"" + config.getConfigSetting("product_code") + "\"";
					            	request_body = json_meta + ",\"log\":\"" + escape_to_json_string(payload) + "\"}\n";

					            }

					            batch_payload += request_body;


					            kafka_consumer.commit(message);

					        } // end message.get_error()

					    } //end foreach message



					    if( batch_payload.size() ){

			        		try{

		        				std::shared_ptr<httplib::Response> es_response = http_client.Post( post_path.c_str(), request_headers, batch_payload, "application/json" );

		        				if( es_response ){

			        				if( es_response->status >= 200 && es_response->status < 300 ){

										// Now commit the message (ack kafka)
							            //kafka_consumer.commit(message);

			        				}else{

			        					json bad_response_object = json::object();

			        					bad_response_object["description"] = "Loggly non-200 response.";
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

			                	cerr << "JetStream: failed to send log lines to loggly: " + string(e.what()) << endl;

			                }

					    }




				    } // end messages.size()


				}catch( std::exception &e ){

					cerr << "JetStream: general exception caught with loggly writer: " + string(e.what()) << endl;

				}



			} // end while run

			cout << "JetStream: exiting." << endl;

	}



}
}
