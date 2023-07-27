#pragma once

#include <string>
using std::string;

#include <memory>
using std::shared_ptr;

#include "server/model/Model.h"
#include "server/model/SearchableModel.h"

#include "json.hpp"
using json = nlohmann::json;


namespace jetstream{
namespace server{
namespace collection{

    class ModelSetCollection;
}


namespace model {


    struct Gizmo : public Model, public SearchableModel {

        using Model::Model; //inherit constructors

    };

    typedef shared_ptr<Gizmo> gizmo_ptr;

}
}
}
