#include "server/collection/GizmoSet.h"

#include "pugixml.hpp"


namespace jetstream {
namespace server {
namespace collection {

    void GizmoSet::loadFromXml( const string& xml_content ){

        {
            pugi::xml_document doc;

            //immutable buffer must outlive doc
            pugi::xml_parse_result result = doc.load_buffer_inplace( (void*)xml_content.c_str(), xml_content.size() );
            //pugi::xml_parse_result result = doc.load_string(xml_content);

            if( result ){
                //std::cout << "XML parsed without errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n\n";
            }else{
                std::cout << "XML parsed with errors\n";
                std::cout << "Error description: " << result.description() << "\n";
                std::cout << "Error offset: " << result.offset << "\n\n";
                throw std::runtime_error("Invalid XML.");
            }

            cout << result.description() << endl;

            auto root_node = doc.root().first_child();

            for( pugi::xml_node gizmo : root_node.children() ){
                if( gizmo.child_value("gizmos") ){
                    std::cout << "gizmos: " << gizmo.child_value("gizmos") << "\n";
                }
            }

        }

    }


}
}
}
