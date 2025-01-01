#pragma once


#include "json.hpp"
using json = nlohmann::json;

#include <string>
using std::string;

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;

#include "workflow/WorkflowRunStepCommand.h"


namespace jetstream{
namespace workflow{

    class WorkflowRunStepCommand;

    class WorkflowRunStep {

        public:
            WorkflowRunStep();
            WorkflowRunStep( json workflow_run_step_json );
            virtual void run( bool keep_running = true );
            vector<string> getOutputTopics() const;

            WorkflowRunStepCommandListPtr getCommands() const;

        protected:
            json workflow_run_step_json;

        friend class WorkflowRun;
        friend class WorkflowRunStepCommand;

    };

}
}