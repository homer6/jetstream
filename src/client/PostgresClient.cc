#include "client/PostgresClient.h"

namespace jetstream::client{

    PostgresClient::PostgresClient( const string connection_string )
            :postgres_connection( connection_string )
    {



    }

}

