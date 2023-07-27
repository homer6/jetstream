#pragma once

#include <vector>
using std::vector;

#include <string>
using std::string;

#include "json.hpp"
using json = nlohmann::json;

#include "server/model/FieldSet.h"

#include <pqxx/pqxx>

namespace jetstream{
namespace server{
namespace model {

    struct Model{

        Model();
        Model( const json& object );
        Model( pqxx::row row );

        virtual ~Model(){};

        template <class T>
        void setValue( const string& field_name, const T& field_value ){
            this->fields.setValue<T>( field_name, field_value );
        }

        template<class T>
        T getValue( const string& field_name ) const{
            return this->fields.getValue<T>( field_name );
        }

        void erase( const string& field_name ){
            this->fields.erase(field_name);
        }

        size_t count( const string& field_name ) const{
            return this->fields.count(field_name);
        }

        void renameField( const string& existing_field_name, const string& new_field_name ){
            this->fields.renameField( existing_field_name, new_field_name );
        }

        vector<string> getFieldNames() const{
            return this->fields.getFieldNames();
        }

        virtual operator json() const;

        FieldSet fields;

    };

}
}
}
