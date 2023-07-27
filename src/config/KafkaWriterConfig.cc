#include "config/KafkaWriterConfig.h"
#include "JetStream.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;


namespace jetstream{
namespace config{

	KafkaWriterConfig::KafkaWriterConfig( JetStream *jetstream )
		:JetStreamConfig(jetstream)
	{



	}


  	void KafkaWriterConfig::printHelp(){

		cerr << "Usage: jetstream kafka [OPTION]... [TARGET_KAFKA]\n"
				"Write from a kafka topic to another kafka topic.\n"
				"\n"
				"Mandatory arguments to long options are mandatory for short options too.\n"
				"  -b, --brokers [BROKERS]                a csv list of kafka brokers\n"
				"                                         (optional; defaults to ENV JETSTREAM_BROKERS)\n"
				"  -c, --consumer-group [CONSUMER_GROUP]  the kafka consumer group shared among all consumers\n"
				"                                         (optional; defaults to ENV JETSTREAM_CONSUMER_GROUP)\n"
				"  -t, --topic [TOPIC]                    a destination kafka topic\n"
				"                                         (optional; defaults to ENV JETSTREAM_TOPIC)\n"
				"  -ta, --task [TOPIC]                    a specific task to run\n"
				"                                         (optional; defaults to ENV JETSTREAM_TASK)\n"
				"  -p, --product-code [PRODUCT_CODE]      a code identifying a part of your organization or product\n"
				"                                         (optional; defaults to ENV JETSTREAM_PRODUCT_CODE)\n"
				"  -h, --hostname [HOSTNAME]              the name of this host that will appear in the log entries\n"
				"                                         (optional; defaults to ENV JETSTREAM_HOSTNAME)\n"
				"  -db, --destination-brokers [BROKERS]   the destination kafka brokers (eg. 'localhost:9200')\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_BROKERS)\n"
				"  -dt, --destination-topic [TOPIC]       the destination kafka topic\n"
				"                                         (optional; defaults to ENV JETSTREAM_DESTINATION_TOPIC)\n"
		<< endl;

  	}

}
}