#include "config/JetStreamConfig.h"
#include "JetStream.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;


namespace jetstream{
namespace config {

    JetStreamConfig::JetStreamConfig( JetStream *jetstream )
        :jetstream(jetstream)
    {

        this->setConfigSetting( "brokers", this->jetstream->getDefaultBrokers() );
        this->setConfigSetting( "consumer_group", this->jetstream->getDefaultConsumerGroup() );
        this->setConfigSetting( "topic", this->jetstream->getDefaultTopic() );
        this->setConfigSetting( "product_code", this->jetstream->getDefaultProductCode() );
        this->setConfigSetting( "hostname", this->jetstream->getDefaultHostname() );

        this->setConfigSetting( "destination_hostname", this->jetstream->getDefaultDestinationHostname() );
        this->setConfigSetting( "destination_username", this->jetstream->getDefaultDestinationUsername() );
        this->setConfigSetting( "destination_password", this->jetstream->getDefaultDestinationPassword() );
        this->setConfigSetting( "destination_index", this->jetstream->getDefaultDestinationIndex() );
        this->setConfigSetting( "destination_secure", this->jetstream->getDefaultDestinationSecure() );
        this->setConfigSetting( "destination_token", this->jetstream->getDefaultDestinationToken() );
        this->setConfigSetting( "destination_auth_url", this->jetstream->getDefaultDestinationAuthUrl() );

        this->setConfigSetting( "destination_bucket", this->jetstream->getDefaultBucket() );
        this->setConfigSetting( "destination_access_key_id", this->jetstream->getDefaultAccessKeyId() );
        this->setConfigSetting( "destination_secret_access_key", this->jetstream->getDefaultSecretAccessKey() );

        this->setConfigSetting( "destination_brokers", this->jetstream->getDefaultDestinationBrokers() );
        this->setConfigSetting( "destination_topic", this->jetstream->getDefaultDestinationTopic() );
        this->setConfigSetting( "task", this->jetstream->getDefaultTask() );

        this->setConfigSetting( "prom_hostname", this->jetstream->getDefaultPrometheusPushGatewayHostname() );
        this->setConfigSetting( "prom_secure", this->jetstream->getDefaultPrometheusPushGatewaySecure() );

        this->setConfigSetting( "postgres_url", this->jetstream->getDefaultPostgresUrl() );

        this->setConfigSetting( "handler_name", this->jetstream->getDefaultHandlerName() );

        this->setConfigSetting("security_protocol", this->jetstream->getDefaultSecurityProtocol());
        this->setConfigSetting("sasl_mechanisms", this->jetstream->getDefaultSaslMechanisms());
        this->setConfigSetting("sasl_username", this->jetstream->getDefaultSaslUsername());
        this->setConfigSetting("sasl_password", this->jetstream->getDefaultSaslPassword());
        this->setConfigSetting("ssl_ca_location", this->jetstream->getDefaultSslCaLocation());

    }


    JetStreamConfig::~JetStreamConfig() {

    }


    string JetStreamConfig::getConfigSetting(const string &key) const {

        if( this->config_settings.count(key) ){
            const string &value = this->config_settings.at( key );
            return value;
        }

        return "";

    }


    void JetStreamConfig::setConfigSetting(const string &key, const string &value) {

        if (key.size() == 0) {
            throw std::runtime_error( "Error: empty keys are not allowed." );
        }

        this->config_settings[key] = value;

    }


    void JetStreamConfig::print() {

        for (const auto &config_setting_pair: this->getConfigSettings()) {

            cout << config_setting_pair.first << ": " << config_setting_pair.second << endl;

        }


    }


    int JetStreamConfig::loadCommandLineArguments() {


        int argc = this->jetstream->command_line_arguments.size();

        int current_argument_offset = 2;


        while( current_argument_offset < argc ){

            string current_argument = this->jetstream->command_line_arguments[current_argument_offset];


            if( current_argument == "--topic" || current_argument == "--topics" || current_argument == "-t" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "topic", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--task" || current_argument == "-ta" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "task", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--brokers" || current_argument == "--broker" || current_argument == "-b" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "brokers", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--consumer-group" || current_argument == "-c" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }

                this->setConfigSetting( "consumer_group", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--product-code" || current_argument == "--prd" || current_argument == "-p" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "product_code", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--hostname" || current_argument == "-h" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "hostname", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--destination-hostname" || current_argument == "-dh" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "destination_hostname", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--destination-username" || current_argument == "-du" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "destination_username", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--destination-password" || current_argument == "-dp" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "destination_password", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--destination-index" || current_argument == "-di" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "destination_index", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--destination-secure" || current_argument == "-ds" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "destination_secure", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--destination-auth-url" || current_argument == "-dau" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }

                this->setConfigSetting( "destination_auth_url", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--token" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }

                this->setConfigSetting( "destination_token", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--prom-hostname" || current_argument == "-ph" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "prom_hostname", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--prom-secure" || current_argument == "-ps" ){

                current_argument_offset++;
                if (current_argument_offset >= argc) {
                    this->printHelp();
                    return -1;
                }
                this->setConfigSetting( "prom_secure", this->jetstream->command_line_arguments[current_argument_offset] );

                current_argument_offset++;
                continue;

            }


            if( current_argument == "--dry-run" ){

                current_argument_offset++;
                this->dry_run = true;
                continue;

            }

            this->additional_arguments.push_back( current_argument );

            current_argument_offset++;

        }

        return 0;


    }


    map <string, string> JetStreamConfig::getConfigSettings() const {

        return this->config_settings;

    }


    vector <string> JetStreamConfig::getAdditionalArguments() const {

        return this->additional_arguments;

    }


}
}
