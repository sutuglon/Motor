/*
 *  resource.cpp
 *  FontTool
 *
 *  Created by Chema on 2/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif
#include "resourcemap.h"

namespace {

    std::string getCurrentDir() {
        std::string ret;
        char cCurrentPath[FILENAME_MAX];
        if (GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
            ret.assign( cCurrentPath);
        }
        return ret;
    }
}

ResourceMap::ResourceMap(const std::string& basepath) {
    std::string tmp(basepath);
    if (tmp==""){
        tmp=getCurrentDir();
    }
    base_=tmp;
}

std::string ResourceMap::getPath(const std::string& name) const {
    const std::string PATH_SEPARATOR="/";
    std::string ret = "";
    Map::const_iterator itr = map_.find(name);
    if (itr!=map_.end()) {
        ret = base_+PATH_SEPARATOR+(*itr).second;
    }
    return ret;
}
