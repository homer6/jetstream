#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace config{

	class LogglyWriterConfig : public JetStreamConfig{

	    public:
	    	LogglyWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}
}
