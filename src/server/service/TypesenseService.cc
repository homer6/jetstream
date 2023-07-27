#include "server/service/TypesenseService.h"

#include "Common.h"


namespace jetstream{
namespace server{
namespace service {

    TypesenseService::TypesenseService( shared_ptr<TypesenseClient> typesense_client )
        :typesense_client(typesense_client)
    {


    }


    void TypesenseService::addModelSet( shared_ptr<SearchableModelSet> model_set ){

        this->model_sets.push_back( model_set );

    }


    json TypesenseService::searchDocuments( const SearchQuery& query ) const{

        return this->typesense_client->searchDocuments(query);

    }


    void TypesenseService::clearModelSets(){

        this->model_sets.clear();

    }



}
}
}

