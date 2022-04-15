#pragma once

#include "writer/Writer.h"
#include "config/KafkaWriterConfig.h"

namespace jetstream{
namespace writer{

    class KafkaWriter : public Writer {

        public:
            KafkaWriter( const ::jetstream::config::KafkaWriterConfig& config );
            virtual void run( const bool& keep_running ) override;

        protected:
            ::jetstream::config::KafkaWriterConfig config;

    };

}
}

