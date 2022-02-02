#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace config{

	class LogzioWriterConfig : public JetStreamConfig{

	    public:
	    	LogzioWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}
}

