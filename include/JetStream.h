#ifndef JETSTREAM_JETSTREAM_H
#define JETSTREAM_JETSTREAM_H

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


	class JetStream{

	    public:
	    	JetStream();
	    	~JetStream();
	        
	        void printHelp();
	        void printVersion();
	        void printHelpElasticsearch();
	        void printHelpLogzio();
	        void printHelpKube();

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
	        string getDefaultDestinationIndex();
	        string getDefaultDestinationSecure();

	        string getDefaultLogzioToken();

	        void runElasticsearchWriter( const string& brokers, const string& consumer_group, const string& topic, const string& product_code, const string& hostname, const string& destination_hostname, const string& destination_index, const string& destination_secure );
	        void runLogzioWriter( const string& brokers, const string& consumer_group, const string& topic, const string& product_code, const string& hostname, const string& this_token );

	        void runKube( const string& brokers, const string& consumer_group, const string& topic, const string& product_code, const string& hostname, const string& subcommand );


	     	bool run;

	    private:
	    	Observer observer;

	     	string command;
	     	vector<string> command_line_arguments;
	     	string current_version;

	     	vector<string> additional_arguments;
	     	map<string,string> environment_variables;
    	

	};

}




#endif //JETSTREAM_JETSTREAM_H
