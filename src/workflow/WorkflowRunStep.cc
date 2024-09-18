#include "workflow/WorkflowRunStep.h"


namespace jetstream{
namespace workflow{


    WorkflowRunStep::WorkflowRunStep(){
    }

    WorkflowRunStep::WorkflowRunStep( json workflow_run_step_json )
        :workflow_run_step_json( workflow_run_step_json )
    {

    }

    void WorkflowRunStep::run( bool keep_running ){

        throw std::runtime_error("WorkflowRunStep::run() not implemented");

    }


}
}