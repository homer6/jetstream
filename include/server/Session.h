#pragma once

#include <string>
using std::string;

#include <memory>
using std::shared_ptr;


namespace jetstream{
namespace server {

    class Session{

        public:
            Session( const string& session_id, long user_id );

            string getSessionId() const;
            long getUserId() const;

        protected:
            string session_id;
            long user_id = 0;

    };


    typedef shared_ptr<Session> session_ptr;

}
}