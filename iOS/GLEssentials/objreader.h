/*
 *  objreader.h
 *  GLSL Demo
 *
 *  Created by Chema on 12/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ObjReader_H
#define ObjReader_H


#include <vector>
#include <string>
#include "vec.h"

struct ObjReader {
    struct Data {
        std::vector<Real3> vx_;
        std::vector<Real3> vn_;
        std::vector<Real2> st_;
        std::vector<Int3> fc_;  // WARNING: each face element = vxindex, texture coord, normal
        std::string matfile_;
    };
    
    Data read (const char* file);
    
};

#endif