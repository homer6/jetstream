#include "observability/prometheus/Registry.h"


namespace jetstream::observability::prometheus{

    void Registry::addMetric( const Metric& metric ){

        const string& metric_name = metric.getName();
        if( metric_name.size() == 0 ){
            throw std::runtime_error("Cannot add metric with no name.");
        }

        std::scoped_lock<std::mutex> lock{ this->metrics_map_mutex };

        this->metrics.insert( { metric_name, metric } );

    }


    void Registry::incrementMetric( const string& metric_name, const map<string,string>& labels, double amount ){

        std::scoped_lock<std::mutex> lock{ this->metrics_map_mutex };

        if( this->metrics.count(metric_name) == 0 ){
            throw std::runtime_error("Metric not found.");
        }

        this->metrics.at(metric_name).increment( labels, amount );

    }



    ostream& operator<<( ostream& os, Registry& registry ){

        std::scoped_lock<std::mutex> lock{ registry.metrics_map_mutex };

        for( auto& [metric_name, metric] : registry.metrics ){
            os << metric;
        }
        return os;

    }

}