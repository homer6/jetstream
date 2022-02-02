#pragma once

#include "writer/Writer.h"
#include "config/LogglyWriterConfig.h"

namespace jetstream{
namespace writer{

    class LogglyWriter : public Writer {

        public:
            LogglyWriter( const ::jetstream::config::LogglyWriterConfig& config );
            virtual void run( const bool& keep_running ) override;

        protected:
            ::jetstream::config::LogglyWriterConfig config;

    };

}
}

