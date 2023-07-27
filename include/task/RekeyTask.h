#pragma once

#include "task/Task.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

namespace jetstream {
namespace task {

    class RekeyTask : public Task{

        public:
            RekeyTask( cppkafka::Producer& kafka_producer, cppkafka::MessageBuilder& message_builder, const vector<string>& keys )
                :Task(kafka_producer, message_builder), keys(keys)
            {

            }

            virtual void run( const string& key, const string& payload ) override;

            vector<string> keys;

    };

}
}

