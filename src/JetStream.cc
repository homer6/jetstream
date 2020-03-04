#include "JetStream.h"

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

#include "ElasticsearchWriterConfig.h"
#include "LogzioWriterConfig.h"
#include "LogglyWriterConfig.h"

#include "KubeLauncherConfig.h"



extern char **environ;


static jetstream::JetStream* jetstream_app_ptr;

static void signal_handler_stop( int sig ){
    
    jetstream_app_ptr->run = false;

    jetstream::Observer observer;

    switch( sig ){
    	case SIGINT: observer.addLogEntry( "jetstream: SIGINT received. Shutting down." ); break;
    	case SIGTERM: observer.addLogEntry( "jetstream: SIGTERM received. Shutting down." ); break;
    	default: observer.addLogEntry( "jetstream: Unknown signal received. Shutting down." );
    };
	
}

/*
static void signal_handler_reload_config( int sig ){

    jetstream_app_ptr->reload_required = true;
    jetstream::Observer observer;
	observer.addLogEntry( "jetstream: SIGHUP received. Reloading configuration." );

}*/


namespace jetstream{

	JetStream::JetStream()
		:run(true), current_version("0.1.0")
	{


	}

	JetStream::~JetStream(){


	}



	void JetStream::registerSignalHandlers(){

		jetstream_app_ptr = this;

        // Signal handler for clean shutdown 
        signal( SIGINT | SIGTERM, signal_handler_stop );
        //signal( SIGHUP, signal_handler_reload_config );

	}



    void JetStream::printHelp(){

		const char *help_message = 
"usage: jetstream [--version] [--help] <command> [<args>]\n"
"\n"
"These are common jetstream commands used in various situations:\n"
"\n"
"behave as a writer\n"
"   elasticsearch     Write from a kafka topic to Elasticsearch\n"
"   s3                Write from a kafka topic to s3 (not implemented)\n"
"   snowflake         Write from a kafka topic to Snowflake (not implemented)\n"
"   logzio            Write from a kafka topic to logz.io\n"
"   loggly            Write from a kafka topic to loggly\n"
"\n"
"behave as a reader\n"
"   http              HTTP to kafka topic\n"
"\n"
"behave as a processor\n"
"   filter            Kafka -> filter -> kafka\n"
"\n"
"Please see: https://github.com/homer6/jetstream to report issues \n"
"or view documentation.\n";

		cout << help_message << endl;

    }



    void JetStream::printVersion(){

    	cout << "jetstream version " << this->current_version << endl;

    }


	void JetStream::printHelpElasticsearch(){

		cerr << "Usage: jetstream elasticsearch [OPTION]... [TARGET_ELASTICSEARCH]\n"
				"Write from a kafka topic to Elasticsearch.\n"
				"\n"
				"Mandatory arguments to long options are mandatory for short options too.\n"
				"  -b, --brokers [BROKERS]                a csv list of kafka brokers\n"
				"                                         (optional; defaults to ENV JETSTREAM_BROKERS)\n"
				"  -c, --consumer-group [CONSUMER_GROUP]  the kafka consumer group shared among all consumers\n"
				"                                         (optional; defaults to ENV JETSTREAM_CONSUMER_GROUP)\n"
				"  -t, --topic [TOPIC]                    a destination kafka topic\n"
				"                                         (optional; defaults to ENV JETSTREAM_TOPIC)\n"
				"  -p, --product-code [PRODUCT_CODE]      a code identifying a part of your organization or product\n"
				"                                         (optional; defaults to ENV JETSTREAM_PRODUCT_CODE)\n"
				"  -h, --hostname [HOSTNAME]              the name of this host that will appear in the log entries\n"
				"                                         (optional; defaults to ENV JETSTREAM_HOSTNAME)\n"
				"  -dh, --destination-hostname [HOSTNAME] the destination elasticsearch hostname (eg. 'localhost:9200')\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_HOSTNAME)\n"
				"  -du, --destination-username [USERNAME] the destination elasticsearch username (eg. 'elastic')\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_USERNAME)\n"
				"  -dp, --destination-password [PASSWORD] the destination elasticsearch password (eg. 'pass123')\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_PASSWORD)\n"
				"  -di, --destination-index [INDEX]       the destination elasticsearch index\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_INDEX)\n"
				"  -ds, --destination-secure [SECURE]     whether the connection to the destination elasticsearch instance is secure\n"
				"                                         eg. ('true' or 'false'). Defaults to 'false'. \n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_SECURE)\n"
				"  -ph, --prom-hostname [HOSTNAME]        prometheus push gateway hostname (eg. 'push.prom.com:5000')\n"
				"                                         (optional; defaults to ENV JETSTREAM_PROM_HOSTNAME)\n"
				"  -ps, --prom-secure [SECURE]            whether to use TLS for prometheus push gateway\n"
				"                                         eg. ('true' or 'false'). Defaults to 'false'. \n"
				"                                         (optional; defaults to ENV JETSTREAM_PROM_SECURE)\n"
		<< endl;

	}




	void JetStream::printHelpLogzio(){

		cerr << "Usage: jetstream logzio [OPTION]...\n"
				"Write from a kafka topic to logz.io.\n"
				"\n"
				"Mandatory arguments to long options are mandatory for short options too.\n"
				"  -b, --brokers [BROKERS]                a csv list of kafka brokers\n"
				"                                         (optional; defaults to ENV JETSTREAM_BROKERS)\n"
				"  -c, --consumer-group [CONSUMER_GROUP]  the kafka consumer group shared among all consumers\n"
				"                                         (optional; defaults to ENV JETSTREAM_CONSUMER_GROUP)\n"
				"  -t, --topic [TOPIC]                    a destination kafka topic\n"
				"                                         (optional; defaults to ENV JETSTREAM_TOPIC)\n"
				"  -p, --product-code [PRODUCT_CODE]      a code identifying a part of your organization or product\n"
				"                                         (optional; defaults to ENV JETSTREAM_PRODUCT_CODE)\n"
				"  -h, --hostname [HOSTNAME]              the name of this host that will appear in the log entries\n"
				"                                         (optional; defaults to ENV JETSTREAM_HOSTNAME)\n"
				"  --token [LOGZ_IO_TOKEN]                the API token provided by logz.io for your account\n"
				"                                         (required; defaults to ENV JETSTREAM_DESTINATION_TOKEN)\n"
				"  -ph, --prom-hostname [HOSTNAME]        prometheus push gateway hostname (eg. 'push.prom.com:5000')\n"
				"                                         (optional; defaults to ENV JETSTREAM_PROM_HOSTNAME)\n"
				"  -ps, --prom-secure [SECURE]            whether to use TLS for prometheus push gateway\n"
				"                                         eg. ('true' or 'false'). Defaults to 'false'. \n"
				"                                         (optional; defaults to ENV JETSTREAM_PROM_SECURE)\n"
		<< endl;

	}



	void JetStream::printHelpLoggly(){

		cerr << "Usage: jetstream loggly [OPTION]...\n"
				"Write from a kafka topic to loggly (batch api).\n"
				"\n"
				"Mandatory arguments to long options are mandatory for short options too.\n"
				"  -b, --brokers [BROKERS]                a csv list of kafka brokers\n"
				"                                         (optional; defaults to ENV JETSTREAM_BROKERS)\n"
				"  -c, --consumer-group [CONSUMER_GROUP]  the kafka consumer group shared among all consumers\n"
				"                                         (optional; defaults to ENV JETSTREAM_CONSUMER_GROUP)\n"
				"  -t, --topic [TOPIC]                    a destination kafka topic\n"
				"                                         (optional; defaults to ENV JETSTREAM_TOPIC)\n"
				"  -p, --product-code [PRODUCT_CODE]      a code identifying a part of your organization or product\n"
				"                                         (optional; defaults to ENV JETSTREAM_PRODUCT_CODE)\n"
				"  -h, --hostname [HOSTNAME]              the name of this host that will appear in the log entries\n"
				"                                         (optional; defaults to ENV JETSTREAM_HOSTNAME)\n"
				"  --token [LOGGLY_TOKEN]                the API token provided by loggly for your account\n"
				"                                         (required; defaults to ENV JETSTREAM_DESTINATION_TOKEN)\n"
				"  -ph, --prom-hostname [HOSTNAME]        prometheus push gateway hostname (eg. 'push.prom.com:5000')\n"
				"                                         (optional; defaults to ENV JETSTREAM_PROM_HOSTNAME)\n"
				"  -ps, --prom-secure [SECURE]            whether to use TLS for prometheus push gateway\n"
				"                                         eg. ('true' or 'false'). Defaults to 'false'. \n"
				"                                         (optional; defaults to ENV JETSTREAM_PROM_SECURE)\n"
		<< endl;

	}



	void JetStream::printHelpKube(){

		cerr << "Usage: jetstream kube [OPTION]... [SUBCOMMAND]\n"
				"Launch jetstream in kubernetes.\n"
				"\n"
				"Mandatory arguments to long options are mandatory for short options too.\n"
				"  -b, --brokers [BROKERS]                a csv list of kafka brokers\n"
				"                                         (optional; defaults to ENV JETSTREAM_BROKERS)\n"
				"  -c, --consumer-group [CONSUMER_GROUP]  the kafka consumer group shared among all consumers\n"
				"                                         (optional; defaults to ENV JETSTREAM_CONSUMER_GROUP)\n"
				"  -t, --topic [TOPIC]                    a destination kafka topic\n"
				"                                         (optional; defaults to ENV JETSTREAM_TOPIC)\n"
				"  -p, --product-code [PRODUCT_CODE]      a code identifying a part of your organization or product\n"
				"                                         (optional; defaults to ENV JETSTREAM_PRODUCT_CODE)\n"
				"  -h, --hostname [HOSTNAME]              the name of this host that will appear in the log entries\n"
				"                                         (optional; defaults to ENV JETSTREAM_HOSTNAME)\n"
		<< endl;

	}



    int JetStream::runFromCommandLine( int argc, char **argv ){

    	int x = 0;

    	if( argc <= 1 ){
    		this->printHelp();
    		return 0;
    	}

    	while( x < argc ){
			this->command_line_arguments.push_back( string(argv[x]) );
			x++;
    	}

    	if( argc > 1 ){
    		this->command = this->command_line_arguments[1];
    	}

		this->loadEnvironmentVariables();


    	if( this->command == "-h" || this->command == "--help" || this->command == "help" ){
    		this->printHelp();
    		return 0;
    	}

    	if( this->command == "-v" || this->command == "--version" || this->command == "version" ){
    		this->printVersion();
    		return 0;
    	}




    	if( this->command == "elasticsearch" ){

            ElasticsearchWriterConfig config( this );
           
            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }

            this->runElasticsearchWriter( config );

            return 0;

    	}
    	



    	if( this->command == "logzio" ){

            LogzioWriterConfig config( this );
           
            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }

            const string this_token = config.getConfigSetting( "destination_token" );

			if( this_token == "TOKEN" || this_token == "" ){
				cout << "Error: Logz.io token is required." << endl;
				this->printHelpLogzio();
				return -1;
			}

			this->runLogzioWriter( config );

    		return 0;

    	}



    	if( this->command == "loggly" ){

            LogglyWriterConfig config( this );
           
            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }

            const string this_token = config.getConfigSetting( "destination_token" );

			if( this_token == "TOKEN" || this_token == "" ){
				cout << "Error: Loggly token is required." << endl;
				this->printHelpLoggly();
				return -1;
			}

			this->runLogglyWriter( config );

    		return 0;

    	}





        if( this->command == "kube" ){

            if( argc <= 2 ){
                this->printHelpKube();
                return -1;
            }

            KubeLauncherConfig config( this );
           
            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }


            vector<string> additional_arguments = config.getAdditionalArguments();

            if( additional_arguments.size() == 0 ){
                cout << "Error: Kube requires a subcommand." << endl;
                this->printHelpKube();
                return -1;
            }

            const string subcommand = additional_arguments[0];
            config.setConfigSetting( "subcommand", subcommand );

            this->runKube( config );

            return 0;

        }


    	this->printHelp();

    	return 1;

    }


    string JetStream::getEnvironmentVariable( const string& variable_name ) const{

    	if( this->environment_variables.count(variable_name) != 0 ){
    		return this->environment_variables.at(variable_name);
    	}

    	return "";

    }

    void JetStream::setEnvironmentVariable( const string& variable_name, const string& variable_value ){

    	this->environment_variables.insert( std::pair<string,string>(variable_name,variable_value) );

    }


	string JetStream::getDefaultTopic(){

		string default_topic_env = this->getEnvironmentVariable( "JETSTREAM_TOPIC" );
		if( default_topic_env.size() > 0 ){
			return default_topic_env;
		}

		return "jetstream_logs";

	}


	string JetStream::getDefaultBrokers(){

		string default_brokers_env = this->getEnvironmentVariable( "JETSTREAM_BROKERS" );
		if( default_brokers_env.size() > 0 ){
			return default_brokers_env;
		}

		return "localhost:9092";

	}


	string JetStream::getDefaultConsumerGroup(){

		string default_consumer_group_env = this->getEnvironmentVariable( "JETSTREAM_CONSUMER_GROUP" );
		if( default_consumer_group_env.size() > 0 ){
			return default_consumer_group_env;
		}

		return "default_consumer_group";

	}


	string JetStream::getDefaultProductCode(){

		string default_product_code_env = this->getEnvironmentVariable( "JETSTREAM_PRODUCT_CODE" );
		if( default_product_code_env.size() > 0 ){
			return default_product_code_env;
		}

		return "prd000";

	}


	string JetStream::getDefaultHostname(){

		string default_hostname_env = this->getEnvironmentVariable( "JETSTREAM_HOSTNAME" );
		if( default_hostname_env.size() > 0 ){
			return default_hostname_env;
		}

		return get_hostname();

	}


	string JetStream::getDefaultDestinationHostname(){

		string default_destination_hostname_env = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_HOSTNAME" );
		if( default_destination_hostname_env.size() > 0 ){
			return default_destination_hostname_env;
		}

		return "localhost:9200";

	}


	string JetStream::getDefaultDestinationUsername(){

		string default_destination_username_env = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_USERNAME" );
		if( default_destination_username_env.size() > 0 ){
			return default_destination_username_env;
		}

		return "username";

	}


	string JetStream::getDefaultDestinationPassword(){

		string default_destination_password_env = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_PASSWORD" );
		if( default_destination_password_env.size() > 0 ){
			return default_destination_password_env;
		}

		return "password";

	}


	string JetStream::getDefaultDestinationIndex(){

		string default_destination_index_env = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_INDEX" );
		if( default_destination_index_env.size() > 0 ){
			return default_destination_index_env;
		}

		return "my_logs";

	}


	string JetStream::getDefaultDestinationSecure(){

		string default_destination_secure_env = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_SECURE" );
		if( default_destination_secure_env.size() > 0 ){
			return default_destination_secure_env;
		}

		return "false";

	}


	string JetStream::getDefaultDestinationToken(){

		string default_token_env = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_TOKEN" );
		if( default_token_env.size() > 0 ){
			return default_token_env;
		}

		return "TOKEN";

	}




    string JetStream::getDefaultPrometheusPushGatewayHostname(){

        string default_prom_hostname_env = this->getEnvironmentVariable( "JETSTREAM_PROM_HOSTNAME" );
        if( default_prom_hostname_env.size() > 0 ){
            return default_prom_hostname_env;
        }

        return "localhost:9200";

    }

    string JetStream::getDefaultPrometheusPushGatewaySecure(){

        string default_prom_secure_env = this->getEnvironmentVariable( "JETSTREAM_PROM_SECURE" );
        if( default_prom_secure_env.size() > 0 ){
            return default_prom_secure_env;
        }

        return "false";

    }





	void JetStream::loadEnvironmentVariables(){

		int i = 0;
		while( environ[i] ){

			string environment_line( environ[i] ); // in the form of "variable=value"
			i++;

			std::string::size_type n = environment_line.find('=');

			if( n == std::string::npos ){
				//not found
				throw std::runtime_error("Unexpected environment format.");
			} else {				
				string variable_name = environment_line.substr(0, n);
				string variable_value = environment_line.substr(n + 1);
				this->environment_variables.insert( std::pair<string,string>(variable_name, variable_value) );
			}

		}

	}



	void JetStream::runElasticsearchWriter( JetStreamConfig& config ){

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
				{ "User-Agent", "jetstream-" + this->current_version }
			};

            const string destination_username = config.getConfigSetting( "destination_username" );
            const string destination_password = config.getConfigSetting( "destination_password" );
			if( destination_username.size() ){
				const string basic_auth_credentials = encodeBase64( destination_username + ":" + destination_password );
				request_headers.insert( { "Authorization", "Basic " + basic_auth_credentials } );
			}



		// consume from kafka
			while( this->run ){


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






	void JetStream::runLogzioWriter( JetStreamConfig& config ){

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


		// connect to logz.io

			int destination_port = 8071;
			string destination_hostname_host = "listener.logz.io";

			httplib::SSLClient http_client( destination_hostname_host.c_str(), destination_port );

			//int x = 0;


			const string post_path = "/?token=" + config.getConfigSetting("destination_token") + "&type=json";

			httplib::Headers request_headers{
				{ "Host", "listener.logz.io:8071" },
				{ "User-Agent", "jetstream-" + this->current_version }
			};


		// consume from kafka
			while( this->run ){


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

			        					bad_response_object["description"] = "Logz.io non-200 response.";
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

			                	cerr << "JetStream: failed to send log lines to logz.io: " + string(e.what()) << endl;

			                }

					    }




				    } // end messages.size()


				}catch( std::exception &e ){

					cerr << "JetStream: general exception caught with logz.io writer: " + string(e.what()) << endl;

				}



			} // end while run

			cout << "JetStream: exiting." << endl;

	}






	void JetStream::runLogglyWriter( JetStreamConfig& config ){

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

			//int x = 0;

			const string post_path = "/bulk/" + config.getConfigSetting("destination_token") + "/tag/bulk/";

			httplib::Headers request_headers{
				{ "Host", "logs-01.loggly.com" },
				{ "User-Agent", "jetstream-" + this->current_version }
			};


		// consume from kafka
			while( this->run ){


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












	void JetStream::runKube( JetStreamConfig& config ){


		// setup deployment

			json deployment = R"(
				{
				  "apiVersion": "apps/v1",
				  "kind": "Deployment",
				  "metadata": {
				    "name": "jetstream-deployment",
				    "labels": {
				      "app": "jetstream"
				    }
				  },
				  "spec": {
				    "replicas": 1,
				    "selector": {
				      "matchLabels": {
				        "app": "jetstream"
				      }
				    },
				    "template": {
				      "metadata": {
				        "labels": {
				          "app": "jetstream"
				        }
				      },
				      "spec": {
				        "containers": [
				          {
				            "name": "jetstream",
				            "image": "homer6/jetstream:latest",
				            "ports": [
				              {
				                "containerPort": 80
				              }
				            ],
                            "env": [
                            ]
				          }
				        ]
				      }
				    }
				  }
				}
			)"_json;

            
            const string brokers = config.getConfigSetting("brokers");
            const string topic = config.getConfigSetting("topic");
            const string hostname = config.getConfigSetting("hostname");
            const string consumer_group = config.getConfigSetting("consumer_group");
            const string product_code = config.getConfigSetting("product_code");
            const string subcommand = config.getConfigSetting("subcommand");

            const string destination_hostname = config.getConfigSetting("destination_hostname");
            const string destination_username = config.getConfigSetting("destination_username");
            const string destination_password = config.getConfigSetting("destination_password");
            const string destination_index = config.getConfigSetting("destination_index");
            const string destination_secure = config.getConfigSetting("destination_secure");
            const string destination_token = config.getConfigSetting("destination_token");

            const string prom_hostname = config.getConfigSetting("prom_hostname");
            const string prom_secure = config.getConfigSetting("prom_secure");

				
			json& labels = deployment["metadata"]["labels"];
			labels["product_code"] = product_code;
			labels["subcommand"] = subcommand;
				
			json& template_labels = deployment["spec"]["template"]["metadata"]["labels"];
			template_labels["product_code"] = product_code;
			template_labels["subcommand"] = subcommand;


			json& container1 = deployment["spec"]["template"]["spec"]["containers"][0];
			container1["command"] = json::array();
			container1["command"].push_back( subcommand );
			container1["args"] = json::array();



			json& env = container1["env"];

			map<string,string> env_temp;

			env_temp["LOGPORT_BROKERS"] = brokers;
			env_temp["LOGPORT_TOPIC"] = topic + ".logger";
			env_temp["LOGPORT_PRODUCT_CODE"] = product_code;
			env_temp["LOGPORT_HOSTNAME"] = hostname;
			
			env_temp["JETSTREAM_BROKERS"] = brokers;
			env_temp["JETSTREAM_CONSUMER_GROUP"] = consumer_group;
			env_temp["JETSTREAM_TOPIC"] = topic;
			env_temp["JETSTREAM_PRODUCT_CODE"] = product_code;
			env_temp["JETSTREAM_HOSTNAME"] = hostname;

			env_temp["JETSTREAM_DESTINATION_HOSTNAME"] = destination_hostname;
			env_temp["JETSTREAM_DESTINATION_USERNAME"] = destination_username;
			env_temp["JETSTREAM_DESTINATION_PASSWORD"] = destination_password;
			env_temp["JETSTREAM_DESTINATION_INDEX"] = destination_index;
            env_temp["JETSTREAM_DESTINATION_SECURE"] = destination_secure;
            env_temp["JETSTREAM_DESTINATION_TOKEN"] = destination_token;

            env_temp["JETSTREAM_PROM_HOSTNAME"] = prom_hostname;
			env_temp["JETSTREAM_PROM_SECURE"] = prom_secure;


			for( auto& env_t : env_temp ){
				env.push_back({
					{ "name", env_t.first },
					{ "value", env_t.second }
				});
			}


			cout << deployment.dump(2) << endl;

			//cout << "---" << endl;

			//deployment["metadata"]["name"] = "jetstream-deployment-2";

			//cout << deployment.dump(4) << endl;


	}












}
