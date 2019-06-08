#include "ElasticsearchWriterConfig.h"
#include "JetStream.h"



namespace jetstream{

	ElasticsearchWriterConfig::ElasticsearchWriterConfig( JetStream *jetstream )
		:JetStreamConfig(jetstream)
	{



	}


  	void ElasticsearchWriterConfig::printHelp(){

  		this->jetstream->printHelpElasticsearch();

  	}


}
