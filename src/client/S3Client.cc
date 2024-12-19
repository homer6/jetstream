#include "client/S3Client.h"


#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <fstream>
#include <memory>



namespace jetstream {
namespace client {


    bool S3Client::put( const string& bucket_name, const string& destination_path, const string& contents ){

        const Aws::String& bucketName = bucket_name;
              
        const Aws::String& fileName = destination_path;

        Aws::S3::S3Client s3Client;
        
        // Create bucket
        // Aws::S3::Model::CreateBucketRequest bucketRequest;
        // bucketRequest.SetBucket(bucketName);
        // s3Client.CreateBucket(bucketRequest);

        // Upload file
        Aws::S3::Model::PutObjectRequest request;
        request.SetBucket(bucketName);
        request.SetKey(fileName);
        
        std::shared_ptr<Aws::StringStream> data = Aws::MakeShared<Aws::StringStream>("");
        *data << contents.c_str();
        request.SetBody(data);

        auto outcome = s3Client.PutObject(request);

        return outcome.IsSuccess();
    
    }


    string S3Client::get( const string& bucket_name, const string& source_path ){
        
        const Aws::String& bucketName = bucket_name;
        const Aws::String& fileName = source_path;

        Aws::S3::S3Client s3Client;

        Aws::S3::Model::GetObjectRequest object_request;
        object_request.SetBucket(bucketName);
        object_request.SetKey(fileName);

        auto get_object_outcome = s3Client.GetObject(object_request);

        if( get_object_outcome.IsSuccess() ){
            auto& retrieved_file = get_object_outcome.GetResultWithOwnership().GetBody();
            Aws::StringStream ss;
            ss << retrieved_file.rdbuf();
            return ss.str();
        }else{
            throw std::runtime_error("Failed to get object from S3");
        }

    }

}
}