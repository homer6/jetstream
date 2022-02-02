#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace config{

	class KubeWriterConfig : public JetStreamConfig{

	    public:
	    	KubeWriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}
}


