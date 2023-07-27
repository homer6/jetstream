#pragma once

#include <cppkafka/cppkafka.h>

#include <string>
using std::string;


namespace jetstream {
namespace task {

    struct Task{

        Task( cppkafka::Producer& kafka_producer, cppkafka::MessageBuilder& message_builder )
            :kafka_producer(kafka_producer), message_builder(message_builder)
        {

        }

        virtual void run( const string& key, const string& payload ) = 0;
        virtual ~Task(){};

        cppkafka::Producer& kafka_producer;
        cppkafka::MessageBuilder& message_builder;

    };

}
}
