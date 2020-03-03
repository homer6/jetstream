#include "LogglyWriterConfig.h"
#include "JetStream.h"



namespace jetstream{

	LogglyWriterConfig::LogglyWriterConfig( JetStream *jetstream )
		:JetStreamConfig(jetstream)
	{



	}


  	void LogglyWriterConfig::printHelp(){

  		this->jetstream->printHelpLoggly();

  	}


}
