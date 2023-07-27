#pragma once

#include <string>
using std::string;

#include <map>
using std::map;

#include "json.hpp"
using json = nlohmann::json;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include "server/model/Gizmo.h"
using ::jetstream::server::model::Gizmo;

#include "server/collection/ModelSet.h"
using ::jetstream::server::collection::ModelSet;

#include "server/collection/SearchableModelSet.h"
using ::jetstream::server::collection::SearchableModelSet;

#include "server/search/SearchQuery.h"
using ::jetstream::server::search::SearchQuery;

#include "Common.h"


namespace jetstream{
namespace server{
namespace collection {

    class GizmoSet : public ModelSet<Gizmo>, public SearchableModelSet {

        public:

            typedef std::shared_ptr<ModelSet<Gizmo>> modelset_gizmo_sp;

            using ModelSet<Gizmo>::ModelSet;

            GizmoSet( modelset_gizmo_sp modelset )
                :ModelSet<Gizmo>(modelset)
            {

            }


            virtual string getSearchIndexName() const override{
                return this->getCollectionPrefix() + "gizmos";
            }

            virtual json getSearchIndexSchema() const override{

                json schema = json::parse(R"schema_index(
                    {
                        "name": "gizmos",
                        "token_separators": ["_", "-", "&", ".", "*", "/"],
                        "fields": [
                            {"name": "id", "type": "int64" },
                            {"name": "name", "type": "string" },
                            {"name": "description", "type": "string", "optional": true },
                            {"name": "creation_date", "type": "string" },
                            {"name": "last_modified", "type": "string", "optional": true },
                            {"name": "color", "type": "string", "optional": true },
                            {"name": "size", "type": "string", "optional": true },
                            {"name": "owner_id", "type": "int64", "optional": true },
                            {"name": "status", "type": "string", "optional": true },
                            {"name": "location", "type": "string", "optional": true }
                        ]
                    }
                )schema_index");

                schema["name"] = this->getSearchIndexName();

                return schema;

            }





            virtual shared_ptr<vector<json>> getSearchModelsJson() const override{

                auto models_json = std::make_shared<vector<json>>();

                for(auto model : this->models){

                    json model_json = json(model);

                    json search_json = json::object();
                    search_json["id"] = model_json.at("id").get<long>();
                    search_json["name"] = model_json.at("name").get<string>();

                    auto set_optional_search_field = [&](const string& field_name){
                        try{
                            //optional
                            search_json[field_name] = model_json.at(field_name).get<string>();
                        }catch(std::exception& e){
                            // log the error if you want
                        }
                    };

                    set_optional_search_field("description");
                    set_optional_search_field("creation_date");
                    set_optional_search_field("last_modified");
                    set_optional_search_field("color");
                    set_optional_search_field("size");
                    set_optional_search_field("status");
                    set_optional_search_field("location");

                    try{
                        //optional
                        search_json["owner_id"] = model_json.at("owner_id").get<long>();
                    }catch(std::exception& e){
                        // log the error if you want
                    }

                    models_json->push_back(search_json);

                }

                return models_json;

            }


            virtual SearchQuery getSearchQuery() const override{

                SearchQuery query;
                query.collection = this->getSearchIndexName();
                query.search_fields = vector<string>({ "name", "description", "creation_date", "last_modified", "color", "size", "status", "location", "owner_id" });
                return query;

            };


            void loadFromXml( const string& xml_content );

            virtual string getCollectionPrefix() const override{
                return "";
            }



    };

}
}
}