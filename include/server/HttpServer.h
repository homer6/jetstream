#pragma once

#include "config/IngestWriterConfig.h"


namespace jetstream{
namespace server{

    class HttpServer{

        public:
            HttpServer( JetStream* jetstream_app, ::jetstream::config::IngestWriterConfig config );
            void run();

        protected:
            JetStream* jetstream_app = nullptr;
            ::jetstream::config::IngestWriterConfig config;
    };

}
}
