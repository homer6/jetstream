#include "LogzioWriterConfig.h"
#include "JetStream.h"



namespace jetstream{

	LogzioWriterConfig::LogzioWriterConfig( JetStream *jetstream )
		:JetStreamConfig(jetstream)
	{



	}


  	void LogzioWriterConfig::printHelp(){

  		this->jetstream->printHelpLogzio();

  	}


}
