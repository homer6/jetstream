#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <memory>

#include "httplib.h"
#include "Url.h"


namespace jetstream{
namespace client{

    //https will be supported by providing https in the scheme
    using http_client_ptr = std::unique_ptr<httplib::Client>;

    using settings_map = std::map<string,string>;

    // this is used as kind of a pre-computed set so that these values don't need to be computed on each message
    struct HttpConnection{

        HttpConnection();
        HttpConnection( const string& url_string );

        void setHeader( const string& key, const string& value );

        homer6::Url url;
        string hostname;
        string scheme_hostname;   //scheme + hostname (without path)
        string full_path_template;
        unsigned short port = 443;
        uint32_t batch_size = 100;
        bool compress = false;
        bool secure = false;
        http_client_ptr http_client;
        httplib::Headers request_headers;
        settings_map settings;
        vector<string> messages;

    };

}
}
