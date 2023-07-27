#include "task/RekeyTask.h"

#include "json.hpp"
using json = nlohmann::json;

namespace jetstream {
namespace task {

    void RekeyTask::run( const string&, const string& payload ){

        json json_object = json::parse( payload );

        json uid_string_json = json_object.at( json::json_pointer("/log_obj/uid") );
        if( !uid_string_json ){
            throw std::runtime_error("/log_obj/uid does not exist.");
        }
        if( !uid_string_json || !uid_string_json.is_string() ){
            throw std::runtime_error("/log_obj/uid is not a string.");
        }

        string uid_string = uid_string_json.get<string>();
        if( uid_string.size() == 0 ){
            //drop this message if it doesn't have a uid to key on
            throw std::runtime_error("/log_obj/uid cannot be empty.");
        }

        /*
        string uid_string;
        if( json_object.is_object() ){
            if( json_object.count("log_obj") ){
                json log_obj = json_object.at("log_obj");
                if( log_obj.is_object() ){
                    if( log_obj.count("uid") ){
                        json uid = log_obj.at("uid");
                        if( uid.is_string() ){
                            uid_string = uid.get<string>();
                        }
                    }
                }
            }
        }*/




        const string message_payload = json_object.dump();
        this->message_builder.key( uid_string );
        this->message_builder.payload( message_payload );
        this->kafka_producer.produce( this->message_builder );

    };

}
}







