#include "server/event/EventRouter.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <mutex>

#include "server/model/Gizmo.h"
using ::jetstream::server::model::Gizmo;

#include "server/collection/GizmoSet.h"
using ::jetstream::server::collection::GizmoSet;

#include "server/ApiServer.h"

#include "Common.h"

#include <thread>
#include <chrono>


namespace jetstream {
namespace server {
namespace event {

    void EventRouter::route( const json& event, ::jetstream::server::ApiServer* api_server_ptr, cppkafka::Producer& events_kafka_producer, cppkafka::MessageBuilder& events_message_builder ){

        cout << "Routing Event: " << event.dump() << endl;

        auto event_route_1 = [&]( shared_ptr<GizmoSet> gizmo_set ){

            if( !gizmo_set ){
                cerr << "send_needs_inference_event: gizmo_set is null" << endl;
            }

            json needs_inference_event = json::object();
            needs_inference_event["event_type"] = "company-api.needs-inference.event";
            needs_inference_event["model_type"] = "gizmos.models.BasicGizmo";
            needs_inference_event["models"] = json(*gizmo_set);
            string event_string = needs_inference_event.dump();

            events_message_builder.payload( event_string );
            events_kafka_producer.produce( events_message_builder );

        };




        if( !event.is_object() ){
            throw std::runtime_error("Event must be an object.");
        }

        // {"event_data":{"created":false,"id":304705,"model":"gizmos.models.BasicGizmo"},"event_type":"company-api.needs-inference.event"}

        string event_type;

        if( event.count("event_type") ){
            if( event.at("event_type").is_string() ){
                event_type = event.at("event_type").get<string>();
            }else{
                throw std::runtime_error("event_type is not a string");
            }
        }else{
            throw std::runtime_error("event_type is required");
        }

        if( event_type.empty() ){
            throw std::runtime_error("Unknown event type.");
        }




        string model_type;

        if( event.count("model_type") ){
            if( event.at("model_type").is_string() ){
                model_type = event.at("model_type").get<string>();
            }else{
                throw std::runtime_error("model_type is not a string");
            }
        }else{
            throw std::runtime_error("model_type is required");
        }

        if( model_type.empty() ){
            throw std::runtime_error("Unknown model type.");
        }

        //return;





        if( event_type == "company-api.needs-inference.event" ){

            if( model_type == "gizmos.models.BasicGizmo" ){

                try{
                    std::scoped_lock<std::mutex> lock{ api_server_ptr->global_mutex };
                    auto gizmo_set = std::make_shared<GizmoSet>( event.at("models") );

                }catch( std::exception& e ){

                    cerr << "event handler for company-api.needs-inference.event failed: " << e.what() << endl;

                }

                return;

            }

            cout << "Discarded company-api.needs-inference.event event type. Model type: " << model_type << endl;
            return;

        }




        cout << "Discarded unknown event type: " << event_type << endl;


    }



}
}
}