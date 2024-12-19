#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include <vector>
using std::vector;

#include <string>
using std::string;


namespace jetstream{
namespace workflow{

    class WorkflowRunContext {

        public:
            WorkflowRunContext();
            WorkflowRunContext( json workflow_run_json, const string handler_name = "" );

            virtual void runSetup();
            virtual void runTeardown();

            virtual ~WorkflowRunContext();
            
        protected:
            json workflow_run_json;
            string handler_name;

            vector<string> global_input_files;
            vector<string> input_files;
            vector<string> output_files;

    };

}
}