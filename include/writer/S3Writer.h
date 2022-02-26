#pragma once

#include "writer/Writer.h"
#include "config/S3WriterConfig.h"

namespace jetstream{
namespace writer{

    class S3Writer : public Writer {

        public:
            S3Writer( const ::jetstream::config::S3WriterConfig& config );
            virtual void run( const bool& keep_running ) override;

        protected:
            ::jetstream::config::S3WriterConfig config;

    };

}
}

