#include "server/service/PostgresService.h"


#include "server/model/Gizmo.h"
using jetstream::server::model::Gizmo;


#include <vector>
using std::vector;

#include <set>
using std::set;

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "Common.h"


namespace jetstream{
namespace server{
namespace service {


    PostgresService::PostgresService( shared_ptr<PostgresClient> postgres_client )
        :postgres_client(postgres_client)
    {

    }

    shared_ptr<GizmoSet> PostgresService::getGizmoById( long gizmo_id ){

        string query;

        {
            pqxx::work transaction{ this->postgres_client->postgres_connection };

            query = R"thisquery(

                SELECT * 
                FROM gizmos g
                WHERE g.id =

            )thisquery";

            query += pqxx::to_string(gizmo_id);

        }

        auto gizmo_set = std::make_shared<GizmoSet>( this->postgres_client->query<Gizmo>(query) );
        return gizmo_set;

    }




    void PostgresService::deleteAllGizmos( const string& gizmo_status ){

        {
            pqxx::work transaction{ this->postgres_client->postgres_connection };

            string query = R"thisquery(
                DELETE
                FROM gizmos
                WHERE status =
            )thisquery";
            query += transaction.quote(gizmo_status);

            transaction.exec0(query);
            transaction.commit();
        }


    }




    session_ptr PostgresService::getSessionBySessionId( const string& session_id ){

        //auto& conn = this->postgres_client->postgres_connection;
        pqxx::work transaction{ this->postgres_client->postgres_connection };

        const string query =
        "SELECT s.user_id user_id, s.organization_id organization_id, s.organization_slug organization_slug "
        "FROM sessions s "
        "WHERE s.id = " + transaction.quote(session_id);

        for( auto [user_id, organization_id, organization_slug] : transaction.query<long,long,string>(query) ){
            transaction.commit();
            return std::make_shared<Session>( session_id, user_id );
        }

        transaction.abort();

        throw std::runtime_error("Session not found.");

    }


}
}
}