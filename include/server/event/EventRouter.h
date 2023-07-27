#pragma once

#include <cppkafka/cppkafka.h>

#include "json.hpp"
using json = nlohmann::json;

namespace jetstream {
namespace server {

    class ApiServer;

namespace event {

    class EventRouter{

        public:
            void route( const json& event, ::jetstream::server::ApiServer* api_server_ptr, cppkafka::Producer& events_kafka_producer, cppkafka::MessageBuilder& events_message_builder );


    };

}
}
}