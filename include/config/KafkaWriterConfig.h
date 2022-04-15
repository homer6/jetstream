#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace config{

	class KafkaWriterConfig : public JetStreamConfig{

	    public:
	    	KafkaWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}
}

