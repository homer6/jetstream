#ifndef JETSTREAM_LOGZIO_WRITER_CONFIG_H
#define JETSTREAM_LOGZIO_WRITER_CONFIG_H

#include <string>
using std::string;

#include <map>
using std::map;

#include "JetStreamConfig.h"


namespace jetstream{

	class JetStream;

	class LogzioWriterConfig : public JetStreamConfig{

	    public:
	    	LogzioWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}




#endif //JETSTREAM_LOGZIO_WRITER_CONFIG_H
