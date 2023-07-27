#include "ingest/LogSplitter.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

namespace jetstream{
namespace ingest{


    void LogSplitter::addLogEntry( const string& json_log_entry ){

        json json_object = json::parse( json_log_entry );
        string log_type = "unknown";

        if( json_object.count("log_type") ){
            log_type = json_object["log_type"].get<string>();
        }

        if( !this->logs.count(log_type) ){
            this->logs[log_type] = json::array();
        }


        if( json_object.count("log_obj") && json_object["log_obj"].is_object() ){
            this->logs.at(log_type).push_back( json_object["log_obj"] );
        }else{
            cout << "Log Object not found: " << json_object.dump() << endl;
        }

    }


    void LogSplitter::clear(){

        this->logs.clear();

    }

    bool LogSplitter::empty() const{

        return ( this->logs.size() == 0 );

    }


}
}

