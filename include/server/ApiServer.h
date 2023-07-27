#pragma once

#include "config/IngestWriterConfig.h"

#include <memory>
using std::shared_ptr;

#include <unordered_map>
using std::unordered_map;

#include <string>
using std::string;

#include "server/Session.h"
using jetstream::server::Session;
using jetstream::server::session_ptr;


#include "client/TypesenseClient.h"
using ::jetstream::client::TypesenseClient;

#include "client/PostgresClient.h"
using ::jetstream::client::PostgresClient;

#include "server/service/PostgresService.h"
using ::jetstream::server::service::PostgresService;

#include "server/service/TypesenseService.h"
using ::jetstream::server::service::TypesenseService;

#include <mutex>

#include "server/event/EventRouter.h"
using ::jetstream::server::event::EventRouter;




#include "httplib.h"

namespace jetstream{
namespace server{

    class ApiServer{

        public:
            ApiServer( JetStream* jetstream_app, ::jetstream::config::IngestWriterConfig config );
            void run();

            session_ptr getSessionFromRequest( const httplib::Request& request );
            string getCookieValue( const httplib::Request& request, const string& cookie_name ) const;

            std::mutex global_mutex;

            shared_ptr<TypesenseClient> typesense_client;
            shared_ptr<PostgresClient> postgres_client;

            shared_ptr<PostgresService> postgres_service;
            shared_ptr<TypesenseService> typesense_service;

        protected:
            JetStream* jetstream_app = nullptr;
            ::jetstream::config::IngestWriterConfig config;

            unordered_map<string, session_ptr> active_sessions; //key: session_id

            EventRouter event_router;

    };

}
}
