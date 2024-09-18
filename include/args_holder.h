#pragma once

#include <string>
#include <vector>

namespace jetstream {

    class args_holder {

        public:
            args_holder();
            explicit args_holder(const std::string& full_command);
            ~args_holder();

            void addArg(const std::string& arg);
            void addArgs(const std::string& full_command);
            char** data();
            char* operator[](int index);
            int size() const;

        private:
            std::vector<std::string> args_strings;
            std::vector<char*> args_pointers;

            void updatePointers();
    };

} 

