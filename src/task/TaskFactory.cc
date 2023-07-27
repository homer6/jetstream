#include "task/TaskFactory.h"

#include "config/JetStreamConfig.h"

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <memory>
using std::unique_ptr;

#include "task/RekeyTask.h"


namespace jetstream {
namespace task {


    TaskFactory::TaskFactory( const ::jetstream::config::JetStreamConfig& config, cppkafka::Producer& kafka_producer, cppkafka::MessageBuilder& message_builder )
        :config(config), kafka_producer(kafka_producer), message_builder(message_builder)
    {


    }

    Task& TaskFactory::create(){

        const string task = this->config.getConfigSetting("task");
        if( !task.size() || task == "no_task" ){
            throw std::runtime_error( "Invalid default task." );
        }

        /*
        if( task == "rekey_uid" ){
            unique_ptr<Task> task( new RekeyTask(this->kafka_producer, this->message_builder), { "/"} );
            this->tasks.push_back( std::move(task) );
            return *this->tasks.back();
        }
         */

        throw std::runtime_error( "Unrecognized task." );

    }

}
}


