#include "server/search/SearchQuery.h"


namespace jetstream{
namespace server{
namespace search {

    SearchQuery::SearchQuery(){

    }


    SearchQuery::SearchQuery( const httplib::Params& params ){
        this->readHttpParameters( params );
    }



    void SearchQuery::readHttpParameters( const httplib::Params& params ){

        for( const auto& [key, value] : params ){

            if( key == "q" ){
                this->phrase = value;
            }

            if( key == "page" ){
                this->page_number = std::stoi( value );
            }

            if( key == "limit" ){
                this->limit = std::stoi( value );
            }

        }

        if( this->limit > 100 ) this->limit = 100;
        if( this->limit < 1 ) this->limit = 1;

        if( this->page_number > 1000000 ) this->page_number = 1000000;
        if( this->page_number < 1 ) this->page_number = 1;

        if( this->phrase.empty() ){
            this->phrase = "*";
        }

    }

}
}
}
