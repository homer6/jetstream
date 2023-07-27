#include "parser/GizmoParser.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "stopwatch.h"

#include <filesystem>
namespace fs = std::filesystem;

#include <sstream>
#include <string>
#include <fstream>
using std::ofstream;
using std::ios;

#include "Common.h"

#include <vector>
using std::vector;

#include "data/GizmoDataAugmenter.h"

#include "rapidcsv.h"
#include <map>
#include <utility>

namespace jetstream {
namespace parser {


    GizmoParser::GizmoParser( const ::jetstream::config::IngestWriterConfig &config, const vector<string>& command_line_arguments )
        :config(config), command_line_arguments(command_line_arguments)
    {


    }

//  make -j8 && time ./build/jetstream data-job-1 /home/user/dev/data

    void GizmoParser::parse(){


        const string model_output_file_preprocessed = "analysis/model_1_preprocessed.csv";
        const string model_output_file_indexed = "analysis/model_1_indexed.csv";
        const string model_output_file_indexed_classes = "analysis/model_1_indexed_classes.csv";

        {
            data::GizmoDataAugmenter gudid_augmenter;
            gudid_augmenter.setInputFile( "/home/user/dev/data/gizmos.csv" );
            gudid_augmenter.setOutputFile( model_output_file_preprocessed );
            gudid_augmenter.augment();

        } //ensure these files are closed before proceeding


        std::multimap<string,string> input_index;
        std::map<string,int> label_index;  //unique label as key; int as value

        //sort and index; drop NAs
        {

            //load the data from the input file; using maps and multimaps automatically sort
            auto separator_param = rapidcsv::SeparatorParams();
            separator_param.mQuotedLinebreaks = true;
            rapidcsv::Document csv_doc( model_output_file_preprocessed, rapidcsv::LabelParams(), separator_param, rapidcsv::ConverterParams(true) );

            vector<string> labels_column = csv_doc.GetColumn<string>("labels");
            vector<string> input_column = csv_doc.GetColumn<string>("input");

            int x = -1;
            for( const auto& label : labels_column ){
                x++;

                const string input = input_column[x];
                if( input.empty() || label.empty() ){
                    //drop NA rows
                    continue;
                }
                input_index.insert( std::pair<string,string>( label, input ) );
                label_index.insert( std::pair<string,int>( label, 0 ) ); //set the index value to zero for now; will update this once the map is complete

            }

        }

        {
            int x = -1;
            for( auto& [label, label_offset] : label_index ) {
                x++;
                label_offset = x;
            }
        }


        {
            ofstream outfile( model_output_file_indexed, ios::out | ios::trunc );
            outfile << "label_index,labels,input" << endl;

            for( const auto& [label, input] : input_index ){
                outfile << label_index.at(label) << "," << label << "," << input << "\n";
            }
        }

        {
            ofstream outfile( model_output_file_indexed_classes, ios::out | ios::trunc );
            outfile << "label_index,class_label" << endl;

            for( const auto& [label, label_offset] : label_index ){
                outfile << label_offset << "," << label << "\n";
            }
        }





        return;

        if( this->command_line_arguments.size() < 3 ){
            throw std::runtime_error("Too few arguments.");
        }

        this->source_filepath = this->command_line_arguments[2];

        cout << "Reading source gizmo files from path: " << this->source_filepath << endl;


        this->walkStage1();
        //wait for all tasks to complete
        this->pool.wait_for_tasks();

        this->walkAll(100);
        //this->walkAll();
        //wait for all tasks to complete
        this->pool.wait_for_tasks();

        this->printAll();


    }



    void GizmoParser::walkStage1( const int number_of_rows ){

        int x = 0;
        GizmoParser* parser = this;

        for( const auto& entry : fs::directory_iterator(this->source_filepath) ){

            x++;

            this->pool.push_task([parser, entry, x, number_of_rows](){

                if( entry.path().extension() != ".log" ) return;

                precise_stopwatch stopwatch;

                std::ifstream source_file( entry.path() );

                // get log_type
                    string stem = entry.path().stem();
                    vector<string> stem_parts = split_string(stem, '.');
                    string log_type = "unknown";
                    if( !stem_parts.empty() ){
                        log_type = stem_parts.front();
                    }


                //if( log_type != "dhcp" && log_type != "radius" ) return;
                if( log_type != "x509" ) return;

                string line;
                int y = 0;
                while( std::getline(source_file, line) ){

                    y++;

                    try{
                        json object = json::parse( line );

                        const string fingerprint = object.at("fingerprint").get<string>();
                        if( fingerprint.size() ){
                            std::scoped_lock lock( parser->shared_mutex );
                            parser->x509_certs[fingerprint] = object;
                        }

                        continue;


                        if( y <= 10 ){
                            string output_dump = object.dump(4);
                            {
                                std::scoped_lock lock( parser->shared_mutex );
                                cout << "\n\nlog_type: " << log_type << "\n";
                                cout << output_dump << endl;
                            }
                        }

                        for( auto& [key, value] : object.items() ){

                            /*
                            if( key == "id.orig_h" || key == "id.resp_h" || key == "client_addr" || key == "client_addr" || key == "requested_addr" ){
                                std::scoped_lock lock( parser->shared_mutex );
                                parser->ips.insert(value);
                                ips_by_log_type_set->insert(value);
                            }


                            if( key == "mac" ){
                                std::scoped_lock lock( parser->shared_mutex );
                                parser->mac_addresses.insert(value);
                                mac_addresses_by_log_type_set->insert(value);
                            }*/

                        }

                    }catch( std::exception& e ){
                        //cout << "skipped" << endl;
                    }

                    if( number_of_rows != -1 ){
                        if( y >= number_of_rows ) break;
                    }

                }

            });

        }


    }



    void GizmoParser::walkAll( const int number_of_rows ){

        int x = 0;
        GizmoParser* parser = this;

        for( const auto& entry : fs::directory_iterator(this->source_filepath) ){

            x++;

            this->pool.push_task([parser, entry, x, number_of_rows](){

                precise_stopwatch stopwatch;

                //{
                //    std::scoped_lock lock( parser->shared_mutex );
                    //cout << std::endl << std::endl << "Reading file (" << x << "): " << entry.path() << std::endl;
                //}

                std::ifstream source_file( entry.path() );

                // get log_type
                    string stem = entry.path().stem();
                    vector<string> stem_parts = split_string(stem, '.');
                    string log_type = "unknown";
                    if( !stem_parts.empty() ){
                        log_type = stem_parts.front();
                    }
                    //cout << "    Log Type: " << log_type << std::endl;


                //if( log_type != "ssl" ) continue;


                set<string>* ips_by_log_type_set = nullptr;
                {
                    std::scoped_lock lock( parser->shared_mutex );
                    if( parser->ips_by_log_type.count(log_type) == 0 ){
                        parser->ips_by_log_type[log_type] = set<string>();
                    }
                    ips_by_log_type_set = &parser->ips_by_log_type.at(log_type);
                }


                set<string>* hostnames_by_log_type_set = nullptr;
                {
                    std::scoped_lock lock( parser->shared_mutex );
                    if( parser->hostnames_by_log_type.count(log_type) == 0 ){
                        parser->hostnames_by_log_type[log_type] = set<string>();
                    }
                    hostnames_by_log_type_set = &parser->hostnames_by_log_type.at(log_type);
                }


                set<string>* user_agents_by_log_type_set = nullptr;
                {
                    std::scoped_lock lock( parser->shared_mutex );
                    if( parser->user_agents_by_log_type.count(log_type) == 0 ){
                        parser->user_agents_by_log_type[log_type] = set<string>();
                    }
                    user_agents_by_log_type_set = &parser->user_agents_by_log_type.at(log_type);
                }


                set<string>* subjects_by_log_type_set = nullptr;
                {
                    std::scoped_lock lock( parser->shared_mutex );
                    if( parser->subjects_by_log_type.count(log_type) == 0 ){
                        parser->subjects_by_log_type[log_type] = set<string>();
                    }
                    subjects_by_log_type_set = &parser->subjects_by_log_type.at(log_type);
                }


                set<string>* issuer_subjects_by_log_type_set = nullptr;
                {
                    std::scoped_lock lock( parser->shared_mutex );
                    if( parser->issuer_subjects_by_log_type.count(log_type) == 0 ){
                        parser->issuer_subjects_by_log_type[log_type] = set<string>();
                    }
                    issuer_subjects_by_log_type_set = &parser->issuer_subjects_by_log_type.at(log_type);
                }


                set<string>* mac_addresses_by_log_type_set = nullptr;
                {
                    std::scoped_lock lock( parser->shared_mutex );
                    if( parser->mac_addresses_by_log_type.count(log_type) == 0 ){
                        parser->mac_addresses_by_log_type[log_type] = set<string>();
                    }
                    mac_addresses_by_log_type_set = &parser->mac_addresses_by_log_type.at(log_type);
                }


                string line;
                int y = 0;
                while( std::getline(source_file, line) ){

                    y++;

                    try{
                        json object = json::parse( line );

                        if( y <= 10 ){
                            string output_dump = object.dump(4);
                            {
                                std::scoped_lock lock( parser->shared_mutex );
                                cout << "\n\nlog_type: " << log_type << "\n";
                                cout << output_dump << endl;
                            }
                        }

                        //cout << log_type << ": " << x << endl;
                        if( log_type == "conn" ){
                            std::scoped_lock lock( parser->summaries_mutex, parser->host_mutex );
                            parser->addConnection(object);
                        }

                        string current_orig_ip_address;
                        string current_resp_ip_address;
                        string host_ip_address;

                        for( auto& [key, value] : object.items() ) {

                            if( key == "id.orig_h" || key == "id.resp_h" || key == "client_addr" || key == "requested_addr" || key == "assigned_addr" || key == "server_addr" ){
                                std::scoped_lock lock( parser->shared_mutex, parser->host_mutex );
                                parser->ips.insert(value);
                                ips_by_log_type_set->insert(value);

                                parser->addHostLogType(value, log_type);

                                /*
                                if( value.get<string>() == "10.0.0.1" ){
                                    string output_dump = object.dump(4);
                                    cout << "\n\nlog_type: " << log_type << "\n";
                                    cout << output_dump << endl;
                                }
                                */

                            }

                            if( key == "id.orig_h" /*|| key == "client_addr" || key == "assigned_addr"*/ ){
                                current_orig_ip_address = value;
                            }

                            if( key == "id.resp_h" /*|| key == "server_addr"*/ ){
                                current_resp_ip_address = value;
                            }

                            if( key == "host" ){
                                host_ip_address = value;
                            }


                        }

                        //attach the subject and subject_issuer from the x509 cert
                        if( log_type == "ssl" ){

                            if( object.count("cert_chain_fps") ){

                                try{

                                    const json& cert_chain_fps = object.at("cert_chain_fps");
                                    if( !cert_chain_fps.is_array() ){
                                        throw std::runtime_error("cert_chain_fps not an array");
                                    }

                                    for( const json& value : cert_chain_fps ){

                                        if( !value.is_string() ){
                                            throw std::runtime_error("cert_chain_fp is not a string");
                                        }
                                        const string fingerprint = value.get<string>();

                                        try{
                                            const json& certificate = parser->getCertificate( fingerprint );

                                            try{
                                                const string cert_issuer = certificate.at("certificate.issuer").get<string>();
                                                std::scoped_lock lock( parser->host_mutex );
                                                parser->addHostIssuerSubjectTalkTo( current_orig_ip_address, cert_issuer );
                                            }catch( std::exception& e ){
                                                //cerr << "Exception certificate.issuer: " << e.what() << endl;
                                                //cerr << certificate.dump(4) << endl;
                                            }

                                            try{
                                                const string cert_subject = certificate.at("certificate.subject").get<string>();
                                                std::scoped_lock lock( parser->host_mutex );
                                                parser->addHostSubjectTalkTo( current_orig_ip_address, cert_subject );
                                            }catch( std::exception& e ){
                                                //cerr << "Exception certificate.subject: " << e.what() << endl;
                                                //cerr << certificate.dump(4) << endl;
                                            }

                                        }catch( std::exception& e ){
                                            //throw std::runtime_error( "Failed to find x509 certificate from provided fingerprint.");
                                        }

                                    }

                                    const string next_protocol = object.at("next_protocol").get<string>();
                                    if( next_protocol.size() ){
                                        std::scoped_lock lock( parser->host_mutex );
                                        parser->addHostProtocol( current_orig_ip_address, "ssl_client_" + next_protocol );
                                    }


                                }catch( std::exception& e ){
                                    //cerr << "Exception: " << e.what() << endl;
                                    //cerr << object.dump(4) << endl;
                                }


                            }


                        }


                        for( auto& [key, value] : object.items() ) {

                            if( key == "host" || key == "query" || key == "host_name" || key == "domain" || key == "client_fqdn" || key == "server_name" ){

                                if( key == "server_name" || key == "query" || key == "host" ){
                                    std::scoped_lock lock( parser->shared_mutex, parser->host_mutex );
                                    parser->hostnames.insert(value);
                                    hostnames_by_log_type_set->insert(value);

                                    parser->addHostHostnameTalkTo( current_orig_ip_address, value );

                                }else{
                                    std::scoped_lock lock( parser->shared_mutex, parser->host_mutex );
                                    parser->hostnames.insert(value);
                                    hostnames_by_log_type_set->insert(value);

                                    parser->addHostHostname( current_orig_ip_address, value );
                                }

                            }

                            //dns
                            if( key == "answers" ){
                                if( value.is_array() ){
                                    std::scoped_lock lock( parser->shared_mutex);
                                    for( auto& item : value ){
                                        if( item.is_string() ){
                                            parser->hostnames.insert(item);
                                            hostnames_by_log_type_set->insert(item);
                                        }
                                    }
                                }
                            }

                            //files
                            if( key == "rx_hosts" || key == "tx_hosts" ){
                                if( value.is_array() ){
                                    std::scoped_lock lock( parser->shared_mutex );
                                    for( auto& item : value ){
                                        if( item.is_string() ){
                                            parser->hostnames.insert(item);
                                            hostnames_by_log_type_set->insert(item);
                                        }
                                    }
                                }
                            }


                            if( key == "user_agent" || key == "unparsed_version" ){
                                std::scoped_lock lock( parser->shared_mutex, parser->host_mutex );
                                parser->user_agents.insert(value);
                                user_agents_by_log_type_set->insert(value);

                                parser->addHostUserAgent( current_orig_ip_address, value );
                            }

                            if( key == "subject" || key == "sub" || key == "certificate.subject" ){

                                if( log_type != "smtp" ){
                                    std::scoped_lock lock( parser->shared_mutex, parser->host_mutex );
                                    parser->subjects.insert(value);
                                    subjects_by_log_type_set->insert(value);

                                    parser->addHostSubject( current_resp_ip_address, value );
                                }

                                if( log_type == "known_certs" ){
                                    std::scoped_lock lock( parser->host_mutex );
                                    parser->addHostSubject( host_ip_address, value );
                                    parser->addHostIssuerSubject( host_ip_address, value );
                                }

                            }


                            if( key == "issuer_subject" || key == "certificate.issuer" ){
                                std::scoped_lock lock( parser->shared_mutex, parser->host_mutex );
                                parser->issuer_subjects.insert(value);
                                issuer_subjects_by_log_type_set->insert(value);

                                parser->addHostIssuerSubject( current_resp_ip_address, value );
                            }


                            if( key == "mac" ){
                                std::scoped_lock lock( parser->shared_mutex, parser->host_mutex );
                                parser->mac_addresses.insert(value);
                                mac_addresses_by_log_type_set->insert(value);

                                parser->addHostMac( current_orig_ip_address, value );
                            }


                            if( key == "id.resp_p" ){
                                std::scoped_lock lock( parser->host_mutex );
                                parser->addHostPort( current_resp_ip_address, value.get<int>() );
                            }




                            if( key == "id.resp_p" ){
                                if( parser->known_services.count(log_type) ){
                                    std::scoped_lock lock( parser->host_mutex );
                                    parser->addHostService( current_resp_ip_address, log_type + "_server" );
                                }
                            }

                            if( key == "id.orig_h" ){
                                if( parser->known_services.count(log_type) ){
                                    std::scoped_lock lock( parser->host_mutex );
                                    parser->addHostService( current_orig_ip_address, log_type + "_server" );
                                }
                            }


                        }

                    }catch( std::exception& e ){
                        //cout << "skipped" << endl;
                    }

                    if( number_of_rows != -1 ){
                        if( y >= number_of_rows ) break;
                    }

                }

                //{
                //    std::scoped_lock lock( parser->shared_mutex );
                    //cout << "Finished " << entry.path() << " in " << stopwatch.elapsed_time_ms() << "ms" << endl;
               //}

            });

        }


    }






    void GizmoParser::printAll(){


        std::scoped_lock lock( this->shared_mutex, this->summaries_mutex, this->host_mutex );

        fs::create_directory( "analysis" );

        {
            ofstream outfile;
            outfile.open("analysis/summary.txt", ios::out | ios::trunc);


            outfile << "\n\n\nUnique IP Count: " << this->ips.size() << endl;

            for( auto& [log_type, ips_by_log_type_set] : this->ips_by_log_type ){
                outfile << "Unique IP Count (by log_type) " << log_type << ": " << ips_by_log_type_set.size() << endl;
            }


            outfile << "\n\n\nUnique MAC Address Count: " << this->mac_addresses.size() << endl;

            for( auto& [log_type, mac_addresses_by_log_type_set] : this->mac_addresses_by_log_type ){
                outfile << "Unique MAC Address Count (by log_type) " << log_type << ": " << mac_addresses_by_log_type_set.size() << endl;
            }



            outfile << "\n\n\nUnique User Agent Count: " << this->user_agents.size() << endl;

            for( auto& [log_type, user_agents_by_log_type_set] : this->user_agents_by_log_type ){
                outfile << "Unique User Agent Count (by log_type) " << log_type << ": " << user_agents_by_log_type_set.size() << endl;
            }


            outfile << "\n\n\nUnique Hostname Count: " << this->hostnames.size() << endl;

            for( auto& [log_type, hostnames_by_log_type_set] : this->hostnames_by_log_type ){
                outfile << "Unique Hostname Count (by log_type) " << log_type << ": " << hostnames_by_log_type_set.size() << endl;
            }


            outfile << "\n\n\nUnique Subjects Count: " << this->subjects.size() << endl;

            for( auto& [log_type, subjects_by_log_type_set] : this->subjects_by_log_type ){
                outfile << "Unique Subjects Count (by log_type) " << log_type << ": " << subjects_by_log_type_set.size() << endl;
            }


            outfile << "\n\n\nUnique TCP Conversations Count: " << this->tcp_connection_summaries.size() << endl;
            outfile << "\n\n\nUnique UDP Conversations Count: " << this->udp_connection_summaries.size() << endl;
            outfile << "\n\n\nUnique Hosts Count: " << this->hosts.size() << endl;

        }



        {
            ofstream outfile;
            outfile.open("analysis/ips.txt", ios::out | ios::trunc);

            outfile << "\n\n\nUnique IP Count: " << this->ips.size() << endl;

            for( auto& [log_type, ips_by_log_type_set] : this->ips_by_log_type ){
                outfile << "Unique IP Count (by log_type) " << log_type << ": " << ips_by_log_type_set.size() << endl;
            }

            outfile << "\n\n\nIP Addresses (" << this->ips.size() << "):\n" << endl;
            for (const auto &ip: this->ips) {
                outfile << ip << endl;
            }
        }

        {
            ofstream outfile;
            outfile.open("analysis/macs.txt", ios::out | ios::trunc);

            outfile << "\n\n\nUnique MAC Address Count: " << this->mac_addresses.size() << endl;

            for( auto& [log_type, mac_addresses_by_log_type_set] : this->mac_addresses_by_log_type ){
                outfile << "Unique MAC Address Count (by log_type) " << log_type << ": " << mac_addresses_by_log_type_set.size() << endl;
            }

            outfile << "\n\n\nMAC Addresses (" << this->mac_addresses.size() << "):\n" << endl;
            for( const auto& mac_address : this->mac_addresses ){
                outfile << mac_address << endl;
            }
        }


        {
            ofstream outfile;
            outfile.open("analysis/user_agents.txt", ios::out | ios::trunc);

            outfile << "\n\n\nUnique User Agent Count: " << this->user_agents.size() << endl;

            for( auto& [log_type, user_agents_by_log_type_set] : this->user_agents_by_log_type ){
                outfile << "Unique User Agent Count (by log_type) " << log_type << ": " << user_agents_by_log_type_set.size() << endl;
            }

            outfile << "\n\n\nUser Agents (" << this->user_agents.size() << "):\n" << endl;
            for( const auto& user_agent : this->user_agents ){
                outfile << user_agent << endl;
            }
        }


        {
            ofstream outfile;
            outfile.open("analysis/hostnames.txt", ios::out | ios::trunc);

            outfile << "\n\n\nUnique Hostname Count: " << this->hostnames.size() << endl;

            for( auto& [log_type, hostnames_by_log_type_set] : this->hostnames_by_log_type ){
                outfile << "Unique Hostname Count (by log_type) " << log_type << ": " << hostnames_by_log_type_set.size() << endl;
            }

            outfile << "\n\n\nHostnames (" << this->hostnames.size() << "):\n" << endl;
            for( const auto& hostname : this->hostnames ){
                outfile << hostname << endl;
            }
        }


        {
            ofstream outfile;
            outfile.open("analysis/subjects.txt", ios::out | ios::trunc);

            outfile << "\n\n\nUnique Subjects Count: " << this->subjects.size() << endl;

            for( auto& [log_type, subjects_by_log_type_set] : this->subjects_by_log_type ){
                outfile << "Unique Subjects Count (by log_type) " << log_type << ": " << subjects_by_log_type_set.size() << endl;
            }

            outfile << "\n\n\nSubjects (" << this->subjects.size() << "):\n" << endl;
            for( const auto& subject : this->subjects ){
                outfile << subject << endl;
            }

        }

        {
            ofstream outfile;
            outfile.open("analysis/issuer_subjects.txt", ios::out | ios::trunc);

            outfile << "\n\n\nUnique Issuer Subjects Count: " << this->issuer_subjects.size() << endl;

            for( auto& [log_type, issuer_subjects_by_log_type_set] : this->issuer_subjects_by_log_type ){
                outfile << "Unique Issuer Subjects Count (by log_type) " << log_type << ": " << issuer_subjects_by_log_type_set.size() << endl;
            }

            outfile << "\n\n\nIssuer Subjects (" << this->issuer_subjects.size() << "):\n" << endl;
            for( const auto& issuer_subject : this->issuer_subjects ){
                outfile << issuer_subject << endl;
            }

        }


        {
            ofstream outfile;
            outfile.open("analysis/tcp_conversations.csv", ios::out | ios::trunc);

            //outfile << "\n\n\nUnique TCP Conversations Count: " << this->tcp_connection_summaries.size() << endl;

            /*
            for( auto& [log_type, subjects_by_log_type_set] : this->subjects_by_log_type ){
                outfile << "Unique TCP Conversations Count (by log_type) " << log_type << ": " << subjects_by_log_type_set.size() << endl;
            }
            */

            //outfile << "\n\n\nTCP Conversations (" << this->tcp_connection_summaries.size() << "):\n" << endl;
            outfile
                << "destination_ip,"
                << "source_ip,"
                << "service,"
                << "connection_count,"
                << "destination_bytes_count,"
                << "source_bytes_count"
            << endl;

            for( const auto& [key, value] : this->tcp_connection_summaries ){
                outfile
                    << value.destination_ip << ","
                    << value.source_ip << ","
                    << value.service << ","
                    << value.connection_count << ","
                    << value.destination_bytes_count << ","
                    << value.source_bytes_count
                << endl;
            }

        }


        {
            ofstream outfile;
            outfile.open("analysis/udp_conversations.csv", ios::out | ios::trunc);

            //outfile << "\n\n\nUnique UDP Conversations Count: " << this->udp_connection_summaries.size() << endl;

            /*
            for( auto& [log_type, subjects_by_log_type_set] : this->subjects_by_log_type ){
                outfile << "Unique UDP Conversations Count (by log_type) " << log_type << ": " << subjects_by_log_type_set.size() << endl;
            }
            */

            //outfile << "\n\n\nUDP Conversations (" << this->udp_connection_summaries.size() << "):\n" << endl;

            outfile
                << "destination_ip,"
                << "source_ip,"
                << "service,"
                << "connection_count,"
                << "destination_bytes_count,"
                << "source_bytes_count"
            << endl;

            for( const auto& [key, value] : this->udp_connection_summaries ){
                outfile
                    << value.destination_ip << ","
                    << value.source_ip << ","
                    << value.service << ","
                    << value.connection_count << ","
                    << value.destination_bytes_count << ","
                    << value.source_bytes_count
                << endl;
            }

        }



        {
            ofstream outfile;
            outfile.open("analysis/hosts.jsonl", ios::out | ios::trunc);

            for( const auto& [key, value] : this->hosts ){

                json object = json::object();

                object["ip"] = value.ip;

                if( value.hostnames.size() ){
                    object["hostnames"] = value.hostnames;
                }

                if( value.hostnames_talk_to.size() ){
                    object["hostnames_talk_to"] = value.hostnames_talk_to;
                }

                if( value.macs.size() ) {
                    object["macs"] = value.macs;
                }

                if( value.connection_count ){
                    object["connection_count"] = value.connection_count;
                }
                if( value.incoming_bytes_count ) {
                    object["incoming_bytes_count"] = value.incoming_bytes_count;
                }
                if( value.outgoing_bytes_count ){
                    object["outgoing_bytes_count"] = value.outgoing_bytes_count;
                }

                if( value.ports.size() ){
                    object["ports"] = value.ports;
                }
                if( value.device_labels.size() ){
                    object["device_labels"] = value.device_labels;
                }
                if( value.user_agents.size() ){
                    object["user_agents"] = value.user_agents;
                }

                if( value.subjects.size() ){
                    object["subjects"] = value.subjects;
                }
                if( value.issuer_subjects.size() ){
                    object["issuer_subjects"] = value.issuer_subjects;
                }

                if( value.subjects_talk_to.size() ){
                    object["subjects_talk_to"] = value.subjects_talk_to;
                }
                if( value.issuer_subjects_talk_to.size() ){
                    object["issuer_subjects_talk_to"] = value.issuer_subjects_talk_to;
                }

                if( value.log_types.size() ){
                    object["log_types"] = value.log_types;
                }
                if( value.services.size() ){
                    object["services"] = value.services;
                }
                if( value.protocols.size() ){
                    object["protocols"] = value.protocols;
                }

                outfile
                    << object.dump()
                << endl;
            }

        }





    }





    void GizmoParser::addConnection( const json& connection ){

        //host_mutex and summaries_mutex must be secured before calling GizmoParser::addConnection

        ConnectionSummary summary;
        summary.source_ip = connection.at("id.orig_h").get<string>();
        summary.destination_ip = connection.at("id.resp_h").get<string>();
        summary.source_bytes_count = connection.at("orig_ip_bytes").get<long>();
        summary.destination_bytes_count = connection.at("resp_ip_bytes").get<long>();

        this->incrementHostConnectionCount( summary.source_ip );
        this->incrementHostConnectionCount( summary.destination_ip );

        this->incrementHostIncomingBytesCount( summary.source_ip, summary.destination_bytes_count );
        this->incrementHostIncomingBytesCount( summary.destination_ip, summary.source_bytes_count );

        this->incrementHostOutgoingBytesCount( summary.source_ip, summary.source_bytes_count );
        this->incrementHostOutgoingBytesCount( summary.destination_ip, summary.destination_bytes_count );


        string protocol;
        try{
           protocol = connection.at("proto").get<string>();
           this->addHostProtocol( summary.source_ip, protocol );
           this->addHostProtocol( summary.destination_ip, protocol );
        }catch( std::exception& e ){
        }

        string service;
        try{
           service = connection.at("service").get<string>();
        }catch( std::exception& e ){
        }

        //std::replace( service.begin(), service.end(), ',', ';'); // replace all ',' to ';'
        //sort services and replace delimiter with ;
        vector<string> services = split_string( service, ',' );
        set<string> services_set( services.begin(), services.end() );

        for( const string& service_str : services_set ){
           this->addHostService( summary.source_ip, service_str + "_client" );
           this->addHostService( summary.destination_ip, service_str + "_server" );
        }

        //size_t x = 0;
        service = explode_string(services_set, ';');

        summary.service = service;

        string connection_key = summary.source_ip + "|" + summary.destination_ip + "|" + service;


        if( protocol == "udp" ){

            if( this->udp_connection_summaries.count(connection_key) ){
                ConnectionSummary& existing_connection = this->udp_connection_summaries.at(connection_key);
                existing_connection.connection_count++;
                existing_connection.source_bytes_count += summary.source_bytes_count;
                existing_connection.destination_bytes_count += summary.destination_bytes_count;
            }else{
                this->udp_connection_summaries[connection_key] = summary;
            }

        }else{

            if( this->tcp_connection_summaries.count(connection_key) ){
                ConnectionSummary& existing_connection = this->tcp_connection_summaries.at(connection_key);
                existing_connection.connection_count++;
                existing_connection.source_bytes_count += summary.source_bytes_count;
                existing_connection.destination_bytes_count += summary.destination_bytes_count;
            }else{
                this->tcp_connection_summaries[connection_key] = summary;
            }

        }


    }




    Host& GizmoParser::getOrCreateHost( const string& ip_address ){

        if( this->hosts.count(ip_address) == 0 ){
            this->hosts[ip_address] = Host(ip_address);
        }
        return this->hosts.at(ip_address);

    }





    void GizmoParser::addHostMac( const string& ip_address, const string& mac ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.macs.insert(mac);

    }



    void GizmoParser::addHostHostname( const string& ip_address, const string& hostname ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.hostnames.insert(hostname);

    }

    void GizmoParser::addHostHostnameTalkTo( const string& ip_address, const string& hostname_talk_to ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.hostnames_talk_to.insert(hostname_talk_to);

    }




    void GizmoParser::addHostPort( const string& ip_address, int port ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.ports.insert(port);

    }



    void GizmoParser::addHostDeviceLabel( const string& ip_address, const string& device_label ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.device_labels.insert(device_label);

    }



    void GizmoParser::addHostUserAgent( const string& ip_address, const string& user_agent ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.user_agents.insert(user_agent);

    }



    void GizmoParser::addHostSubject( const string& ip_address, const string& subject ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.subjects.insert(subject);

    }



    void GizmoParser::addHostIssuerSubject( const string& ip_address, const string& issuer_subject ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.issuer_subjects.insert(issuer_subject);

    }

    void GizmoParser::addHostSubjectTalkTo( const string& ip_address, const string& subject_talk_to ){

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.subjects_talk_to.insert(subject_talk_to);

    }


    void GizmoParser::addHostIssuerSubjectTalkTo( const string& ip_address, const string& issuer_subject_talk_to ){

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.issuer_subjects_talk_to.insert(issuer_subject_talk_to);

    }



    void GizmoParser::addHostLogType( const string& ip_address, const string& log_type ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.log_types.insert(log_type);

    }



    void GizmoParser::addHostService( const string& ip_address, const string& service ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.services.insert(service);

    }



    void GizmoParser::addHostProtocol( const string& ip_address, const string& protocol ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.protocols.insert(protocol);

    }





    void GizmoParser::incrementHostConnectionCount( const string& ip_address, long connection_count ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.connection_count += connection_count;

    }



    void GizmoParser::incrementHostIncomingBytesCount( const string& ip_address, long incoming_bytes_count ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.incoming_bytes_count += incoming_bytes_count;

    }



    void GizmoParser::incrementHostOutgoingBytesCount( const string& ip_address, long outgoing_bytes_count ) {

        if( ip_address.empty() ) return;
        Host& host = this->getOrCreateHost( ip_address );
        host.outgoing_bytes_count += outgoing_bytes_count;

    }




    const json& GizmoParser::getCertificate( const string& fingerprint ) const{

        return this->x509_certs.at(fingerprint);

    }









}
}
