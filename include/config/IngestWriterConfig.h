#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace config{

	class IngestWriterConfig : public JetStreamConfig{

	    public:
	    	IngestWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}
}
