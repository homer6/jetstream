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



    WorkflowRunStepResult WorkflowRunStepCommand::run(){

        const string full_command = this->getFullCommand();

        // do fork and exec with environment variables

        args_holder args{full_command};


        int pipefd[2];
        if( pipe(pipefd) == -1 ){
            throw std::runtime_error( "Failed to create pipe: " + std::string(strerror(errno)) );
        }

        pid_t pid = fork();

        if( pid == -1 ){

            throw std::runtime_error( "Fork failed: " + std::string(strerror(errno)) );

        }else if( pid == 0 ){
        
            // Child process
        
            close(pipefd[0]);  // Close read end of pipe
            dup2(pipefd[1], STDOUT_FILENO);
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);

            execvp(args[0], args.data());
            // If execvp returns, it must have failed
            std::cerr << "Exec failed: " << strerror(errno) << std::endl;
            exit(1);

        } else {
            
            // Parent process
            close(pipefd[1]);  // Close write end of pipe

            // Read output from child
            char buffer[4096];
            std::string output;
            ssize_t count;
            while( (count = read(pipefd[0], buffer, sizeof(buffer))) > 0 ){
                output.append(buffer, count);
            }
            close(pipefd[0]);

            // Wait for child to finish
            int status;
            waitpid( pid, &status, 0 );
            
            return WorkflowRunStepResult{ WIFEXITED(status) ? WEXITSTATUS(status) : -1, output };

        }
    

    }



}
}