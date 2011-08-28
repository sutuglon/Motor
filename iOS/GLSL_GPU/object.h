/*
 *  object.h
 *  GLSL Demo
 *
 *  Created by Chema on 12/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>
#include <string>

#include "vec.h"
#include "objreader.h"

#ifndef Object_H
#define Object_H

struct Object {
    
    typedef Real3 type_vx;
    typedef Real3 type_vn;
    typedef Real4 type_tg;
    typedef UShort3 type_fc;
    typedef Real2 type_uv;
    
    std::vector<type_vx> vx_; // vertex
    std::vector<type_vn> vn_; // normals
    std::vector<type_tg> tg_; // tangents
    std::vector<type_fc> fc_; // faces
    std::vector<type_uv> uv_; // textcoords
    
//    std::vector<Real3> vx_; // vertex
//    std::vector<Real3> vn_; // normals
//    std::vector<Real4> tg_; // tangents
//    std::vector<Int3> fc_; // faces
//    std::vector<Real2> uv_; // textcoords
    int texture_;

    void calculateNormals();    
    void calculateTangents();
    void centerPivot();
    void uniformScale(real value);
    void uniformTranslate(Real3 vec);

};


void writeObjectToFile(Object& obj, const char* fileName);
void readObjectFromFile(Object& obj, const char* fileName);

void initObjectFromOBJData(Object& obj, const ObjReader::Data& data);
void initObjectFromRawData(Object& obj, float verts[][3], unsigned vsize, unsigned faces[][3], unsigned fsize, float scale = 1.);

void prepare4SphereCollision(Object& obj, real radius);


#endif