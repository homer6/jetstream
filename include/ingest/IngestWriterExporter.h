#pragma once

#include "observability/prometheus/Registry.h"
using jetstream::observability::prometheus::Registry;
using jetstream::observability::prometheus::Metric;

#include "httplib.h"

namespace jetstream{
namespace ingest{

    class IngestWriterExporter{

        public:
            IngestWriterExporter();
            void run();

            Registry registry;
            httplib::Server http_server;

    };

}
}