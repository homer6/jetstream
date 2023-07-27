#include "data/GizmoDataAugmenter.h"


#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <filesystem>
namespace fs = std::filesystem;

#include <sstream>
#include <string>
#include <fstream>
using std::ofstream;
using std::ios;

#include "Common.h"

#include <algorithm>

#include <vector>
using std::vector;

#include "rapidcsv.h"

#include <random>


namespace jetstream{
namespace data{

    void GizmoDataAugmenter::augment(){

        //ignore empty cells: https://github.com/d99kris/rapidcsv#reading-a-file-with-invalid-numbers-eg-empty-cells-as-numeric-data
        auto separator_param = rapidcsv::SeparatorParams();
        separator_param.mQuotedLinebreaks = true;
        rapidcsv::Document csv_doc( this->input_file, rapidcsv::LabelParams(), separator_param, rapidcsv::ConverterParams(true) );

        this->openOutfile();


        vector<string> id_column = csv_doc.GetColumn<string>("id");
        vector<string> name_column = csv_doc.GetColumn<string>("name");
        vector<string> description_column = csv_doc.GetColumn<string>("description");
        vector<string> creation_date_column = csv_doc.GetColumn<string>("creation_date");
        vector<string> last_modified_column = csv_doc.GetColumn<string>("last_modified");
        vector<string> color_column = csv_doc.GetColumn<string>("color");
        vector<string> size_column = csv_doc.GetColumn<string>("size");
        vector<string> status_column = csv_doc.GetColumn<string>("status");
        vector<string> location_column = csv_doc.GetColumn<string>("location");
        vector<string> owner_id_column = csv_doc.GetColumn<string>("owner_id");

        ofstream& this_outfile = this->outfile;
        auto print_line = [&this_outfile](const string& , const string& value, const string& id ){
            this_outfile << "gizmo-" << id << "," << value << endl;
        };

        int x = -1;
        for(const auto& id : id_column){

            x++;

            try{

                const string name = to_lower_string(name_column[x]);
                const string description = to_lower_string(description_column[x]);
                const string creation_date = to_lower_string(creation_date_column[x]);
                const string last_modified = to_lower_string(last_modified_column[x]);
                const string color = to_lower_string(color_column[x]);
                const string size = to_lower_string(size_column[x]);
                const string status = to_lower_string(status_column[x]);
                const string location = to_lower_string(location_column[x]);
                const string owner_id = to_lower_string(owner_id_column[x]);

                string whole_string =
                    name + " " +
                    description + " " +
                    creation_date + " " +
                    last_modified + " " +
                    color + " " +
                    size + " " +
                    status + " " +
                    location + " " +
                    owner_id;

                if(id.empty()) continue;

                whole_string = this->clean_values(whole_string);
                if(whole_string.size()){
                    print_line(id, whole_string, id);
                }else{
                    continue;
                }

            }catch(std::exception& e){
                cerr << "Exception on row " << x << endl;
                cerr << e.what() << endl;
            }

        }

    }


}
}


