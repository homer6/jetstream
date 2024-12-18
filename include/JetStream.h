#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "Observer.h"


namespace jetstream{

	class Watch;
	class Database;

	class Inspector;

    namespace config{
        class JetStreamConfig;
    }
    using config::JetStreamConfig;

	class JetStream{

	    public:
	        void printHelp();
	        void printVersion();

	        int runFromCommandLine( int argc, char **argv );
	        void registerSignalHandlers();

			void loadEnvironmentVariables();
            string getEnvironmentVariable( const string& variable_name ) const;
            void setEnvironmentVariable( const string& variable_name, const string& variable_value );

	        Observer& getObserver();

	        string getDefaultTopic();
	        string getDefaultBrokers();
	        string getDefaultConsumerGroup();
	        string getDefaultProductCode();
	        string getDefaultHostname();

	        string getDefaultDestinationHostname();
	        string getDefaultDestinationUsername();
	        string getDefaultDestinationPassword();
	        string getDefaultDestinationIndex();
	        string getDefaultDestinationSecure();
	        string getDefaultDestinationToken();
	        string getDefaultDestinationAuthUrl();

	        string getDefaultBucket();
	        string getDefaultAccessKeyId();
	        string getDefaultSecretAccessKey();
            string getDefaultDestinationBrokers();
            string getDefaultDestinationTopic();
            string getDefaultTask();

	        string getDefaultPrometheusPushGatewayHostname();
	        string getDefaultPrometheusPushGatewaySecure();

            string getDefaultPostgresUrl();

			string getDefaultHandlerName();


	     	bool keep_running = true;

	    	Observer observer;

	     	string command;
	     	vector<string> command_line_arguments;
	     	string current_version = "0.2.0";

	     	vector<string> additional_arguments;
	     	map<string,string> environment_variables;

	     	int log_level = 0;
    	

	};

}


