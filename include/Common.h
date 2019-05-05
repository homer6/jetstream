#ifndef JETSTREAM_COMMON_H
#define JETSTREAM_COMMON_H

#include <string>
using std::string;

#include <stdint.h>
#include <sys/types.h>

#include <sstream>

#include <vector>
using std::vector;

#include <iostream>
#include <iomanip>


namespace jetstream{

	string execute_command( const string& command );
	string execute_command( const string& command, int& exit_code );

	bool file_exists( const string& filename );

	string get_file_contents( const string& filepath);

	uint64_t get_file_size( const string& filepath );

	//gets the absolute path (resolves symlinks too)
	string get_real_filepath( const string& relative_filepath );

	string get_executable_filepath( const string& relative_filepath );


	vector<string> split_string( const string& source, char delimiter );


	string escape_to_json_string( const string& unescaped_string );


	//format: 1556311722.644052770
	string get_timestamp();


	// resource usage

	int proc_status_get_rss_usage_in_kb( pid_t pid );

	string proc_status_get_name( pid_t pid );


	// computer identification

    string get_hostname();



    template<class T> 
    string to_string( T input ){
		std::ostringstream stringstream;
		stringstream << input;
		return stringstream.str();
    };


}




#endif //JETSTREAM_COMMON_H
