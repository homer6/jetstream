#pragma once

#include <memory>
using std::shared_ptr;

#include <string>
using std::string;

#include "workflow/WorkflowRunStep.h"
#include "workflow/WorkflowRunStepResult.h"

namespace jetstream{
namespace workflow{

    class WorkflowRunStepCommand {

        public:
            WorkflowRunStepCommand( shared_ptr<WorkflowRunStep> workflow_run_step );
            WorkflowRunStepResult run();

        protected:
            string getFullCommand() const;

            shared_ptr<WorkflowRunStep> workflow_run_step;

    };


}
}