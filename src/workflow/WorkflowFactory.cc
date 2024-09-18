#include "workflow/WorkflowFactory.h"


namespace jetstream{
namespace workflow{

    WorkflowFactory::WorkflowFactory(){
    }

    WorkflowRun WorkflowFactory::createWorkflowRun( json workflow_run_json ){
        return WorkflowRun( workflow_run_json );
    }

}
}