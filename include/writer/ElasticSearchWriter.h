#pragma once

#include "writer/Writer.h"
#include "config/ElasticSearchWriterConfig.h"

namespace jetstream{
namespace writer{

    class ElasticSearchWriter : public Writer {

        public:
            ElasticSearchWriter( const ::jetstream::config::ElasticSearchWriterConfig& config );
            virtual void run( const bool& keep_running ) override;

        protected:
            ::jetstream::config::ElasticSearchWriterConfig config;

    };

}
}

