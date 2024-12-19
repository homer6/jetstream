#include "workflow/WorkflowRunContext.h"


namespace jetstream{
namespace workflow{

    WorkflowRunContext::WorkflowRunContext(){
    }

    WorkflowRunContext::WorkflowRunContext( json workflow_run_json, const string handler_name )
        :workflow_run_json( workflow_run_json ), handler_name( handler_name )
    {

        /*
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
        */

    }


    void WorkflowRunContext::runSetup(){

        // download all input files

    }


    void WorkflowRunContext::runTeardown(){

        // upload all output files
        

    }

    WorkflowRunContext::~WorkflowRunContext(){

        // delete ephemeral files (input and output; don't delete the global input files)

    }


}
}