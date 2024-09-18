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


namespace jetstream{
namespace workflow{



    WorkflowRunStepCommand::WorkflowRunStepCommand( shared_ptr<WorkflowRunStep> workflow_run_step )
        :workflow_run_step( workflow_run_step )
    {
    }



    string WorkflowRunStepCommand::getFullCommand() const{

        if( !this->workflow_run_step ){
            throw std::runtime_error("WorkflowRunStepCommand has no WorkflowRunStep");
        }

        const json& workflow_run_step_json = this->workflow_run_step->workflow_run_step_json;

        // Extract command and parameters
        std::string command = workflow_run_step_json["command"];
        std::string full_command = command;

        // If there are parameters, append them to the command
        if(workflow_run_step_json.contains("parameters")){
            auto parameters = workflow_run_step_json["parameters"];
            for(auto it = parameters.begin(); it != parameters.end(); ++it){
                full_command += " " + it.key() + " " + it.value().get<std::string>();
            }
        }

        return full_command;

    }



    std::map<string,string> WorkflowRunStepCommand::getEnvironmentVariables() const{

        std::map<string,string> env_vars;

        if( !this->workflow_run_step ){
            throw std::runtime_error("WorkflowRunStepCommand has no WorkflowRunStep");
        }

        const json& workflow_run_step_json = this->workflow_run_step->workflow_run_step_json;

        // Extract environment variables
        if( workflow_run_step_json.contains("environment") ){
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




