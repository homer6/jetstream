#pragma once

#include "writer/Writer.h"
#include "config/KubeWriterConfig.h"

namespace jetstream{
namespace writer{

    class KubeWriter : public Writer {

        public:
            KubeWriter( const ::jetstream::config::KubeWriterConfig& config );
            virtual void run( const bool& keep_running ) override;

        protected:
            ::jetstream::config::KubeWriterConfig config;

    };

}
}


