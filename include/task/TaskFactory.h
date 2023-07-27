#pragma once

#include "task/Task.h"
#include "config/JetStreamConfig.h"

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

#include <cppkafka/cppkafka.h>

namespace jetstream {
namespace task {

    struct TaskFactory{

        TaskFactory( const ::jetstream::config::JetStreamConfig& config, cppkafka::Producer& kafka_producer, cppkafka::MessageBuilder& message_builder );

        Task& create();

        const ::jetstream::config::JetStreamConfig& config;
        cppkafka::Producer& kafka_producer;
        cppkafka::MessageBuilder& message_builder;
        vector< unique_ptr<Task> > tasks;

    };

}
}

