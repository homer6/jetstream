#pragma once

#include <aws/s3/S3Client.h>

#include <string>
using std::string;


namespace jetstream{
namespace client {

    class S3Client{

        public:
            S3Client();
            
            bool put( const string& bucket_name, const string& destination_path, const string& contents );
            string get( const string& bucket_name, const string& source_path );

        protected:
            Aws::S3::S3Client s3_client;

    };

}
}
