#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include <string>
using std::string;

#include <memory>
using std::shared_ptr;

#include "WorkflowRunStep.h"
#include "WorkflowRunStepResult.h"

#include <functional>


namespace jetstream{
namespace workflow{

    class WorkflowRun {

        public:
            WorkflowRun();
            WorkflowRun( json workflow_run_json, const string handler_name = "" );

            virtual void run( const bool& keep_running, const std::function<void()>& poll_service_callback = nullptr );
            //virtual void addWorkflowRunStep( const WorkflowRunStep& workflow_run_step, int index = -1 );

            json getWorkflowRunJson() const;
            WorkflowRunStepResult getResult() const;
            shared_ptr<WorkflowRunStep> getExecutedStep() const;

            bool stepWasExecuted() const;
            
        protected:
            json workflow_run_json;
            string handler_name;
            shared_ptr<WorkflowRunStep> executed_step = nullptr;
            WorkflowRunStepResult result;

    };

}
}