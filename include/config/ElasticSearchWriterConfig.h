#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace config{

	class ElasticSearchWriterConfig : public JetStreamConfig{

	    public:
	    	ElasticSearchWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}
}

