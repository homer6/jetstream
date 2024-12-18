#include "workflow/WorkflowRunStep.h"


namespace jetstream{
namespace workflow{


    WorkflowRunStep::WorkflowRunStep(){
    }

    WorkflowRunStep::WorkflowRunStep( json workflow_run_step_json )
        :workflow_run_step_json( workflow_run_step_json )
    {

    }

    void WorkflowRunStep::run( bool keep_running ){

        throw std::runtime_error("WorkflowRunStep::run() not implemented");

    }

    vector<string> WorkflowRunStep::getOutputTopics() const{

        vector<string> output_topics;

        if( this->workflow_run_step_json.contains("output_topics") ){

            for( auto &output_topic : this->workflow_run_step_json["output_topics"] ){
                if( output_topic.is_string() ){
                    output_topics.push_back( output_topic.get<string>() );
                }else{
                    throw std::runtime_error("WorkflowRunStep output_topics must be an array of strings");
                }
                
            }

        }

        return output_topics;

    }


}
}