#ifndef JETSTREAM_LOGGLY_WRITER_CONFIG_H
#define JETSTREAM_LOGGLY_WRITER_CONFIG_H

#include <string>
using std::string;

#include <map>
using std::map;

#include "JetStreamConfig.h"


namespace jetstream{

	class JetStream;

	class LogglyWriterConfig : public JetStreamConfig{

	    public:
	    	LogglyWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}




#endif //JETSTREAM_LOGGLY_WRITER_CONFIG_H
