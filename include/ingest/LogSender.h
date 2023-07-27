#pragma once
#include "ingest/LogSplitter.h"

#include "client/HttpConnection.h"
using jetstream::client::HttpConnection;

#include "config/IngestWriterConfig.h"
using jetstream::config::IngestWriterConfig;

#include "client/KafkaProducer.h"
using jetstream::client::KafkaProducer;

#include "thread_pool.hpp"

#include <map>
using std::map;

#include <mutex>


namespace jetstream{
namespace ingest{

    class LogSenderAttempt;
    class IngestWriterExporter;

	class LogSender{

	    public:
            LogSender( const ::jetstream::config::IngestWriterConfig& config, IngestWriterExporter& exporter );
            void send( const LogSplitter& splitter, const string& user_id ) noexcept;  //blocks

            const string getIngestApiPathByLogType( const string& log_type ) const;

            void updateJwtToken( const string& user_id );
            void invalidateJwtToken();

            HttpConnection& useConnection( const string& log_type );

            void poll( int timeout_ms = 0 );

            friend class LogSenderAttempt;

        protected:
            bool attemptSend();

            IngestWriterConfig config;
            KafkaProducer error_producer;
            KafkaProducer rejected_error_producer;

            map<string, HttpConnection> ingest_connections;   //log_type, connection
            string jwt_token;

            std::mutex jwt_token_mutex;
            //thread_pool pool{20};

            IngestWriterExporter& exporter;

	};

}
}

