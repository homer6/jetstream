#include "ingest/IngestWriterExporter.h"

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <sstream>

#include "json.hpp"
using json = nlohmann::json;


#include "Common.h"

namespace jetstream{
namespace ingest{

    IngestWriterExporter::IngestWriterExporter(){

        this->registry.addMetric( { "ingest_sink_http_requests_total", "Number of HTTP requests (batches * attempts) sent to the Ingest API." } );
        this->registry.addMetric( { "ingest_sink_batches_total", "Number of batches sent to the Ingest API." } );
        this->registry.addMetric( { "ingest_sink_events_total", "Number of individual events sent to the Ingest API." } );
        this->registry.addMetric( { "ingest_sink_rejected_events_total", "Number of individual events rejected by the Ingest API." } );
        this->registry.addMetric( { "ingest_sink_dropped_events_total", "Number of individual events dropped (not successfully accepted by the Ingest API). This includes rejected events and all other reasons for being dropped." } );
        this->registry.addMetric( { "ingest_sink_accepted_events_total", "Number of individual events successfully accepted by the Ingest API." } );
        this->registry.addMetric( { "ingest_sink_jwt_refresh_count", "Number of JWT refresh attempts." } );

        /*
            exporter.registry.incrementMetric( "ingest_sink_http_requests_total", {
                { "organization_id", organization_id },
                { "log_type", log_type }
            });
         */

    }



    void IngestWriterExporter::run(){

        cout << "starting jetstream ingest writer exporter" << endl;


        IngestWriterExporter* exporter = this;

        this->http_server.Get("/metrics", [exporter]( const httplib::Request&, httplib::Response& response ){

            std::ostringstream output_stream;
            output_stream << exporter->registry;

            string response_body = output_stream.str();
            response.set_content( response_body, "text/plain; version=0.0.4; charset=utf-8" );

        });

        this->http_server.listen("0.0.0.0", 8080);

    }

}
}
