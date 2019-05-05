#include "Observer.h"

#include "Common.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;


namespace jetstream{


	Observer::Observer(){

	
	
	}

	Observer::~Observer(){


	}

	void Observer::addMetricEntry( const string& metric_entry ){

        size_t entry_length = metric_entry.size();

        if( entry_length == 0 ){
            return;
        }

        string json_meta = "{\"generated_at\":" + get_timestamp();

        //unstructured single-line entry
            if( metric_entry[0] != '{' ){
                cout << json_meta + ",\"metric\":\"" + escape_to_json_string(metric_entry) + "\"}" << endl;
            }

        //embedded single-line JSON
            if( metric_entry[0] == '{' ){
                //this embedded single-line JSON MUST begin and end with a brace
                cout << json_meta + ",\"metric\":" + metric_entry + "}" << endl;
            }

	}


	void Observer::addEventEntry( const string& event_entry ){

        size_t entry_length = event_entry.size();

        if( entry_length == 0 ){
            return;
        }

        string json_meta = "{\"generated_at\":" + get_timestamp();

        //unstructured single-line entry
            if( event_entry[0] != '{' ){
                cout << json_meta + ",\"event\":\"" + escape_to_json_string(event_entry) + "\"}" << endl;
            }

        //embedded single-line JSON
            if( event_entry[0] == '{' ){
                //this embedded single-line JSON MUST begin and end with a brace
                cout << json_meta + ",\"event\":" + event_entry + "}" << endl;
            }


	}


	void Observer::addTraceEntry( const string& trace_entry ){

        size_t entry_length = trace_entry.size();

        if( entry_length == 0 ){
            return;
        }

        string json_meta = "{\"generated_at\":" + get_timestamp();

        //unstructured single-line entry
            if( trace_entry[0] != '{' ){
                cout << json_meta + ",\"trace\":\"" + escape_to_json_string(trace_entry) + "\"}" << endl;
            }

        //embedded single-line JSON
            if( trace_entry[0] == '{' ){
                //this embedded single-line JSON MUST begin and end with a brace
                cout << json_meta + ",\"trace\":" + trace_entry + "}" << endl;
            }

	}


	void Observer::addTelemetryEntry( const string& telemetry_entry ){

        size_t entry_length = telemetry_entry.size();

        if( entry_length == 0 ){
            return;
        }

        string json_meta = "{\"generated_at\":" + get_timestamp();

        //unstructured single-line entry
            if( telemetry_entry[0] != '{' ){
                cout << json_meta + ",\"telemetry\":\"" + escape_to_json_string(telemetry_entry) + "\"}" << endl;
            }

        //embedded single-line JSON
            if( telemetry_entry[0] == '{' ){
                //this embedded single-line JSON MUST begin and end with a brace
                cout << json_meta + ",\"telemetry\":" + telemetry_entry + "}" << endl;
            }

	}


	void Observer::addLogEntry( const string& log_line ){

        size_t entry_length = log_line.size();

        if( entry_length == 0 ){
            return;
        }

        string json_meta = "{\"generated_at\":" + get_timestamp();

        //unstructured single-line entry
            if( log_line[0] != '{' ){
                cout << json_meta + ",\"log\":\"" + escape_to_json_string(log_line) + "\"}" << endl;
            }

        //embedded single-line JSON
            if( log_line[0] == '{' ){
                //this embedded single-line JSON MUST begin and end with a brace
                cout << json_meta + ",\"log\":" + log_line + "}" << endl;
            }

	}


	    	




}
