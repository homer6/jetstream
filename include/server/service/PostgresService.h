#pragma once

#include <string>
using std::string;

#include "json.hpp"
using json = nlohmann::json;

#include <memory>
using std::shared_ptr;

#include "client/PostgresClient.h"
using ::jetstream::client::PostgresClient;

#include "server/collection/GizmoSet.h"
using ::jetstream::server::collection::GizmoSet;

#include "server/collection/ModelSet.h"
using ::jetstream::server::collection::ModelSet;

#include "server/model/Model.h"
using jetstream::server::model::Model;

#include "server/Session.h"
using jetstream::server::Session;

#include "server/model/Gizmo.h"
using jetstream::server::model::Gizmo;
using jetstream::server::model::gizmo_ptr;


#include <pqxx/pqxx>

namespace jetstream{
namespace server{
namespace service {

    class PostgresService{

        public:
            PostgresService( shared_ptr<PostgresClient> postgres_client );

            shared_ptr<GizmoSet> getGizmoById( long gizmo_id );
            void deleteAllGizmos( const string& organization_slug );

            session_ptr getSessionBySessionId( const string& session_id );

        protected:
            shared_ptr<PostgresClient> postgres_client;


    };

}
}
}
