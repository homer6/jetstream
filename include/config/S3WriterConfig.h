#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace config{

	class S3WriterConfig : public JetStreamConfig{

	    public:
	    	S3WriterConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}
}
