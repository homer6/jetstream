#pragma once

#include <string>
using std::string;

#include "json.hpp"
using json = nlohmann::json;

namespace jetstream{
namespace server{
namespace model {

    struct SearchableModel{

        //virtual operator json() const = 0;

    };

}
}
}
