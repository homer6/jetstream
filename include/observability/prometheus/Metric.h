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

namespace jetstream::observability::prometheus{


    class Metric {

        public:
            Metric( const string& metric_name, const string& help_message = "", const string& metric_type = "counter" );

            void increment( const map<string,string>& labels, double amount = 1.0 );
            void increment( const string& labels, double amount = 1.0 );

            friend ostream& operator<<(ostream& os, const Metric& metric);

            const string& getName() const;

        protected:

            string serializeLabels( const map<string,string>& labels ) const;

            string name;
            string help_message;
            string type;
            map<string, double> counters;  //labelset(sorted by key), counter

    };


    ostream& operator<<( ostream& os, const Metric& metric );

}

