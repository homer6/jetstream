#ifndef JETSTREAM_ELASTICSEARCH_WRITER_CONFIG_H
#define JETSTREAM_ELASTICSEARCH_WRITER_CONFIG_H

#include <string>
using std::string;

#include <map>
using std::map;

#include "JetStreamConfig.h"


namespace jetstream{

	class JetStream;

	class ElasticsearchWriterConfig : public JetStreamConfig{

	    public:
	    	ElasticsearchWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}




#endif //JETSTREAM_ELASTICSEARCH_WRITER_CONFIG_H
