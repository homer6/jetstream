#include "writer/IngestWriter.h"


#include <iostream>
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;

#include <unistd.h>
#include <signal.h>

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


#include "ingest/LogSplitter.h"
#include "ingest/LogSender.h"

#include "stopwatch.h"

#include <thread>


namespace jetstream{
namespace writer{

    IngestWriter::IngestWriter( const ::jetstream::config::IngestWriterConfig& config )
        :config(config)
    {

    }


	void IngestWriter::run( const bool& keep_running ){


        //start the metrics server

            IngestWriter* ingest_writer_ptr = this;
            std::thread http_server_thread([ingest_writer_ptr](){
                ingest_writer_ptr->exporter.run();
            });


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


		// connect to ingest
            //this->ingest_connection = HttpConnection( config.getConfigSetting("destination_hostname") );


		// consume from kafka
            ingest::LogSplitter splitter;
            ingest::LogSender sender{ this->config, ingest_writer_ptr->exporter };

            const string user_id = config.getConfigSetting("user_id");


			while( keep_running ){

                precise_stopwatch stopwatch;

				try{

                    sender.updateJwtToken( user_id );
                    cout << "End updateJwtToken: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                    size_t poll_remaining = 256;
                    size_t tries_remaining = 1;


                    splitter.clear();
                    sender.poll();

                    while( poll_remaining > 10 ){

                        tries_remaining--;

                        size_t max_batch_size = poll_remaining;
                        std::chrono::milliseconds poll_timeout_ms{5};

                        cout << "End sender.poll(): " << stopwatch.elapsed_time_ms() << "ms" << endl;

                        vector<Message> messages = kafka_consumer.poll_batch(max_batch_size, poll_timeout_ms);
                        cout << "End poll_batch: " << stopwatch.elapsed_time_ms() << "ms" << endl;

                        size_t total_messages = messages.size();

                        if( total_messages > 0 ){

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

                                        json log_object = json::parse(payload);

                                        const string this_user_id = log_object.at("user_id").get<string>();

                                        if (this_user_id != user_id) {
                                            continue;
                                        }

                                        splitter.addLogEntry(payload);
                                        x++;
                                    }catch( std::exception &e ){
                                        cerr << "JetStream: failed to parse payload: " + string(e.what()) << endl;
                                    }

                                }

                                poll_remaining--;

                            }

                        } // end messages.size()

                        try{
                            if( total_messages > 0 ) kafka_consumer.commit();
                        }catch(std::exception& e){
                            cerr << "JetStream: general exception (inner) caught with ingest writer: " + string(e.what()) << endl;
                            cout << "End exception: " << stopwatch.elapsed_time_ms() << "ms" << endl;
                        }


                        if( tries_remaining < 1 ){
                            break;
                        }

                    } // poll_remaining

                    cout << "End splitter.addLogEntry(" << 256 - poll_remaining << "): " << stopwatch.elapsed_time_ms() << "ms" << endl;


                    if( !splitter.empty() ){
                        sender.send( splitter, user_id );  //nothrow; blocks until all log types have returned (retries batch once)
                        cout << "End sender.send(): " << stopwatch.elapsed_time_ms() << "ms" << endl;
                        //cout << "End kafka_consumer.commit(): " << stopwatch.elapsed_time_ms() << "ms" << endl;
                    }

                    //kafka_consumer.commit();

				}catch( std::exception &e ){

					cerr << "JetStream: general exception caught with ingest writer: " + string(e.what()) << endl;
                    cout << "End exception: " << stopwatch.elapsed_time_ms() << "ms" << endl;

				}



			} // end while run


			cout << "JetStream: exiting." << endl;

            this->exporter.http_server.stop();
            http_server_thread.join();

	}






}
}
