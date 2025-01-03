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
using std::cout;
using std::cerr;
using std::endl;


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

#include "workflow/WorkflowRunStepCommand.h"

#include "Common.h"


namespace jetstream{
namespace workflow{


    WorkflowRun::WorkflowRun(){

        this->workflow_run_json = {
            {"workflow_run", {
                {"metadata", {
                    {"start_time", ""},
                    {"end_time", ""},
                    {"duration", 0},
                    {"status", "starting"}
                }},
                {"steps", json::array()}
            }}
        };

    }

    WorkflowRun::WorkflowRun( json workflow_run_json, const string handler_name )
        :workflow_run_json( workflow_run_json ), handler_name( handler_name )
    {

        if( !this->workflow_run_json.is_object() ){
            throw std::runtime_error("WorkflowRun JSON must be an object");
        }

        if( !this->workflow_run_json.contains("workflow_run") ){
            throw std::runtime_error("WorkflowRun JSON must contain a 'workflow_run' key");
        }

        if( !this->workflow_run_json["workflow_run"].is_object() ){
            throw std::runtime_error("WorkflowRun JSON must contain a 'workflow_run' object");
        }

        if( !this->workflow_run_json["workflow_run"].contains("steps") ){
            throw std::runtime_error("WorkflowRun JSON must contain a 'steps' key");
        }

        if( !this->workflow_run_json["workflow_run"]["steps"].is_array() ){
            throw std::runtime_error("WorkflowRun JSON 'steps' key must be an array");
        }


    }





    void WorkflowRun::run( const bool& keep_running, const std::function<void()>& poll_service_callback ){

        // Record the start time
        // auto workflow_start = std::chrono::system_clock::now();
        // std::time_t start_time = std::chrono::system_clock::to_time_t(workflow_start);
        // this->workflow_run_json["workflow_run"]["metadata"]["start_time"] = std::ctime(&start_time);

        // Iterate through each step

        auto& steps = this->workflow_run_json["workflow_run"]["steps"];

        for( auto& step : steps ){

            // Check if the step status is "starting" and the handler name matches

            if( !step.contains("step") || !step["step"].is_string() ){
                throw std::runtime_error("WorkflowRunStep JSON must contain a 'step' key with a string value");
            }
            const string step_name = step["step"].get<string>();


            if( !step.contains("status") || !step["status"].is_string() ){
                throw std::runtime_error("WorkflowRunStep JSON must contain a 'status' key with a string value");
            }
            const string step_status = step["status"].get<string>();

            if( this->stepWasExecuted() ){
                // set the next step to "starting" and exit
                step["status"] = "starting";
                return;
            }

            if( step_status != "starting" ){
                cout << "Skipping step: " << step_name << " with status: " << step_status << endl;                
            }

            if( step_name != this->handler_name ){
                cout << "Skipping step: " << step_name << " with handler name mismatch. Step name: " << step_name << ", handler name: " << this->handler_name << endl;
            }

            if( step_status == "starting" && step_name == this->handler_name ){
                
                // Create a WorkflowRunStep object
                auto workflow_run_step = std::make_shared<WorkflowRunStep>(step);

                // Run the step
                //workflow_run_step.run();



                // Update step status to "running"
                step["status"] = "running";
                // Record the step start time
                auto step_start = std::chrono::system_clock::now();
                std::time_t step_start_time = std::chrono::system_clock::to_time_t(step_start);
                step["start_time"] = jetstream::get_timestamp();

                try{

                    WorkflowRunStepCommandListPtr commands = workflow_run_step->getCommands();

                    for( const auto& command : *commands ){
                        
                        // Execute the command and capture output
                        this->result = command->run( poll_service_callback );

                        cout << "Command output: " << this->result.output << endl;
                        cout << "Command exit code: " << this->result.exit_code << endl;

                        if( this->result.exit_code > 0 ){
                            // Command failed
                            step["status"] = "failed";
                            // Store the error output
                            step["error"] = this->result.output;

                            string full_command = command->getFullCommand();

                            throw std::runtime_error( "Command failed: " + full_command );
                        }

                        // Record the end time
                        auto step_end = std::chrono::system_clock::now();
                        std::time_t step_end_time = std::chrono::system_clock::to_time_t(step_end);
                        step["end_time"] = jetstream::get_timestamp();

                        // Calculate duration
                        auto duration = std::chrono::duration_cast<std::chrono::seconds>(step_end - step_start).count();
                        step["duration"] = duration;

                        if( this->result.exit_code == 0 ){

                            // Command succeeded
                            step["status"] = "completed";
                            // Optionally, you can store the output
                            step["output"] = this->result.output;

                        }else{

                            // Command failed
                            step["status"] = "failed";
                            // Store the error output
                            step["error"] = this->result.output;
                            
                            // Optionally, you can decide to stop the workflow
                            if( !keep_running ){
                                break;
                            }

                        }

                    }

                    /*
                        // Execute the command and capture output
                        WorkflowRunStepCommand workflow_run_step_command( workflow_run_step );
                        this->result = workflow_run_step_command.run( poll_service_callback );

                        cout << "Command output: " << this->result.output << endl;
                        cout << "Command exit code: " << this->result.exit_code << endl;

                        // Record the end time
                        auto step_end = std::chrono::system_clock::now();
                        std::time_t step_end_time = std::chrono::system_clock::to_time_t(step_end);
                        step["end_time"] = jetstream::get_timestamp();

                        // Calculate duration
                        auto duration = std::chrono::duration_cast<std::chrono::seconds>(step_end - step_start).count();
                        step["duration"] = duration;

                        if( this->result.exit_code == 0 ){

                            // Command succeeded
                            step["status"] = "completed";
                            // Optionally, you can store the output
                            step["output"] = this->result.output;

                        }else{

                            // Command failed
                            step["status"] = "failed";
                            // Store the error output
                            step["error"] = this->result.output;
                            
                            // Optionally, you can decide to stop the workflow
                            if( !keep_running ){
                                break;
                            }

                        }
                    */

                }catch( const std::exception& e ){

                    // Handle exceptions during command execution
                    step["status"] = "failed";
                    step["error"] = string(e.what());
                    if( !keep_running ){
                        break;
                    }

                }

                this->executed_step = std::make_shared<WorkflowRunStep>(step);                

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


    bool WorkflowRun::stepWasExecuted() const{

        return this->executed_step != nullptr;

    }

    json WorkflowRun::getWorkflowRunJson() const{

        return this->workflow_run_json;

    }

    WorkflowRunStepResult WorkflowRun::getResult() const{

        return this->result;

    }


    shared_ptr<WorkflowRunStep> WorkflowRun::getExecutedStep() const{

        return this->executed_step;

    }


}
}