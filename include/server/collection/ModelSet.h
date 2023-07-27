#pragma once

#include <string>
using std::string;

#include "json.hpp"
using json = nlohmann::json;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include "server/model/FieldSet.h"

#include <pqxx/pqxx>

#include "rapidcsv.h"

namespace jetstream{
namespace server{
namespace collection {

    template <class T>
    struct ModelSet{

        ModelSet(){}

        ModelSet( std::shared_ptr<ModelSet<T>> model_set )
            :models(std::move(model_set->models))
        {

        }



        ModelSet( const json& object ){
            if( !object.is_array() ){
                throw std::runtime_error("JSON Array expected.");
            }
            for( const auto& value : object ){
                this->models.push_back( T(value) );
            }
        };

        ModelSet( pqxx::result result ){
            for( auto row : result ){
                this->addModel( T(row) );
            }
        }

        virtual ~ModelSet(){};

        virtual void addModel( const T& model ){
            this->models.push_back(model);
        }

        virtual void addModel( T&& model ){
            this->models.push_back( std::move(model) );
        }

        virtual void addModel( const json& object ){
            this->models.push_back( T(object) );
        }


        virtual operator json() const{

            json results = json::array();
            for( const auto& model : this->models ){
                results.push_back( json(model) );
            }
            return results;

        }


        virtual void loadFromCsv( const string& csv_contents ){


            //load the data from the input file; using maps and multimaps automatically sort
            auto separator_param = rapidcsv::SeparatorParams();
            separator_param.mQuotedLinebreaks = true;


            std::stringstream sstream( csv_contents );
            rapidcsv::Document csv_doc( sstream, rapidcsv::LabelParams(), separator_param, rapidcsv::ConverterParams(true) );

            vector<string> column_names = csv_doc.GetColumnNames();

            const size_t num_columns = column_names.size();
            const size_t num_rows = csv_doc.GetRowCount();


            for( size_t row_index = 0; row_index < num_rows; row_index++ ){

                json current_object = json::object();

                for( size_t col_index = 0; col_index < num_columns; col_index++ ){

                    string value = csv_doc.GetCell<string>( col_index, row_index );
                    const string column_name = column_names[col_index];

                    current_object[ column_name ] = value;

                }

                this->addModel( current_object );

            }

        }


        bool empty() const{

            return this->models.empty();

        }

        size_t size() const{

            return this->models.size();

        }


        void clear(){

            this->models.clear();

        }


        const T& at( size_t index ) const{

            if( this->models.empty() ){
                throw std::runtime_error( "Out of bounds." );
            }

            if( index < 0 || index > this->models.size() - 1 ){
                throw std::runtime_error( "Out of bounds." );
            }

            return this->models.at( index );

        }



        vector<T> models;

        typedef T value_type;

    };

}
}
}
