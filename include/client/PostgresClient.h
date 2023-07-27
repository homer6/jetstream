#pragma once

#include <string>
using std::string;

#include <pqxx/pqxx>

#include <memory>
using std::shared_ptr;

#include "server/collection/ModelSet.h"
using jetstream::server::collection::ModelSet;


namespace jetstream::client{


    class PostgresClient{

        public:
            PostgresClient( const string connection_string );


            template <class T>
            auto query( const char* query_char ){
                return this->query<T>( string(query_char) );
            }

            template <class T>
            auto query( const string& query ){
                pqxx::work transaction{ this->postgres_connection };
                auto results = std::make_shared< ModelSet<T> >( pqxx::result(transaction.exec(query)) );
                transaction.commit();
                return results;
            }



            template <class T>
            auto query( pqxx::work& transaction, const char* query_char ){
                return this->query<T>( transaction, string(query_char) );
            }

            template <class T>
            auto query( pqxx::work& transaction, const string& query ){
                return std::make_shared< ModelSet<T> >( pqxx::result(transaction.exec(query)) );
            }

            pqxx::connection postgres_connection;

    };


}