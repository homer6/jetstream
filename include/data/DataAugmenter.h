#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;


namespace jetstream{
namespace data{

    class DataAugmenter{

        public:
            virtual void setMaxPermutations( int max_permutations );
            virtual void setInputFile( const string& input_file );
            virtual void setOutputFile( const string& output_file );

            virtual string clean_values( const string& input ) const;

            virtual void setAppendOutput( bool append_output );
            virtual void setIncludeHeadersInOutput( bool include_headers_in_output );

            virtual void augment() = 0;

        protected:
            virtual void openOutfile();
            virtual vector<string> generate_permutations( const string& input ) const;

            int max_permutations = 30;
            string input_file;
            string output_file;

            bool append_output = true;
            bool include_headers_in_output = false;

            ofstream outfile;

    };

}
}


