#pragma once

#include <memory>
using std::shared_ptr;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "json.hpp"
using json = nlohmann::json;

#include "workflow/WorkflowRunStepResult.h"

#include <functional>


namespace jetstream{
namespace workflow{

    class WorkflowRunStep;

    class WorkflowRunStepCommand {

        public:
            WorkflowRunStepCommand( const WorkflowRunStep& workflow_run_step, const json& workflow_run_step_command_json );
            WorkflowRunStepResult run( const std::function<void()>& poll_service_callback = nullptr );

            map<string,string> getEnvironmentVariables() const;

            string getFullCommand() const;

        protected:          
            const WorkflowRunStep& workflow_run_step;
            const json workflow_run_step_command_json;

    };


    typedef shared_ptr<WorkflowRunStepCommand> WorkflowRunStepCommandPtr;
    typedef vector<WorkflowRunStepCommandPtr> WorkflowRunStepCommandList;
    typedef shared_ptr<WorkflowRunStepCommandList> WorkflowRunStepCommandListPtr;


}
}