#pragma once


#include "json.hpp"
using json = nlohmann::json;

#include <string>
using std::string;

#include <memory>
using std::shared_ptr;


namespace jetstream{
namespace workflow{

    class WorkflowRunStepCommand;

    class WorkflowRunStep {

        public:
            WorkflowRunStep();
            WorkflowRunStep( json workflow_run_step_json );
            virtual void run( const bool& keep_running );

        protected:
            json workflow_run_step_json;

        friend class WorkflowRun;
        friend class WorkflowRunStepCommand;

    };

}
}