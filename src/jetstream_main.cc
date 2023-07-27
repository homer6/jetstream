#include <string.h>

#include <stdexcept>

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <string>
using std::string;


#include "JetStream.h"

#include "Common.h"


int main( int argc, char **argv ){

    try{

        //cout << jetstream::unescape_json_string("") << endl;
        //return 0;

        jetstream::JetStream jetstream_app;
        jetstream_app.registerSignalHandlers();
        return jetstream_app.runFromCommandLine( argc, argv );

    }catch( std::exception& e ){

        jetstream::Observer observer;
        string exception_message( "Fatal exception caught: " );
        exception_message += string( e.what() );
        observer.addLogEntry( exception_message );
        cerr << exception_message << endl;
        return -1;

    }catch( ... ){

        jetstream::Observer observer;
        string exception_message( "Unknown fatal exception caught." );
        observer.addLogEntry( exception_message );
        cerr << exception_message << endl;
        return -1;

    }

}

