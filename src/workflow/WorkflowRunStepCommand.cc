#include "workflow/WorkflowRunStepCommand.h"

#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <chrono>
#include <ctime>

#include <utility>

// Add necessary includes
#include <iostream>
#include <sstream>


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <vector>
#include <sstream>

#include "CommandExecutor.h"
#include <iostream>
#include <map>


using std::string;

#include "Common.h"
using jetstream::args_holder;

#include "workflow/WorkflowRunStep.h"


namespace jetstream{
namespace workflow{



    WorkflowRunStepCommand::WorkflowRunStepCommand( const WorkflowRunStep& workflow_run_step, const json& workflow_run_step_command_json )
        :workflow_run_step(workflow_run_step), workflow_run_step_command_json(workflow_run_step_command_json)
    {

    }



    string WorkflowRunStepCommand::getFullCommand() const{

        // Extract command and parameters

        if( !workflow_run_step_command_json.contains("command") ){
            throw std::runtime_error("WorkflowRunStepCommand JSON must contain a 'command' key");
        }

        if( !workflow_run_step_command_json["command"].is_string() ){
            throw std::runtime_error("WorkflowRunStepCommand 'command' key must be a string");
        }

        std::string command = workflow_run_step_command_json["command"].get<std::string>();
        std::string full_command = command;

        // If there are parameters, append them to the command
        if( workflow_run_step_command_json.contains("parameters") ){

            if( !workflow_run_step_command_json["parameters"].is_object() ){
                throw std::runtime_error("WorkflowRunStepCommand 'parameters' key must be an object");
            }

            auto parameters = workflow_run_step_command_json["parameters"];
            for( auto it = parameters.begin(); it != parameters.end(); ++it ){
                full_command += " " + it.key() + " " + it.value().get<std::string>();
            }

        }

        return full_command;

    }



    std::map<string,string> WorkflowRunStepCommand::getEnvironmentVariables() const{

        std::map<string,string> env_vars;

        const json& workflow_run_step_json = this->workflow_run_step.workflow_run_step_json;

        // Extract environment variables
        if( workflow_run_step_json.contains("environment") ){

            if( !workflow_run_step_json["environment"].is_object() ){
                throw std::runtime_error("WorkflowRunStep 'environment' key must be an object");
            }

            auto environment = workflow_run_step_json["environment"];
            for( const auto& [key, value] : environment.items() ){
                env_vars[key] = value.get<std::string>();
            }
        }

        return env_vars;

    }



    WorkflowRunStepResult WorkflowRunStepCommand::run( const std::function<void()>& poll_service_callback ){

        const string full_command = this->getFullCommand();

        try{

            jetstream::CommandExecutor executor( full_command );

            // Add arguments
            // executor.addArgument("-la");
            // executor.addArgument("/home/user");

            // Set environment variables
            auto env_vars = this->getEnvironmentVariables();
            executor.addEnvironmentVariables( env_vars );

            // executor.addEnvironmentVariable("MY_VAR1", "value1");

            // Set stdout callback
            string all_output;

            executor.setStdoutCallback([&all_output](const std::string& output) {
                std::cout << "STDOUT: " << output;
                all_output += output;
            });

            // Set stderr callback
            executor.setStderrCallback([](const std::string& output) {
                std::cerr << "STDERR: " << output;
            });

            // Execute the command
            int exit_code = executor.execute( true, poll_service_callback );

            //std::cout << "Command exited with status: " << exit_code << std::endl;

            return WorkflowRunStepResult{ exit_code, all_output };
            
        }catch( const std::runtime_error& e ){

            std::cerr << "Execution failed: " << e.what() << std::endl;
            throw e;

        }


    }



}
}




