#include "client/TypesenseClient.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <chrono>
#include <memory>



#include "JetStream.h"

#include "stopwatch.h"

#include "Common.h"


namespace jetstream {
namespace client {

    TypesenseClient::TypesenseClient( const string& url, const string& token )
        :connection(url)
    {

        this->connection.setHeader( "X-TYPESENSE-API-KEY", token );

    }

    json TypesenseClient::searchDocuments( const SearchQuery& query ){

        return this->makeGetRequest( "/collections/" + query.collection + "/documents/search?q=" + query.phrase + "&page=" + std::to_string(query.page_number) +  "&per_page=" + std::to_string(query.limit) + "&query_by=" + explode_string(query.search_fields,',') );

    }


    void TypesenseClient::insertDocuments( const string& collection, const vector<json>& documents ){

        string body;
        const size_t num_docs = documents.size();
        size_t x = 0;

        for( const json& document : documents ){

            body += document.dump();
            x++;

            if( x < num_docs ){
                body += "\n";
            }

        }

        this->makePostRequest( "/collections/" + collection + "/documents/import?action=create", body );

    }






    void TypesenseClient::insertDocuments( shared_ptr<SearchableModelSet> model_set ){

        auto models_json = model_set->getSearchModelsJson();
        this->insertDocuments( model_set->getSearchIndexName(), *models_json );

    }


    void TypesenseClient::createCollection( shared_ptr<SearchableModelSet> model_set ){

        string body = model_set->getSearchIndexSchema().dump();
        this->makePostRequest( "/collections", body );

    }


    void TypesenseClient::deleteCollection( shared_ptr<SearchableModelSet> model_set ){

        this->makeDeleteRequest( "/collections/" + model_set->getSearchIndexName() );

    }











    json TypesenseClient::makePostRequest( const string& path, const string& body ){

        precise_stopwatch stopwatch;

        try{

            httplib::Result http_result = this->connection.http_client->Post( path.c_str(), this->connection.request_headers, body, "text/plain" );
            cout << "End POST request: " << stopwatch.elapsed_time_ms() << "ms" << endl;

            if( http_result ){

                const auto http_response = http_result.value();

                if( http_response.status >= 200 && http_response.status < 300 ){

                    cout << http_response.status << " response" << endl;
                    //cout << http_response.body << endl;
                    return json::object();

                }else if( http_response.status == 401 ){

                    // 401-level response;
                    cerr << http_response.status << " response" << endl;

                }else if( http_response.status == 400 || (http_response.status > 401 && http_response.status < 500) ){

                    // 400-level response;
                    cerr << http_response.status << " response" << endl;
                    json result = json::parse( http_response.body );
                    cerr << result.dump(4) << endl;

                }else{

                    // 500-level or 100-level response;
                    cerr << http_response.status << " response" << endl;

                }

            }else{

                // error
                cerr << "No result response" << endl;

            }

        }catch( const std::exception &e ){

            cerr << "Exception: " << e.what() << endl;

        }catch( ... ){

            cerr << "Exception." << endl;

        }

        //cout << "End task: " << stopwatch.elapsed_time_ms() << "ms" << endl;

        return json::object();

    }



    json TypesenseClient::makeGetRequest( const string& path ){

        precise_stopwatch stopwatch;

        try{

            httplib::Result http_result = this->connection.http_client->Get( path.c_str(), this->connection.request_headers );
            cout << "End GET request: " << stopwatch.elapsed_time_ms() << "ms" << endl;

            if( http_result ){

                const auto http_response = http_result.value();

                if( http_response.status >= 200 && http_response.status < 300 ){

                    cout << http_response.status << " response" << endl;
                    //cout << http_response.body << endl;
                    return json::parse(http_response.body);

                }else if( http_response.status == 401 ){

                    // 401-level response;
                    cerr << http_response.status << " response" << endl;

                }else if( http_response.status == 400 || (http_response.status > 401 && http_response.status < 500) ){

                    // 400-level response;
                    cerr << http_response.status << " response" << endl;
                    json result = json::parse( http_response.body );
                    cerr << result.dump(4) << endl;

                }else{

                    // 500-level or 100-level response;
                    cerr << http_response.status << " response" << endl;

                }

            }else{

                // error
                cerr << "No result response" << endl;

            }

        }catch( const std::exception &e ){

            cerr << "Exception: " << e.what() << endl;

        }catch( ... ){

            cerr << "Exception." << endl;

        }

        //cout << "End task: " << stopwatch.elapsed_time_ms() << "ms" << endl;

        return json::array();

    }








    void TypesenseClient::makeDeleteRequest( const string& path ){

        precise_stopwatch stopwatch;

        try{

            httplib::Result http_result = this->connection.http_client->Delete( path.c_str(), this->connection.request_headers );
            cout << "End DELETE request: " << stopwatch.elapsed_time_ms() << "ms" << endl;

            if( http_result ){

                const auto http_response = http_result.value();

                if( http_response.status >= 200 && http_response.status < 300 ){

                    cout << http_response.status << " response" << endl;
                    return;

                }else if( http_response.status == 401 ){

                    // 401-level response;
                    cerr << http_response.status << " response" << endl;

                }else if( http_response.status == 400 || (http_response.status > 401 && http_response.status < 500) ){

                    // 400-level response;
                    cerr << http_response.status << " response" << endl;
                    json result = json::parse( http_response.body );
                    cerr << result.dump(4) << endl;

                }else{

                    // 500-level or 100-level response;
                    cerr << http_response.status << " response" << endl;

                }

            }else{

                // error
                cerr << "No result response" << endl;

            }

        }catch( const std::exception &e ){

            cerr << "Exception: " << e.what() << endl;

        }catch( ... ){

            cerr << "Exception." << endl;

        }

        //cout << "End task: " << stopwatch.elapsed_time_ms() << "ms" << endl;


    }





}
}