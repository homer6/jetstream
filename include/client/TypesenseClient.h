#pragma once

#include "client/HttpConnection.h"
using jetstream::client::HttpConnection;

#include "json.hpp"
using json = nlohmann::json;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include "server/collection/SearchableModelSet.h"
using ::jetstream::server::collection::SearchableModelSet;

#include "server/model/SearchableModel.h"
using ::jetstream::server::model::SearchableModel;

#include "server/search/SearchQuery.h"
using ::jetstream::server::search::SearchQuery;


namespace jetstream{
namespace client {

    class TypesenseClient {

        public:
            TypesenseClient( const string& url, const string& token );

            json searchDocuments( const SearchQuery& query );

            void insertDocuments( const string& collection, const vector<json>& documents );

            void insertDocuments( shared_ptr<SearchableModelSet> model_set );

            void createCollection( shared_ptr<SearchableModelSet> model_set );
            void deleteCollection( shared_ptr<SearchableModelSet> model_set );

        protected:
            json makePostRequest( const string& path, const string& body );
            json makeGetRequest( const string& path );
            void makeDeleteRequest( const string& path );
            HttpConnection connection;

    };

}
}
