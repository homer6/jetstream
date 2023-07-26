#pragma once

#include <string>
using std::string;

#include <map>
using std::map;

#include <iostream>
using std::ostream;
using std::cout;
using std::endl;
using std::cerr;

#include <mutex>

#include "observability/prometheus/Metric.h"

namespace jetstream::observability::prometheus{

    class Metric;

    class Registry{

        public:
            void addMetric( const Metric& metric );
            void incrementMetric( const string& metric_name, const map<string,string>& labels, double amount = 1.0 );

            friend ostream& operator<<(ostream& os, Registry& registry);

        protected:
            map<string, Metric> metrics;  //metric_name, Metric
            std::mutex metrics_map_mutex;

    };


    ostream& operator<<( ostream& os, Registry& registry );

}

