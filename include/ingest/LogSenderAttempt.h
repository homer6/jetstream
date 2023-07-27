#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include <string>
using std::string;


namespace jetstream{
namespace ingest{

    class LogSender;

	class LogSenderAttempt{

	    public:
            LogSenderAttempt( LogSender* sender_ptr, const string& log_type, json& logs_json, const int attempt_number );
            bool send( const string& user_id ) noexcept;  //blocks; true on success

            string last_error_report;

        protected:
            json removeErroneousRecords( const json& response_body_json );

            LogSender* sender_ptr = nullptr;
            const string log_type;
            json& logs_json;
            const int attempt_number = 0;



	};

}
}

