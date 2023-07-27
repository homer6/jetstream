#include "data/DataAugmenter.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <filesystem>
namespace fs = std::filesystem;

#include <sstream>
#include <string>
#include <fstream>
using std::ofstream;
using std::ios;

#include "Common.h"

#include <algorithm>

#include "rapidcsv.h"

#include <random>



namespace jetstream{
namespace data{

    void DataAugmenter::setMaxPermutations( int max_permutations ){
        this->max_permutations = max_permutations;
    }

    void DataAugmenter::setInputFile( const string& input_file ){
        this->input_file = input_file;
    }

    void DataAugmenter::setOutputFile( const string& output_file ){
        this->output_file = output_file;
    }

    void DataAugmenter::setAppendOutput( bool append_output ){
        this->append_output = append_output;
    }

    void DataAugmenter::setIncludeHeadersInOutput( bool include_headers_in_output ){
        this->include_headers_in_output = include_headers_in_output;
    }


    void DataAugmenter::openOutfile(){

        if( this->append_output ){
            this->outfile.open( this->output_file, ios::out | ios::app );
        }else{
            this->outfile.open( this->output_file, ios::out | ios::trunc );
        }

        if( this->include_headers_in_output ){
            //headings
            //this->outfile << "label" << endl;
            this->outfile << "labels,input" << endl;
        }

    }


    string DataAugmenter::clean_values( const string& input ) const{

        string new_string = input;
        new_string = strip_string( new_string );
        new_string = replace_all_string( new_string, "(", "" );
        new_string = replace_all_string( new_string, ")", "" );
        new_string = replace_all_string( new_string, "-", " " );
        new_string = replace_all_string( new_string, "/", " " );
        new_string = replace_all_string( new_string, ",", " " );
        new_string = replace_all_string( new_string, ".", "" );
        new_string = replace_all_string( new_string, "  ", " " );
        new_string = replace_all_string( new_string, "\n", " " );
        new_string = replace_all_string( new_string, "\"", "" );
        new_string = to_lower_string( new_string );
        return new_string;

    };



    vector<string> DataAugmenter::generate_permutations( const string& input ) const{

        vector<string> output_vector;

        //split on " "; drop common words
        vector<string> new_vector;
        vector<string> parts = split_string(input, ' ');
        for( const auto& value : parts ){
            if( value == "a" || value == "of" || value == "or" || value == "and" || value == "for" || value == "in" || value == "&" ) continue;
            new_vector.push_back(value);
        }

        //std::next_permutation requires this to be sorted
        std::sort( new_vector.begin(), new_vector.end() );

        int x = 0;
        do{
            x++;
            if( x > this->max_permutations * 3 ) break;
            output_vector.push_back( explode_string(new_vector, ' ') );
        }while( std::next_permutation(new_vector.begin(), new_vector.end()) );

        if( x > this->max_permutations ){
            auto rng = std::default_random_engine{};
            std::shuffle( std::begin(output_vector), std::end(output_vector), rng );
            output_vector.resize( this->max_permutations );
        }

        return output_vector;

    };


}
}


