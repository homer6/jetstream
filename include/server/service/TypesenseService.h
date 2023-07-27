#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include "json.hpp"
using json = nlohmann::json;


#include "client/TypesenseClient.h"
using ::jetstream::client::TypesenseClient;

#include "server/collection/SearchableModelSet.h"
using ::jetstream::server::collection::SearchableModelSet;

#include "server/search/SearchQuery.h"
using ::jetstream::server::search::SearchQuery;

namespace jetstream{
namespace server{
namespace service {

    class TypesenseService{

        public:
            TypesenseService( shared_ptr<TypesenseClient> typesense_client );

            void addModelSet( shared_ptr<SearchableModelSet> model_set );

            json searchDocuments( const SearchQuery& query ) const;


            void clearModelSets();
            void indexAll() const;

        protected:
            shared_ptr<TypesenseClient> typesense_client;
            vector< shared_ptr<SearchableModelSet> > model_sets;


    };

}
}
}
