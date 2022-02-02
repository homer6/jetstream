#pragma once

#include <string>
using std::string;

#include <map>
using std::map;

#include <vector>
using std::vector;


namespace jetstream{

	class JetStream;

    namespace config{

        class JetStreamConfig{

            public:
                JetStreamConfig( JetStream *jetstream );
                virtual ~JetStreamConfig();

                virtual string getConfigSetting( const string& key ) const;
                virtual void setConfigSetting( const string& key, const string& value );

                virtual void printHelp() = 0;
                virtual void print();

                virtual int loadCommandLineArguments();

                virtual map<string,string> getConfigSettings() const;
                virtual vector<string> getAdditionalArguments() const;

                JetStream *jetstream;
                map<string,string> config_settings;
                vector<string> additional_arguments;
                bool dry_run = false;

        };

    }

}
