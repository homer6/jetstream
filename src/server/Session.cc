#include "server/Session.h"


namespace jetstream::server{


    Session::Session( const string& session_id, long user_id )
        :session_id(session_id), user_id(user_id)
    {


    }


    string Session::getSessionId() const{
        return this->session_id;
    }


    long Session::getUserId() const{
        return this->user_id;
    }


}
