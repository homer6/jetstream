#pragma once

#include <memory>
using std::shared_ptr;

#include <string>
using std::string;

#include <map>
using std::map;

#include "workflow/WorkflowRunStep.h"
#include "workflow/WorkflowRunStepResult.h"

#include <functional>

namespace jetstream{
namespace workflow{

    class WorkflowRunStepCommand {

        public:
            WorkflowRunStepCommand( shared_ptr<WorkflowRunStep> workflow_run_step );
            WorkflowRunStepResult run( const std::function<void()>& poll_service_callback = nullptr );

            map<string,string> getEnvironmentVariables() const;

        protected:
            string getFullCommand() const;

            shared_ptr<WorkflowRunStep> workflow_run_step;

    };


}
}