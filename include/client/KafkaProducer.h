#pragma once

#include <string>
using std::string;

#include <librdkafka/rdkafka.h>

namespace jetstream{

    class Observer;


    /*
        Produces messages to kafka.
        The Observer object must exist for the lifetime of this KafkaProducer.
    */
    class KafkaProducer{

        public:
            KafkaProducer( Observer& observer, const string &brokers_list, const string &topic );
            ~KafkaProducer();

            void produce( const string& message ); //throws on failure

            void poll( int timeout_ms = 0 );

        protected:
            Observer& observer;

            string brokers_list;
            string topic;

            rd_kafka_t *rk;             /* Producer instance handle */
            rd_kafka_topic_t *rkt;      /* Topic object */
            
            char errstr[512];           /* librdkafka API error reporting buffer */

    };

}

