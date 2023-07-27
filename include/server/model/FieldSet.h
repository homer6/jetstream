#pragma once


#include <vector>
using std::vector;

#include <string>
using std::string;

#include <unordered_map>
using std::unordered_map;

#include <any>

#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <algorithm>

namespace jetstream {
namespace server {
namespace model {


    struct Field {

        Field(){};

        explicit Field( double new_value );
        explicit Field( long new_value );
        explicit Field( bool new_value );
        explicit Field( const string& new_value );
        explicit Field( const json& new_value );


        template<class T>
        T getValue() const{
            if( this->value.has_value() ){
                return std::any_cast<T>(this->value);
            }else{
                return T();
            }
        }

        json::value_t type;
        std::any value;

        operator json() const;

    };



    class FieldSet {

        public:
            FieldSet(){};
            FieldSet(const json& object);

            FieldSet(const vector<string> &field_names)
                :field_names(field_names)
            {

            }

            template<class T>
            void setValue( const string& field_name, const T& field_value ){
                this->field_values[field_name] = Field(field_value);
                this->addFieldName( field_name );
            }

            template<class T>
            T getValue( const string& field_name ) const{
                if( this->field_values.count(field_name) == 0 ){
                    throw std::runtime_error( field_name + " not found." );
                }
                return this->field_values.at(field_name).getValue<T>();
            }

            void renameField( const string& existing_field_name, const string& new_field_name ){
                if( this->field_values.count(existing_field_name) == 0 ){
                    throw std::runtime_error( existing_field_name + " not found." );
                }
                this->field_values[new_field_name] = this->field_values.at(existing_field_name);
                this->addFieldName( new_field_name );
                this->erase(existing_field_name);
            }

            void erase( const string& field_name ){
                this->field_values.erase(field_name);
                this->removeFieldName(field_name);
            }

            size_t count( const string& field_name ) const{
                return this->field_values.count(field_name);
            }

            operator json() const{

                json returned = json::object();
                for( const auto& [key, value] : this->field_values ){

                    //cout << "field_name: " << key << " value.type: " << int(value.type) << endl;

                    switch( value.type ){
                        case json::value_t::null:
                        case json::value_t::object:
                        case json::value_t::array:
                            //skip
                            break;
                        case json::value_t::number_integer:
                            returned[key] = value.getValue<long>();
                            break;
                        case json::value_t::boolean:
                            returned[key] = value.getValue<bool>();
                            break;
                        case json::value_t::string:
                            returned[key] = value.getValue<string>();
                            break;
                        case json::value_t::number_float:
                            returned[key] = value.getValue<double>();
                            break;
                    };

                }
                return returned;

            }


            const string asString() const{
                return json(*this).dump();
            }

            vector<string> getFieldNames() const{
                return this->field_names;
            }

        protected:
            void addFieldName( const string& field_name ){
                if( std::find(this->field_names.begin(), this->field_names.end(), field_name) == this->field_names.end() ){
                    //not found
                    this->field_names.push_back(field_name);
                }
            }

            void removeFieldName( const string& field_name ){
                this->field_names.erase( std::remove(this->field_names.begin(), this->field_names.end(), field_name), this->field_names.end() );
            }

            vector<string> field_names; //maintain order of field names
            unordered_map<string, Field> field_values;
    };


}
}
}