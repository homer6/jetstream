#ifndef JETSTREAM_KAFKA_PRODUCER_H
#define JETSTREAM_KAFKA_PRODUCER_H

#include <string>
using std::string;

#include <librdkafka/rdkafka.h>

namespace logport{

    class Observer;


    /*
        Produces messages to kafka.
        The Observer object must exist for the lifetime of this KafkaProducer.
    */
    class KafkaProducer{

        public:
            KafkaProducer( Observer& observer, const string &brokers_list, const string &topic, const string &undelivered_log );
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


#endif //JETSTREAM_KAFKA_PRODUCER_H
