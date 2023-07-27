#pragma once

#include <string>
using std::string;

#include <iostream>
using std::ostream;
using std::cout;
using std::endl;
using std::cerr;


namespace jetstream::frontend::view {

    struct SearchView {

        string render() const;

        string getMimeType() const;

        friend ostream& operator<<(ostream& os, SearchView& view);

    };

}



