#ifndef JETSTREAM_JETSTREAM_H
#define JETSTREAM_JETSTREAM_H

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "Platform.h"
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

	        int runFromCommandLine( int argc, char **argv );
	        void registerSignalHandlers();

			void loadEnvironmentVariables();
            string getEnvironmentVariable( const string& variable_name ) const;
            void setEnvironmentVariable( const string& variable_name, const string& variable_value );

	        Observer& getObserver();

	        string getDefaultTopic();
	        string getDefaultBrokers();
	        string getDefaultProductCode();
	        string getDefaultHostname();


	        void runElasticsearchWriter();


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
