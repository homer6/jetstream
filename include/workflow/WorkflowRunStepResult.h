#pragma once

#include <string>
using std::string;

namespace jetstream{
namespace workflow{

    class WorkflowRunStepResult{
            
        public:
            WorkflowRunStepResult(){}

            WorkflowRunStepResult( int exit_code, const string& output )
                :exit_code(exit_code), output(output)
            {
            }
            
            int exit_code = 0;
            string output;

    };


}
}