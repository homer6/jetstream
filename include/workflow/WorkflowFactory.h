#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include <string>
using std::string;

#include "workflow/WorkflowRun.h"

namespace jetstream{
namespace workflow{

    class WorkflowFactory {

        public:
            WorkflowFactory();
            virtual WorkflowRun createWorkflowRun( json workflow_run_json );


    };


}
}