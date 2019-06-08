#include "KubeLauncherConfig.h"
#include "JetStream.h"



namespace jetstream{

	KubeLauncherConfig::KubeLauncherConfig( JetStream *jetstream )
		:JetStreamConfig(jetstream)
	{



	}


  	void KubeLauncherConfig::printHelp(){

  		this->jetstream->printHelpKube();

  	}


}
