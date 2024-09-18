#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include "args_holder.h"

namespace jetstream {

    class CommandExecutor {

        public:
            CommandExecutor();
            explicit CommandExecutor( const std::string& command );

            // Add command-line arguments
            void addArgument(const std::string& arg);
            void addArguments(const std::string& args);
            void addArguments(const std::vector<std::string>& args);

            // Add environment variables (key-value pairs)
            void addEnvironmentVariable(const std::string& key, const std::string& value);
            void addEnvironmentVariables(const std::map<std::string, std::string>& env_vars);

            // Set callbacks for stdout and stderr
            void setStdoutCallback(const std::function<void(const std::string&)>& callback);
            void setStderrCallback(const std::function<void(const std::string&)>& callback);

            // Execute the command
            // Throws exceptions on errors
            int execute(bool wait_for_completion = true);

        private:
            args_holder args;
            args_holder env;

            std::function<void(const std::string&)> stdout_callback;
            std::function<void(const std::string&)> stderr_callback;

    };

}
