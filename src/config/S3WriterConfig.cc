#include "config/S3WriterConfig.h"
#include "JetStream.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

namespace jetstream{
namespace config {

    S3WriterConfig::S3WriterConfig(JetStream *jetstream)
            : JetStreamConfig(jetstream) {


    }


    void S3WriterConfig::printHelp() {

		cerr << "Usage: jetstream s3 [OPTION]...\n"
				"Write from a kafka topic to s3.\n"
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
				"  -bu, --bucket [BUCKET]                 the name of this host that will appear in the log entries\n"
				"                                         (required; defaults to ENV JETSTREAM_BUCKET)\n"
				"  --access-key-id [ACCESS_KEY_ID]        the Access Key ID used for your AWS account.\n"
				"                                         (required; defaults to ENV JETSTREAM_ACCESS_KEY_ID)\n"
				"  <SecretAccessKey>                      the Secret Access Key used for this Access Key ID.\n"
				"                                         (required; defaults to ENV JETSTREAM_SECRET_ACCESS_KEY)\n"
		<< endl;

    }

}
}
