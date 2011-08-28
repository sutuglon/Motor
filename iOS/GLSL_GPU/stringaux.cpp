/*
 *  string.cpp
 *  GLSprite
 *
 *  Created by Chema on 8/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <string>
#include <sstream>
#include <vector>
#include "stringaux.h"

using namespace std;

namespace stringaux {
    const char* TAB = "   ";
    
    std::string Tabulate(unsigned level) {
        std::string ret;
        for(unsigned i=0;i<level;++i) {
            ret+=TAB;
        }
        return ret;
    }

    void TrimSpaces(std::string& str) {
        size_t startpos = str.find_first_not_of(" \t");
        size_t endpos = str.find_last_not_of(" \t");

        if(( string::npos == startpos ) || ( string::npos == endpos)) {
            str = "";
        }
        else {
            str = str.substr( startpos, endpos-startpos+1 );
        }
    }

    void SplitString(const std::string& line, std::vector<std::string>& ret, const char separator ) {
        std::string str=line;
        TrimSpaces(str);
        istringstream iss(str);
        while ( std::getline(iss, str, separator) )   {
            ret.push_back(str);
        }
    }
}