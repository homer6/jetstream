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


    WorkflowRunStepCommandListPtr WorkflowRunStep::getCommands() const{

        WorkflowRunStepCommandListPtr commands = std::make_shared<WorkflowRunStepCommandList>();

        if( this->workflow_run_step_json.contains("commands") ){

            if( !this->workflow_run_step_json["commands"].is_array() ){
                throw std::runtime_error("WorkflowRunStep commands must be an array");
            }

            const json& commands_json = this->workflow_run_step_json["commands"];

            for( const json& command : commands_json ){
                if( command.is_object() ){
                    commands->push_back( std::make_shared<WorkflowRunStepCommand>( *this, command ) );
                }else{
                    throw std::runtime_error("WorkflowRunStep commands must be an array of objects");
                }                
            }

        }

        return commands;

    }

}
}