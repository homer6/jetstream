#include "client/HttpConnection.h"


namespace jetstream{
namespace client {


    HttpConnection::HttpConnection(){

    }


    HttpConnection::HttpConnection( const string& url_string ){

        this->url = homer6::Url{ url_string };
        this->port = this->url.getPort();
        this->secure = this->url.isSecure();
        this->hostname = this->url.getHost();
        this->scheme_hostname = this->url.getScheme() + "://" + this->hostname + ":" + std::to_string(this->port) ;

        //initializes https if scheme is https; supports both http and https
        this->http_client = std::make_unique<httplib::Client>( this->scheme_hostname.c_str() );

        this->full_path_template = this->url.getFullPath();

        this->request_headers = {
            { "Host", this->hostname },
            { "User-Agent", "jetstream" }
        };

    }


    void HttpConnection::setHeader( const string& key, const string& value ){

        //update the first matching existing key (and return), if it exists
        //this effectively treats this multimap as a map, but we're okay with this
        for( auto& [existing_key, existing_value] : this->request_headers ){
            if( key == existing_key ){
                existing_value = value;
                return;
            }
        }

        this->request_headers.insert( { key, value } );

    }


}
}