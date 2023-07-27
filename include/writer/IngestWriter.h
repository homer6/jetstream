#pragma once

#include "writer/Writer.h"
#include "config/IngestWriterConfig.h"

#include <string>
using std::string;

#include "ingest/IngestWriterExporter.h"


namespace jetstream{
namespace writer{

    class IngestWriter : public Writer {

        public:
            IngestWriter( const ::jetstream::config::IngestWriterConfig& config );
            virtual void run( const bool& keep_running ) override;

        protected:
            ::jetstream::config::IngestWriterConfig config;
            ::jetstream::ingest::IngestWriterExporter exporter;

    };

}
}
