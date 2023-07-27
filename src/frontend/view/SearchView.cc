#include "frontend/view/SearchView.h"

namespace jetstream::frontend::view {

    string SearchView::render() const{

        return R"searchui(

<html>
    <head>

        <script>

            window.onload = function(){

                app = {};

                app.search_gizmos = function( searchTerms ){
                    var xhr = new XMLHttpRequest();
                    xhr.addEventListener("load", function(){
                        if (xhr.readyState === xhr.DONE) {
                            if( xhr.status === 200 ){
                                console.log(xhr.response);
                                console.log(xhr.responseText);
                                var search_results_div = document.getElementById('search-results-gizmos');
                                search_results_div.innerHTML = xhr.responseText; // JSON.stringify(xhr.responseJson, one, two, three);
                            }
                        }
                    });
                    xhr.open('GET', '/api/v1/gizmos?limit=10&q=' + searchTerms, true);
                    xhr.withCredentials = true;
                    xhr.send(null);
                };


                app.search_all = function(){
                    var searchTerms = document.getElementById('search-input').value;
                    app.search_gizmos(searchTerms);
                };


            };

        </script>

    </head>
    <body>

        <div>
            <input id="search-input" type="text" onkeyup="app.search_all()">
        </div>

        <table>
            <thead>
                <tr>
                    <td><h1>Gizmos</h1></td>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td valign="top"><pre id="search-results-gizmos"></pre></td>
                </tr>
            </tbody>
        </table>

    </body>
</html>

)searchui";

    }

    string SearchView::getMimeType() const{

        return "text/html";

    }

    ostream& operator<<( ostream& os, SearchView& view ){

        os << view.render();

        return os;

    }

}