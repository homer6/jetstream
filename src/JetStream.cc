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
"   s3                Write from a kafka topic to s3\n"
"   snowflake         Write from a kafka topic to Snowflake\n"
"   logzio            Write from a kafka topic to logz.io\n"
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
				"  -di, --destination-index [INDEX]       the destination elasticsearch index\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_INDEX)\n"
				"  -ds, --destination-secure [SECURE]     whether the connection to the destination elasticsearch instance is secure\n"
				"                                         eg. ('true' or 'false'). Defaults to 'false'. \n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_SECURE)\n"
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
				"                                         (required; defaults to ENV JETSTREAM_LOGZIO_TOKEN)\n"
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

    		if( argc <= 2 ){

    			this->printHelpElasticsearch();

    			return -1;

    		}

    		int current_argument_offset = 2;

    		string this_brokers = this->getDefaultBrokers();
    		string this_consumer_group = this->getDefaultConsumerGroup();
    		string this_topic = this->getDefaultTopic();
    		string this_product_code = this->getDefaultProductCode();
    		string this_hostname = this->getDefaultHostname();

    		string destination_hostname = this->getDefaultDestinationHostname();
    		string destination_index = this->getDefaultDestinationIndex();
    		string destination_secure = this->getDefaultDestinationSecure();


    		bool is_last_argument = false;


    		while( current_argument_offset < argc ){

    			if( current_argument_offset == argc - 1 ){
    				is_last_argument = true;
    			}


    			string current_argument = this->command_line_arguments[ current_argument_offset ];


    			if( current_argument == "--topic" || current_argument == "--topics" || current_argument == "-t" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				this_topic = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--brokers" || current_argument == "--broker" || current_argument == "-b" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				this_brokers = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--consumer-group" || current_argument == "-c" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				this_consumer_group = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--product-code" || current_argument == "--prd" || current_argument == "-p" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				this_product_code = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--hostname" || current_argument == "-h" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				this_hostname = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}



    			if( current_argument == "--destination-hostname" || current_argument == "-dh" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				destination_hostname = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--destination-index" || current_argument == "-di" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				destination_index = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--destination-secure" || current_argument == "-ds" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				destination_secure = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpElasticsearch();
						return -1;
    				}
    				continue;

    			}



    			if( is_last_argument ){

    				//add kafka consumer and Elasticsearch writer here
    				this->runElasticsearchWriter( this_brokers, this_consumer_group, this_topic, this_product_code, this_hostname, destination_hostname, destination_index, destination_secure );
    				return 0;

    			}


    			current_argument_offset++;

    		}

    		return 0;

    	}  //end elasticsearch
    	






    	if( this->command == "logzio" ){

    		int current_argument_offset = 2;

    		string this_brokers = this->getDefaultBrokers();
    		string this_consumer_group = this->getDefaultConsumerGroup();
    		string this_topic = this->getDefaultTopic();
    		string this_product_code = this->getDefaultProductCode();
    		string this_hostname = this->getDefaultHostname();

    		string this_token = this->getDefaultLogzioToken();


    		bool is_last_argument = false;


    		while( current_argument_offset < argc ){

    			if( current_argument_offset == argc - 1 ){
    				is_last_argument = true;
    			}


    			string current_argument = this->command_line_arguments[ current_argument_offset ];


    			if( current_argument == "--topic" || current_argument == "--topics" || current_argument == "-t" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				this_topic = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--brokers" || current_argument == "--broker" || current_argument == "-b" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				this_brokers = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--consumer-group" || current_argument == "-c" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				this_consumer_group = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--product-code" || current_argument == "--prd" || current_argument == "-p" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				this_product_code = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--hostname" || current_argument == "-h" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				this_hostname = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				continue;

    			}

    			if( current_argument == "--token" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpLogzio();
						return -1;
    				}
    				this_token = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
    					is_last_argument = true;
    				}

    				if( !is_last_argument ){
    					continue;
    				}

    			}


    			if( is_last_argument ){

    				break;

    			}


    			current_argument_offset++;

    		}

			if( this_token == "TOKEN" ){
				cout << "Error: Logz.io token is required." << endl;
				this->printHelpLogzio();
				return -1;
			}


			//add kafka consumer and Elasticsearch writer here
			this->runLogzioWriter( this_brokers, this_consumer_group, this_topic, this_product_code, this_hostname, this_token );

    		return 0;

    	}  //end logzio









    	if( this->command == "kube" ){

    		if( argc <= 2 ){

    			this->printHelpKube();

    			return -1;

    		}

    		int current_argument_offset = 2;

    		string this_brokers = this->getDefaultBrokers();
    		string this_consumer_group = this->getDefaultConsumerGroup();
    		string this_topic = this->getDefaultTopic();
    		string this_product_code = this->getDefaultProductCode();
    		string this_hostname = this->getDefaultHostname();

    		bool is_last_argument = false;

    		while( current_argument_offset < argc ){

    			if( current_argument_offset == argc - 1 ){
    				is_last_argument = true;
    			}


    			string current_argument = this->command_line_arguments[ current_argument_offset ];


    			if( current_argument == "--topic" || current_argument == "--topics" || current_argument == "-t" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				this_topic = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--brokers" || current_argument == "--broker" || current_argument == "-b" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				this_brokers = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--consumer-group" || current_argument == "-c" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				this_consumer_group = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--product-code" || current_argument == "--prd" || current_argument == "-p" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				this_product_code = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				continue;

    			}


    			if( current_argument == "--hostname" || current_argument == "-h" ){

    				current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				this_hostname = this->command_line_arguments[ current_argument_offset ];

					current_argument_offset++;
    				if( current_argument_offset >= argc ){
						this->printHelpKube();
						return -1;
    				}
    				continue;

    			}



    			if( is_last_argument ){
    				
    				this->runKube( this_brokers, this_consumer_group, this_topic, this_product_code, this_hostname, current_argument );
    				return 0;

    			}


    			current_argument_offset++;

    		}

    		return 0;

    	}  //end kube
    	










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


	string JetStream::getDefaultLogzioToken(){

		string default_logzio_token_env = this->getEnvironmentVariable( "JETSTREAM_LOGZIO_TOKEN" );
		if( default_logzio_token_env.size() > 0 ){
			return default_logzio_token_env;
		}

		return "TOKEN";

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



	void JetStream::runElasticsearchWriter( const string& brokers, const string& consumer_group, const string& topic, const string& /*product_code*/, const string& /*hostname*/, const string& destination_hostname, const string& destination_index, const string& destination_secure ){

		//setup kafka consumer

			using cppkafka::Consumer;
			using cppkafka::Configuration;
			using cppkafka::Message;
			using cppkafka::TopicPartitionList;

			// Construct the configuration
				cppkafka::Configuration kafka_config = {
				    { "metadata.broker.list", brokers },
				    { "group.id", consumer_group },
				    // Disable auto commit
				    { "enable.auto.commit", false }
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
			    kafka_consumer.subscribe( { topic } );


		// connect to elasticsearch

			int destination_port = 9200;
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


			const string post_path = "/" + destination_index + "/_doc"; 

		// consume from kafka
			while( this->run ){

		        // Try to consume a message
		        Message message = kafka_consumer.poll();


			    if( message ){

			        // If we managed to get a message
			        if( message.get_error() ){

			            // Ignore EOF notifications from rdkafka
			            if( !message.is_eof() ){
			            	cerr << "JetStream: [+] Received error notification: " + message.get_error().to_string() << endl;
			            }

			        } else {

			            // Print the key (if any)
			            if( message.get_key() ){
			                cout << "JetStream: message key: " + string(message.get_key()) << endl;
			            }

			            const string payload = message.get_payload();



			            json json_object;
			            try{
			            	json_object = json::parse( payload );
			        		try{
			        			
			        			if( json_object.count("shipped_at") ){

			        				//cout << x++ << endl;

			        				std::shared_ptr<httplib::Response> es_response = http_client->Post( post_path.c_str(), json_object.dump(), "application/json");

			        				if( es_response ){

			        					if( es_response->status >= 200 && es_response->status < 300 ){
											// Now commit the message (ack kafka)
								            kafka_consumer.commit(message);
				        				}

			        				}


			        			}else{
			        				//ignore this message
					        		kafka_consumer.commit(message);
			        			}

			                }catch( const std::exception& e ){
			                	cerr << "JetStream: failed to apply object: " + string(e.what()) << endl;
			                }
			            }catch( const std::exception& e ){
			            	cerr << "JetStream: failed to parse payload: " + string(e.what()) << endl;
			            }

			        }

			    }


			}

			cout << "JetStream: exiting." << endl;

		//write to elasticsearch


	}






	void JetStream::runLogzioWriter( const string& brokers, const string& consumer_group, const string& topic, const string& /*product_code*/, const string& /*hostname*/, const string& token ){

		//setup kafka consumer

			using cppkafka::Consumer;
			using cppkafka::Configuration;
			using cppkafka::Message;
			using cppkafka::TopicPartitionList;

			// Construct the configuration
				cppkafka::Configuration kafka_config = {
				    { "metadata.broker.list", brokers },
				    { "group.id", consumer_group },
				    // Disable auto commit
				    { "enable.auto.commit", false }
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
			    kafka_consumer.subscribe( { topic } );


		// connect to logz.io

			int destination_port = 8071;
			string destination_hostname_host = "listener.logz.io";

			httplib::SSLClient http_client( destination_hostname_host.c_str(), destination_port );

			//int x = 0;


			const string post_path = "/?token=" + token + "&type=json";

			httplib::Headers request_headers{
				{ "Host", "listener.logz.io:8071" },
				{ "User-Agent", "jetstream-" + this->current_version }
			};


		// consume from kafka
			while( this->run ){

		        // Try to consume a message
		        //Message message = kafka_consumer.poll();


    			//std::vector<Message> poll_batch(size_t max_batch_size, std::chrono::milliseconds timeout);

				size_t max_batch_size = 10000;
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
				            if( message.get_key() ){
				                cout << "JetStream: message key: " + string(message.get_key()) << endl;
				            }

				            const string payload = message.get_payload();


				            string request_body;

				            json json_object;
				            try{

				            	json_object = json::parse( payload );
				            	request_body = json_object.dump() + "\n";

				            }catch( const std::exception& e ){

				            	cerr << "JetStream: failed to parse payload: " + string(e.what()) << endl;
				            	request_body = payload + "\n";

				            }

				            batch_payload += request_body;


				            kafka_consumer.commit(message);

				        } // end message.get_error()



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




			    	} //end foreach message



			    } // end messages.size()


			}

			cout << "JetStream: exiting." << endl;

	}












	void JetStream::runKube( const string& brokers, const string& consumer_group, const string& topic, const string& product_code, const string& hostname, const string& subcommand ){


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

			env_temp["JETSTREAM_DESTINATION_HOSTNAME"] = this->getDefaultDestinationHostname();
			env_temp["JETSTREAM_DESTINATION_INDEX"] = this->getDefaultDestinationIndex();
			env_temp["JETSTREAM_DESTINATION_SECURE"] = this->getDefaultDestinationSecure();

			env_temp["JETSTREAM_LOGZIO_TOKEN"] = this->getDefaultLogzioToken();


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
