#include "workflow/WorkflowRun.h"
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

namespace jetstream{
namespace workflow{


    WorkflowRun::WorkflowRun(){
    }

    WorkflowRun::WorkflowRun( json workflow_run_json, const string handler_name )
        :workflow_run_json( workflow_run_json ), handler_name( handler_name )
    {

    }

    void WorkflowRun::addWorkflowRunStep( const WorkflowRunStep& workflow_run_step, int index ){
        if( index == -1 ){
            this->workflow_run_json["steps"].push_back( workflow_run_step.workflow_run_step_json );
        } else {
            this->workflow_run_json["steps"].insert( this->workflow_run_json["steps"].begin() + index, workflow_run_step.workflow_run_step_json );
        }
    }






    void WorkflowRun::run(){

        WorkflowRunStepResult result = WorkflowRunStepCommand(full_command).run();
        std::string output = result.output;
        int exit_code = result.exit_code;

        if (exit_code == 0) {
            // Command succeeded
            step["status"] = "completed";
            step["output"] = output;
        } else {
            // Command failed
            step["status"] = "failed";
            step["error"] = output;
            // Optionally, you can decide to stop the workflow
            if (!keep_running) {
                break;
            }
        }

    }


    void WorkflowRun::run( const bool& keep_running ){

        // Record the start time
        // auto workflow_start = std::chrono::system_clock::now();
        // std::time_t start_time = std::chrono::system_clock::to_time_t(workflow_start);
        // this->workflow_run_json["workflow_run"]["metadata"]["start_time"] = std::ctime(&start_time);

        // Iterate through each step
        for( auto& step : this->workflow_run_json["workflow_run"]["steps"] ){

            // Check if the step status is "starting" and the handler name matches

            if( step["status"] == "starting" && step["step"] == this->handler_name ){
                
                // Create a WorkflowRunStep object
                WorkflowRunStep workflow_run_step( step );

                // Run the step
                 workflow_run_step.run();



                // Update step status to "running"
                step["status"] = "running";
                // Record the step start time
                auto step_start = std::chrono::system_clock::now();
                std::time_t step_start_time = std::chrono::system_clock::to_time_t(step_start);
                step["start_time"] = std::ctime(&step_start_time);

                try {

                    // Execute the command and capture output
                    WorkflowRunStepResult result = WorkflowRunStepCommand( full_command ).run();

                    // Record the end time
                    auto step_end = std::chrono::system_clock::now();
                    std::time_t step_end_time = std::chrono::system_clock::to_time_t(step_end);
                    step["end_time"] = std::ctime(&step_end_time);

                    // Calculate duration
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(step_end - step_start).count();
                    step["duration"] = duration;

                    if( result.exit_code == 0 ){

                        // Command succeeded
                        step["status"] = "completed";
                        // Optionally, you can store the output
                        step["output"] = output;

                    }else{

                        // Command failed
                        step["status"] = "failed";
                        // Store the error output
                        step["error"] = output;
                        
                        // Optionally, you can decide to stop the workflow
                        if( !keep_running ){
                            break;
                        }

                    }

                }catch( const std::exception& e ){

                    // Handle exceptions during command execution
                    step["status"] = "failed";
                    step["error"] = string(e.what());
                    if( !keep_running ){
                        break;
                    }

                }

            }

        }

        // Record the workflow end time and duration
        // auto workflow_end = std::chrono::system_clock::now();
        // std::time_t end_time = std::chrono::system_clock::to_time_t(workflow_end);
        // this->workflow_run_json["workflow_run"]["metadata"]["end_time"] = std::ctime(&end_time);
        // auto total_duration = std::chrono::duration_cast<std::chrono::seconds>(workflow_end - workflow_start).count();
        // this->workflow_run_json["workflow_run"]["metadata"]["duration"] = total_duration;

        // Update the overall status
        bool any_failed = false;
        bool all_completed = true;
        for( auto& step : this->workflow_run_json["workflow_run"]["steps"] ){
            if(step["status"] == "failed"){
                any_failed = true;
            }
            if(step["status"] != "completed"){
                all_completed = false;
            }
        }

        if(any_failed){
            this->workflow_run_json["workflow_run"]["metadata"]["status"] = "failed";
        }
        else if(all_completed){
            this->workflow_run_json["workflow_run"]["metadata"]["status"] = "completed";
        }
        else{
            this->workflow_run_json["workflow_run"]["metadata"]["status"] = "running";
        }


    }





}
}