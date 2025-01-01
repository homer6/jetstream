#include "JetStream.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;

#include <unistd.h>
#include <signal.h>

#include "client/KafkaProducer.h"

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

#include "config/ElasticSearchWriterConfig.h"
#include "config/LogglyWriterConfig.h"
#include "config/KubeWriterConfig.h"
#include "config/IngestWriterConfig.h"
#include "config/S3WriterConfig.h"

#include "writer/ElasticSearchWriter.h"
#include "writer/LogglyWriter.h"
#include "writer/KubeWriter.h"
#include "writer/IngestWriter.h"
#include "writer/S3Writer.h"

#include "parser/GizmoParser.h"

#include "server/HttpServer.h"
#include "server/ApiServer.h"
#include <thread>


extern char **environ;


static jetstream::JetStream* jetstream_app_ptr;

static void signal_handler_stop( int sig ){
    
    jetstream_app_ptr->keep_running = false;

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



	void JetStream::registerSignalHandlers(){

		jetstream_app_ptr = this;

        // Signal handler for clean shutdown 
        signal( SIGINT | SIGTERM, signal_handler_stop );
        //signal( SIGHUP, signal_handler_reload_config );

	}



    void JetStream::printHelp(){

		const char *help_message = 
"usage: jetstream [--version] [--help] [--dry-run] <command> [<args>]\n"
"\n"
"These are common jetstream commands used in various situations:\n"
"\n"
"behave as a writer\n"
"   elasticsearch     Write from a kafka topic to Elasticsearch\n"
"   s3                Write from a kafka topic to s3 (not implemented)\n"
"   loggly            Write from a kafka topic to loggly\n"
"   ingest            Write from a kafka topic to Ingest HTTP\n"
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


        //run HttpServer (jetstream proxy)
        if( this->command == "server" ){

            config::IngestWriterConfig config( this );

            //int return_code = config.loadCommandLineArguments();
            config.loadCommandLineArguments();

            /*
            if( return_code != 0 ){
                return return_code;
            }

            if( config.dry_run ){
            	config.print();
            	return 0;
            }
             */

            JetStream *this_jetstream_ptr = this;
            std::thread http_server_thread([this_jetstream_ptr, config]() {
                jetstream::server::HttpServer http_server(this_jetstream_ptr, config);
                http_server.run();
            });
            http_server_thread.join();
            return 0;

        }


        if( this->command == "api-server" ){

            config::IngestWriterConfig config( this );

            //int return_code = config.loadCommandLineArguments();
            config.loadCommandLineArguments();

            /*
            if( return_code != 0 ){
                return return_code;
            }

            if( config.dry_run ){
            	config.print();
            	return 0;
            }
             */

            JetStream *this_jetstream_ptr = this;
            std::thread http_server_thread([this_jetstream_ptr, config]() {
                jetstream::server::ApiServer api_server(this_jetstream_ptr, config);
                api_server.run();
            });
            http_server_thread.join();
            return 0;

        }


        //run data processing job
        if( this->command == "data-job-1" ){

            config::IngestWriterConfig config( this );

            //int return_code = config.loadCommandLineArguments();
            config.loadCommandLineArguments();

            /*
            if( return_code != 0 ){
                return return_code;
            }

            if( config.dry_run ){
            	config.print();
            	return 0;
            }
             */

            parser::GizmoParser parser( config, this->command_line_arguments );
            parser.parse();
            return 0;

        }



    	if( this->command == "elasticsearch" ){

            config::ElasticSearchWriterConfig config( this );
           
            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }

            if( config.dry_run ){
            	config.print();
            	return 0;
            }

            writer::ElasticSearchWriter es_writer( config );
            es_writer.run( this->keep_running );

            return 0;

    	}




    	if( this->command == "loggly" ){

            config::LogglyWriterConfig config( this );
           
            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }

            const string this_token = config.getConfigSetting( "destination_token" );

			if( this_token == "TOKEN" || this_token == "" ){
				cout << "Error: Loggly token is required." << endl;
				config.printHelp();
				return -1;
			}

            if( config.dry_run ){
            	config.print();
            	return 0;
            }

            writer::LogglyWriter loggly_writer( config );
            loggly_writer.run( this->keep_running );

    		return 0;

    	}


    	if( this->command == "s3" ){

            config::S3WriterConfig config( this );

            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }

            const string this_bucket = config.getConfigSetting( "destination_bucket" );
			if( this_bucket == "BUCKET" || this_bucket == "" ){
				cout << "Error: S3 bucket is required." << endl;
				config.printHelp();
				return -1;
			}

            const string this_access_key_id = config.getConfigSetting( "destination_access_key_id" );
			if( this_access_key_id == "" ){
				cout << "Error: S3 Access Key ID is required." << endl;
				config.printHelp();
				return -1;
			}

            const string this_secret_access_key = config.getConfigSetting( "destination_secret_access_key" );
			if( this_secret_access_key == "" ){
				cout << "Error: S3 Secret Access Key is required." << endl;
				config.printHelp();
				return -1;
			}

            if( config.dry_run ){
            	config.print();
            	return 0;
            }

            writer::S3Writer s3_writer( config );
            s3_writer.run( this->keep_running );

    		return 0;

    	}



        if( this->command == "kube" ){

            config::KubeWriterConfig config( this );

            if( argc <= 2 ){
                config.printHelp();
                return -1;
            }

            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }


            vector<string> additional_arguments = config.getAdditionalArguments();

            if( additional_arguments.size() == 0 ){
                cout << "Error: Kube requires a subcommand." << endl;
                config.printHelp();
                return -1;
            }

            const string subcommand = additional_arguments[0];
            config.setConfigSetting( "subcommand", subcommand );

            if( config.dry_run ){
            	config.print();
            	return 0;
            }

            writer::KubeWriter kube_writer( config );
            kube_writer.run( this->keep_running );

            return 0;

        }




    	if( this->command == "ingest" ){

            config::IngestWriterConfig config( this );

            int return_code = config.loadCommandLineArguments();

            if( return_code != 0 ){
                return return_code;
            }

            if( config.dry_run ){
            	config.print();
            	return 0;
            }

            writer::IngestWriter ingest_writer( config );
            ingest_writer.run( this->keep_running );

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


	string JetStream::getDefaultTask(){

		string default_topic_env = this->getEnvironmentVariable( "JETSTREAM_TASK" );
		if( default_topic_env.size() > 0 ){
			return default_topic_env;
		}

		return "no_task";

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

		string default_hostname_env = this->getEnvironmentVariable( "HOSTNAME" );
		if( default_hostname_env.size() > 0 ){
			return default_hostname_env;
		}

		default_hostname_env = this->getEnvironmentVariable( "JETSTREAM_HOSTNAME" );
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


	string JetStream::getDefaultDestinationAuthUrl(){

		string default_auth_url = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_AUTH_URL" );
		if( default_auth_url.size() > 0 ){
			return default_auth_url;
		}

		return "";

	}


	string JetStream::getDefaultBucket(){

		string default_bucket = this->getEnvironmentVariable( "JETSTREAM_BUCKET" );
		if( default_bucket.size() > 0 ){
			return default_bucket;
		}

		return "BUCKET";

	}


	string JetStream::getDefaultAccessKeyId(){

		string default_value = this->getEnvironmentVariable( "JETSTREAM_ACCESS_KEY_ID" );
		if( default_value.size() > 0 ){
			return default_value;
		}

		return "";

	}


	string JetStream::getDefaultSecretAccessKey(){

		string default_value = this->getEnvironmentVariable( "JETSTREAM_SECRET_ACCESS_KEY" );
		if( default_value.size() > 0 ){
			return default_value;
		}

		return "";

	}



	string JetStream::getDefaultDestinationBrokers(){

		string default_value = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_BROKERS" );
		if( default_value.size() > 0 ){
			return default_value;
		}

		return "";

	}


	string JetStream::getDefaultDestinationTopic(){

		string default_value = this->getEnvironmentVariable( "JETSTREAM_DESTINATION_TOPIC" );
		if( default_value.size() > 0 ){
			return default_value;
		}

		return "";

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


    string JetStream::getDefaultPostgresUrl(){

        string default_postgres_url_env = this->getEnvironmentVariable( "JETSTREAM_POSTGRES_URL" );
        if( default_postgres_url_env.size() > 0 ){
            return default_postgres_url_env;
        }

        return "postgresql://username@localhost/dbname?connect_timeout=10&application_name=myapp&ssl=true";

    }


	string JetStream::getDefaultHandlerName(){

		string default_value = this->getEnvironmentVariable( "JETSTREAM_HANDLER_NAME" );
		if( default_value.size() > 0 ){
			return default_value;
		}

		return "na";

	}

    string JetStream::getDefaultSecurityProtocol() {
        string default_value = this->getEnvironmentVariable("JETSTREAM_SECURITY_PROTOCOL");
        if( default_value.size() > 0 ){
            return default_value;
        }
        return "SASL_SSL";
    }

    string JetStream::getDefaultSaslMechanisms() {
        string default_value = this->getEnvironmentVariable("JETSTREAM_SASL_MECHANISMS");
        if( default_value.size() > 0 ){
            return default_value;
        }
        return "SCRAM-SHA-512";
    }

    string JetStream::getDefaultSaslUsername() {
        string default_value = this->getEnvironmentVariable("JETSTREAM_SASL_USERNAME");
        if( default_value.size() > 0 ){
            return default_value;
        }
        return "scram-user";
    }

    string JetStream::getDefaultSaslPassword() {
        string default_value = this->getEnvironmentVariable("JETSTREAM_SASL_PASSWORD");
        if( default_value.size() > 0 ){
            return default_value;
        }
        return "super_secret_password";
    }

    string JetStream::getDefaultSslCaLocation() {
        string default_value = this->getEnvironmentVariable("JETSTREAM_SSL_CA_LOCATION");
        if( default_value.size() > 0 ){
            return default_value;
        }
        return "/path/to/kafka-certificate.crt";
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



}
