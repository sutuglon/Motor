/*
 *  resource.h
 *  FontTool
 *
 *  Created by Chema on 2/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ResourceMap_H
#define ResourceMap_H

#include <map>
#include <string>

struct ResourceMap {
    typedef std::map<std::string, std::string> Map;
    Map map_;
    std::string base_;
    
    ResourceMap(const std::string& basepath="");
    void setBase(const std::string& basepath) {base_=basepath;}
    void addPath(const std::string& name, const std::string& path) {
        map_[name]=path;
    };
    std::string getPath(const std::string& name) const;
};
#endif