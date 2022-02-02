#include "writer/KubeWriter.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "json.hpp"
using json = nlohmann::json;


namespace jetstream{
namespace writer{

    KubeWriter::KubeWriter( const ::jetstream::config::KubeWriterConfig& config )
        :config(config)
    {

    }


	void KubeWriter::run( const bool& /*keep_running*/ ){


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
}

