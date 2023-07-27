#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "httplib.h"

namespace jetstream{
namespace server{
namespace search {

    struct SearchQuery{

        SearchQuery();
        SearchQuery( const httplib::Params& params );

        string collection;
        string phrase;

        int page_number = 1;
        int limit = 20;

        vector<string> search_fields;


        void readHttpParameters( const httplib::Params& params );


    };

}
}
}
