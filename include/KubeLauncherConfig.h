#ifndef JETSTREAM_KUBE_LAUNCHER_CONFIG_H
#define JETSTREAM_KUBE_LAUNCHER_CONFIG_H

#include <string>
using std::string;

#include <map>
using std::map;

#include "JetStreamConfig.h"


namespace jetstream{

	class JetStream;

	class KubeLauncherConfig : public JetStreamConfig{

	    public:
	    	KubeLauncherConfig( JetStream *jetstream );
	    	virtual void printHelp();

	};

}




#endif //JETSTREAM_KUBE_LAUNCHER_CONFIG_H
