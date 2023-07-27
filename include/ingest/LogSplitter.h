#pragma once

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <string>
using std::string;

#include "json.hpp"
using json = nlohmann::json;


namespace jetstream{
namespace ingest{

	class LogSplitter{

        friend class LogSender;

	    public:
            void addLogEntry( const string& json_log_entry );
            void clear();
            bool empty() const;

        protected:
            map<string,json> logs;  //  { log_type, json::array() }

	};

}
}

