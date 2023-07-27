#pragma once

#include "config/IngestWriterConfig.h"
using jetstream::config::IngestWriterConfig;

//#include "client/KafkaProducer.h"
//using jetstream::client::KafkaProducer;

#include "thread_pool.hpp"

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <mutex>

#include <set>
using std::set;

#include <map>
using std::map;

#include "json.hpp"
using json = nlohmann::json;


namespace jetstream{
namespace parser{

    struct Host{

        Host(){}
        Host( const string& ip )
            :ip(ip)
        {

        }


        string ip;

        set<string> hostnames;
        set<string> hostnames_talk_to;
        set<string> macs;

        long connection_count = 0;
        long incoming_bytes_count = 0;
        long outgoing_bytes_count = 0;
        set<int> ports;
        set<string> device_labels;
        set<string> user_agents;

        set<string> subjects;
        set<string> issuer_subjects;

        set<string> subjects_talk_to;
        set<string> issuer_subjects_talk_to;

        set<string> log_types;
        set<string> services;
        set<string> protocols;
    };

    struct ConnectionSummary{
        string source_ip;
        string destination_ip;
        string service;
        long connection_count = 1;
        long source_bytes_count = 0;
        long destination_bytes_count = 0;
    };


	class GizmoParser{

	    public:
            GizmoParser( const ::jetstream::config::IngestWriterConfig& config, const vector<string>& command_line_arguments );
            void parse();

        protected:
            void walkStage1( const int number_of_rows = -1 );
            void walkAll( const int number_of_rows = -1 );

            void printAll();


            void addConnection( const json& connection );


            Host& getOrCreateHost( const string& ip_address );

            void addHostMac( const string& ip_address, const string& mac );

            void addHostHostname( const string& ip_address, const string& hostname );
            void addHostHostnameTalkTo( const string& ip_address, const string& hostname_talk_to );

            void addHostPort( const string& ip_address, int port );
            void addHostDeviceLabel( const string& ip_address, const string& device_label );
            void addHostUserAgent( const string& ip_address, const string& user_agent );

            void addHostSubject( const string& ip_address, const string& subject );
            void addHostIssuerSubject( const string& ip_address, const string& issuer_subject );

            void addHostSubjectTalkTo( const string& ip_address, const string& subject_talk_to );
            void addHostIssuerSubjectTalkTo( const string& ip_address, const string& issuer_subject_talk_to );

            void addHostLogType( const string& ip_address, const string& log_type );
            void addHostService( const string& ip_address, const string& service );
            void addHostProtocol( const string& ip_address, const string& protocol );
            void incrementHostConnectionCount( const string& ip_address, long connection_count = 1 );
            void incrementHostIncomingBytesCount( const string& ip_address, long incoming_bytes_count );
            void incrementHostOutgoingBytesCount( const string& ip_address, long outgoing_bytes_count );

            const json& getCertificate( const string& fingerprint ) const;




            IngestWriterConfig config;
            vector<string> command_line_arguments;
            //KafkaProducer error_producer;
            string source_filepath;

            set<string> ips;
            map<string, set<string>> ips_by_log_type;

            set<string> hostnames;
            map<string, set<string>> hostnames_by_log_type;

            set<string> user_agents;
            map<string, set<string>> user_agents_by_log_type;

            set<string> subjects;
            map<string, set<string>> subjects_by_log_type;

            set<string> issuer_subjects;
            map<string, set<string>> issuer_subjects_by_log_type;

            set<string> mac_addresses;
            map<string, set<string>> mac_addresses_by_log_type;


            map<string, ConnectionSummary> tcp_connection_summaries;
            map<string, ConnectionSummary> udp_connection_summaries;

            map<string, Host> hosts;  //ip to host
            //map<string, string> mac_to_ip;  //

            std::mutex shared_mutex;
            std::mutex summaries_mutex;
            std::mutex host_mutex;
            thread_pool pool{12, 100};

            map<string, json> x509_certs;  // fingerprint -> x509 json

            const set<string> known_services = { "http", "rdp", "ssl", "ssh", "ntp", "smtp", "ftp", "dns", "dhcp" };

	};

}
}

