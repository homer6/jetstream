#include "writer/KafkaWriter.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Common.h"

#include <sys/wait.h>

#include "json.hpp"
using json = nlohmann::json;

#include <cppkafka/cppkafka.h>


#include <map>
using std::map;



namespace jetstream{
namespace writer{

    KafkaWriter::KafkaWriter( const ::jetstream::config::KafkaWriterConfig& config )
        :config(config)
    {

    }


	void KafkaWriter::run( const bool& keep_running ){

		//setup kafka consumer

			using cppkafka::Consumer;
			using cppkafka::Producer;
			using cppkafka::Configuration;
			using cppkafka::Message;
			using cppkafka::TopicPartitionList;

			// Construct the configuration
				cppkafka::Configuration kafka_consumer_config = {
				    { "metadata.broker.list", config.getConfigSetting("brokers") },
				    { "group.id", config.getConfigSetting("consumer_group") },
				    // Disable auto commit
				    { "enable.auto.commit", false },
				    { "auto.offset.reset", "latest" } //earliest or latest
				};

			// Create the consumer
    			cppkafka::Consumer kafka_consumer( kafka_consumer_config );

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


		// connect to destination kafka

            // Create the config
            cppkafka::Configuration kafka_producer_config = {
                { "metadata.broker.list", config.getConfigSetting("destination_brokers") }
            };

            // Create the producer
            cppkafka::Producer kafka_producer( kafka_producer_config );


            cppkafka::MessageBuilder message_builder( config.getConfigSetting("destination_topic") );

            // Get the partition we want to write to. If no partition is provided, this will be
            // an unassigned one
            //if (partition_value != -1) {
            //    message_builder.partition(partition_value);
            //}



		// consume from kafka
			while( keep_running ){


				try{

					// Try to consume a message
			        //Message message = kafka_consumer.poll();

					size_t max_batch_size = 20000;
					std::chrono::milliseconds poll_timeout_ms{1000};

					vector<Message> messages = kafka_consumer.poll_batch( max_batch_size, poll_timeout_ms );


				    if( messages.size() ){

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

    				            json json_object;
					            try{

					            	json_object = json::parse( payload );

                                    const string message_payload = json_object.dump();
                                    message_builder.payload( message_payload );
                                    kafka_producer.produce( message_builder );

					            }catch( const std::exception& e ){

					            	cerr << "JetStream: failed to parse payload: " + string(e.what()) << endl;
							        //string json_meta = "{\"@timestamp\":" + get_timestamp() + ",\"@ts\":\"" + get_timestamp("%Y/%m/%d %H:%M:%S") + "\",\"host\":\"" + config.getConfigSetting("hostname") + "\",\"source\":\"" + config.getConfigSetting("topic") + "\",\"prd\":\"" + config.getConfigSetting("product_code") + "\"";
					            	//request_body += json_meta + ",\"log\":\"" + escape_to_json_string(payload) + "\"}\n";

					            }

					            kafka_consumer.commit(message);

					        }

					    }

                        kafka_producer.flush();

				    }

				}catch( std::exception &e ){

					cerr << "JetStream: general exception caught with kafka writer: " + string(e.what()) << endl;

				}

			} // end while run

			cout << "JetStream: exiting." << endl;

	}



}
}
