#include "server/model/Model.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

namespace jetstream {
namespace server {
namespace model {

    Model::Model(){}

    Model::Model( const json& object )
        :fields(object)
    {



    }


    Model::Model( pqxx::row row ){

        //https://stackoverflow.com/questions/32135752/how-to-check-column-type-using-libpqxx
        //https://github.com/lib/pq/blob/21405079790728ed5ef4f890dd071b97e9713f9f/oid/types.go

        for( const pqxx::field& field : row ){

            if( !field.is_null() ){

                //if( string(field.name()) == "rip_score" )
                //cout << "Field: " << field.name() << " type: " << field.type() << endl;

                switch( field.type() ){
                    case 16: //bool
                        this->setValue<bool>( field.name(), field.as<bool>() );
                        break;

                    case 20: //int8 (64bit)
                        this->setValue<long>( field.name(), field.as<long>() );
                        break;

                    case 700: //T_float4 (32 bit)
                    case 701: //T__float8 (64 bit)
                    case 1231: //T__numeric
                    case 1700: //T_numeric
                        this->setValue<double>( field.name(), field.as<double>() );
                        break;

                    default:
                        this->setValue<string>( field.name(), string(field.c_str()) );
                }
            }

        }

    }



    Model::operator json() const{
        return this->fields;
    }


}
}
}