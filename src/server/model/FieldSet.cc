#include "../../../include/server/model/FieldSet.h"




namespace jetstream {
namespace server {
namespace model {

    Field::Field( double new_value )
        :type(json::value_t::number_float), value(new_value)
    {

    }

    Field::Field( long new_value )
        :type(json::value_t::number_integer), value(new_value)
    {

    }

    Field::Field( bool new_value )
        :type(json::value_t::boolean), value(new_value)
    {

    }

    Field::Field( const string& new_value )
        :type(json::value_t::string), value(new_value)
    {

    }


    Field::Field( const json& new_value )
        :type(new_value.type())
    {

        switch( new_value.type() ){
            case json::value_t::null:
                break;
            //case json::value_t::object:
            //case json::value_t::array:
                //skip
                //break;
            case json::value_t::number_integer:
            case json::value_t::number_unsigned:
                this->value = new_value.get<long>();
                break;
            case json::value_t::boolean:
                this->value = new_value.get<bool>();
                break;
            case json::value_t::string:
                this->value = new_value.get<string>();
                break;
            case json::value_t::number_float:
                this->value = new_value.get<double>();
                break;

            default:
                throw std::runtime_error("Type not implemented.");
        };


        /*
            case detail::value_t::null:
            case detail::value_t::object:
            case detail::value_t::array:
            case detail::value_t::string:
            case detail::value_t::boolean:
            case detail::value_t::number_integer:
            case detail::value_t::number_unsigned:
            case detail::value_t::number_float:
            case detail::value_t::binary:
            case detail::value_t::discarded:

         */


    }



    Field::operator json() const{

        switch( this->type ){
            case json::value_t::null:
            case json::value_t::object:
            case json::value_t::array:
                //skip
                break;
            case json::value_t::number_integer:
            case json::value_t::number_unsigned:
                return std::any_cast<long>(this->value);
                break;
            case json::value_t::boolean:
                return std::any_cast<bool>(this->value);
                break;
            case json::value_t::string:
                return std::any_cast<std::string>(this->value);
                break;
            case json::value_t::number_float:
                return std::any_cast<double>(this->value);
                break;
        };
        return nullptr;

    }






    FieldSet::FieldSet( const json& object ){

        if( !object.is_object() ){
            throw std::runtime_error("JSON Object expected.");
        }

        for( const auto& [key, value] : object.items() ){

            switch( value.type() ){
                case json::value_t::null:
                case json::value_t::object:
                case json::value_t::array:
                    //skip
                    break;
                case json::value_t::number_integer:
                case json::value_t::number_unsigned:
                    this->field_values.insert( std::pair<string,Field>( key, value.get<long>() ) );
                    this->field_names.push_back( key );
                    break;
                case json::value_t::boolean:
                    this->field_values.insert( std::pair<string,Field>( key, value.get<bool>() ) );
                    this->field_names.push_back( key );
                    break;
                case json::value_t::string:
                    this->field_values.insert( std::pair<string,Field>( key, value.get<string>() ) );
                    this->field_names.push_back( key );
                    break;
                case json::value_t::number_float:
                    this->field_values.insert( std::pair<string,Field>( key, value.get<double>() ) );
                    this->field_names.push_back( key );
                    break;
            };

        }

    }



}
}
}