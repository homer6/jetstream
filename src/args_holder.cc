#include "args_holder.h"
#include <sstream>

namespace jetstream {

    args_holder::args_holder() {
        // Default constructor
    }

    args_holder::args_holder( const std::string& full_command ){
        addArgs(full_command);
    }

    void args_holder::addArg( const std::string& arg ) {
        args_strings.push_back(arg);
        updatePointers();
    }

    void args_holder::addArgs( const std::string& full_command ){

        std::istringstream iss(full_command);
        std::string token;
        while (iss >> token) {
            args_strings.push_back(token);
        }
        updatePointers();

    }


    void args_holder::updatePointers(){

        args_pointers.clear();
        for (std::string& arg_str : args_strings) {
            args_pointers.push_back(const_cast<char*>(arg_str.c_str()));
        }
        args_pointers.push_back(nullptr);

    }


    char** args_holder::data(){
        return args_pointers.data();
    }

    char* args_holder::operator[]( int index ){
        return args_pointers[index];
    }

    int args_holder::size() const{
        return args_strings.size();
    }

    args_holder::~args_holder(){
        // No manual memory management required
    }

}