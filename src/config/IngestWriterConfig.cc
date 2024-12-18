#include "config/IngestWriterConfig.h"
#include "JetStream.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;


namespace jetstream{
namespace config{

	IngestWriterConfig::IngestWriterConfig( JetStream *jetstream )
		:JetStreamConfig(jetstream)
	{



	}


  	void IngestWriterConfig::printHelp(){

		cerr << "Usage: jetstream ingest [OPTION]... [TARGET_INGEST]\n"
				"Write from a kafka topic to Ingest HTTP.\n"
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
				"  -di, --destination-index [INDEX]       the destination ingest endpoint\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_INDEX)\n"
				"  -ds, --destination-secure [SECURE]     whether the connection to the destination ingest instance is secure\n"
				"                                         eg. ('true' or 'false'). Defaults to 'true'. \n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_SECURE)\n"
				"  -dau, --destination-auth-url [URL]     the full url of the auth server (to recieve JWT tokens).\n"
				"                                         eg. ('https://target.hostname.com/api/user/login'). Defaults to ''. \n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_AUTH_URL)\n"
				"  -ha, --handler-name [NAME]             the name of the workflows handler.\n"
				"                                         (option; defaults to na)\n"
		<< endl;

  	}

}
}