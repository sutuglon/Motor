/*
 *  string.h
 *  GLSprite
 *
 *  Created by Chema on 8/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef StringAux_H
#define StringAux_H

#include <string>
#include <vector>

namespace stringaux {
    std::string Tabulate(unsigned level);
    void TrimSpaces(std::string& str);
    void SplitString(const std::string& line, std::vector<std::string>& ret, const char separator) ;
}

#endif