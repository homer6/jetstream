#pragma once

#include <string>
using std::string;

#include "json.hpp"
using json = nlohmann::json;

#include "server/model/Model.h"
using ::jetstream::server::model::Model;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include "server/search/SearchQuery.h"
using ::jetstream::server::search::SearchQuery;

namespace jetstream{
namespace server{
namespace collection {

    struct SearchableModelSet{

        virtual string getSearchIndexName() const = 0;
        virtual json getSearchIndexSchema() const = 0;
        virtual shared_ptr<vector<json>> getSearchModelsJson() const = 0;

        virtual SearchQuery getSearchQuery() const = 0;

        virtual string getCollectionPrefix() const = 0;

    };

}
}
}
