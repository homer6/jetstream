#include "config/ElasticSearchWriterConfig.h"
#include "JetStream.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;


namespace jetstream{
namespace config{

	ElasticSearchWriterConfig::ElasticSearchWriterConfig( JetStream *jetstream )
		:JetStreamConfig(jetstream)
	{



	}


  	void ElasticSearchWriterConfig::printHelp(){

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

}
}