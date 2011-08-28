/*
 *  object.cpp
 *  GLSL Demo
 *
 *  Created by Chema on 12/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <sstream>
#include <iostream>
#include <fstream>

#include <set>
#include "GLInclude.h"
#include "IO.h"
#include "object.h"


using namespace InputOutput;

namespace  {
    enum eData {
        eNoData = 0x0,
        eVxData = 0x1,
        eVnData = 0x1<<1,
        eTgData = 0x1<<2,
        eFcData = 0x1<<3,
        eUvData = 0x1<<4,
    };
}

void writeObjectToFile(Object& obj, const char* fileName) {
    
	using std::ofstream;
    ofstream fout(fileName, std::ios::binary|std::ios::trunc);
    
    
    if (fout.is_open() && fout.good()) {
        int IOData = eNoData;
        IOData |= obj.vx_.size()>0 ? eVxData:eNoData;
        IOData |= obj.vn_.size()>0 ? eVnData:eNoData;
        IOData |= obj.tg_.size()>0 ? eTgData:eNoData;
        IOData |= obj.fc_.size()>0 ? eFcData:eNoData;
        IOData |= obj.uv_.size()>0 ? eUvData:eNoData;
        
        fout.write(reinterpret_cast<const char*>(&IOData),sizeof(int));
        
        if (IOData & eVxData) {
            write (fout, obj.vx_);   
        }
        if (IOData & eVnData) {
            write (fout, obj.vn_);
        }
        if (IOData & eTgData) {
            write (fout, obj.tg_);
        }
        if (IOData & eFcData){
            write (fout, obj.fc_);
        }
        if (IOData & eUvData) {
            write (fout, obj.uv_);   
        }
        
    }    
    fout.close();
}

void readObjectFromFile(Object& obj, const char* fileName) {
	using std::ifstream;
	
    ifstream fin(fileName, std::ios::binary);
    
    if (fin.is_open()) {
        int IOData;
        
        fin.read(reinterpret_cast<char*>(&IOData),sizeof(int));
        
        
        if (IOData & eVxData) {
            read (fin, obj.vx_);   
        }
        if (IOData & eVnData) {
            read (fin, obj.vn_);
        }
        if (IOData & eTgData) {
            read (fin, obj.tg_);
        }
        if (IOData & eFcData){
            read (fin, obj.fc_);
        }
        if (IOData & eUvData) {
            read (fin, obj.uv_);   
        }
    }
    
    fin.close();
}



void initObjectFromRawData(Object& obj, float verts[][3], unsigned vsize, unsigned faces[][3], unsigned fsize) {
    obj.vx_.reserve(vsize);
    for (unsigned i=0;i<vsize;i++) {
        obj.vx_.push_back(Real3(verts[i][0],verts[i][1],verts[i][2]));
    }
    obj.fc_.reserve(fsize);
    for (unsigned i=0;i<fsize;++i) {
        obj.fc_.push_back(UShort3(faces[i][0],faces[i][1],faces[i][2]));
    }
    obj.vn_.reserve(vsize);
    for (unsigned i = 0; i < vsize; i++) {
        obj.vn_[i]+=Real3(0.);
    }
}

namespace {
    struct UVertex { // unique edges (indices)
        Int3 vx_;
        int setpos_;

        UVertex (const Int3& facevx)
        : vx_(facevx)
        , setpos_(-1)
        {;}
        bool operator<(const UVertex& ref) const {
            return std::lexicographical_compare(&vx_[0], &vx_[2], &ref.vx_[0], &ref.vx_[2]);
        }
    };
    typedef std::set<UVertex> UniqueVxMap; // unique vertex map
}

// WARNING: data.fc_ = vertex index/texture coord ix/normal ix (see objreader)
void initObjectFromOBJData (Object& obj, const ObjReader::Data& data) {
    UniqueVxMap uvm;
    for (unsigned i=0;i<data.fc_.size();i++) {
        Int3 vx = data.fc_[i];
        uvm.insert(UVertex(vx));
    }
    int cc=0;
    UniqueVxMap::iterator itr = uvm.begin();
    while(itr!=uvm.end()) {
        (*const_cast<UVertex*>(&*itr)).setpos_=cc++;
        itr++;
    }
    itr = uvm.begin();
//    while(itr!=uvm.end()) {
//        std::cout<<(*itr).setpos_<<" "<<(*itr).vx_<<std::endl;
//        itr++;
//    }
    
    itr = uvm.begin();
    while(itr!=uvm.end()) {
        Int3 vxdesc = (*itr).vx_;
        obj.vx_.push_back(data.vx_[vxdesc[0]-1]);
        obj.vn_.push_back(data.vn_[vxdesc[2]-1]);
        //std::cout<<obj.vx_[obj.vx_.size()-1]<<obj.vn_[obj.vn_.size()-1]<<std::endl;
        itr++;
    }
    if (data.st_.size()) {
        itr = uvm.begin();
        while(itr!=uvm.end()) {
            obj.uv_.push_back(data.st_[(*itr).vx_[1]-1]);
//            std::cout<<obj.vx_[obj.vx_.size()-1]<<obj.uv_[obj.uv_.size()-1]
//                <<obj.vn_[obj.vn_.size()-1]<<std::endl;
            itr++;
        }
    }
    for (unsigned i=0;i<data.fc_.size();i+=3) {
        UniqueVxMap::iterator itr0 = uvm.find(UVertex(data.fc_[i]));
        UniqueVxMap::iterator itr1 = uvm.find(UVertex(data.fc_[i+1]));
        UniqueVxMap::iterator itr2 = uvm.find(UVertex(data.fc_[i+2]));

        if (itr0!=uvm.end() && itr1!=uvm.end() && itr2!=uvm.end()) {
            //std::cout<<(*itr0).vx_<<(*itr1).vx_<<(*itr2).vx_<<std::endl;
            obj.fc_.push_back(UShort3((*itr0).setpos_,(*itr1).setpos_,(*itr2).setpos_));
            //std::cout<<obj.fc_[obj.fc_.size()-1]<<std::endl;
        }
    }
}


void Object::calculateNormals() {
    size_t vsize = vx_.size();
    size_t fsize = fc_.size();
    vn_.resize(vsize);
    for (unsigned i = 0; i < vsize; i++) {
        vn_[i]=Real3(0.);
    }
    for (unsigned i = 0; i < fsize; i++) {
        const UShort3& face=fc_[i];
        Real3 a = vx_[face[0]];
        Real3 b = vx_[face[1]];
        Real3 c = vx_[face[2]];
        Real3 n = normalize( crossProd( b-a, c-a) );
        
        vn_[face[0]]+=n;
        vn_[face[1]]+=n;
        vn_[face[2]]+=n;
    }
    for (unsigned i = 0; i < vsize; i++) {
        vn_[i]=normalize( vn_[i] );
    }
//    for (unsigned i = 0; i < vsize; i++) {
//        vn_[i]=normalize(vx_[i]);
//    }

}


void Object::calculateTangents() {
    tg_.clear();
    tg_.reserve(vx_.size());
    
    std::vector<Real3> T;
    std::vector<Real3> B;
    
    T.resize(vx_.size());
    B.resize(vx_.size());
    
    for (unsigned i = 0;i<fc_.size(); ++i) {
    
        int i1 = fc_[i][0];
        int i2 = fc_[i][1];
        int i3 = fc_[i][2];
        
        const Real3& v1 = vx_[i1];
        const Real3& v2 = vx_[i2];
        const Real3& v3 = vx_[i3];
        
        const Real2& uv1 = uv_[i1];
        const Real2& uv2 = uv_[i2];
        const Real2& uv3 = uv_[i3];
        
        Real3 q1=v2-v1;
        Real3 q2=v3-v1;
        
        Real2 st1=uv2-uv1;
        Real2 st2=uv3-uv1;
        
        // calculate t and b for this triangle
        // see Mathematics 3d Game programming and Computer Graphics, 2nd ed., section 6.8
        float r = 1.0F / (st1[0] * st2[1] - st2[0] * st1[1]);
        Real3 t = r* Real3((st2[1] * q1[0] - st1[1] * q2[0]) , 
                           (st2[1] * q1[1] - st1[1] * q2[1]) , 
                           (st2[0] * q1[2] - st1[1] * q2[2]));
        Real3 b = r* Real3((st1[0] * q2[0] - st2[0] * q1[0]) , 
                           (st1[0] * q2[1] - st2[0] * q1[1]) , 
                           (st1[0] * q2[2] - st2[0] * q1[2]));
        
        // average tangent (vertex shared by many tris)
        T[i1] += t;
        T[i2] += t;
        T[i3] += t;
        
        B[i1] += b;
        B[i2] += b;
        B[i3] += b;
    }
    
    // gram-schmidt N,T (B is calculated later as obj.tg_[i][3]*crossProd(N[i],T[i])
    const std::vector<Real3>& N = vn_;
    for (unsigned i=0;i<vn_.size();++i) {
        T[i]=normalize(T[i]-dotProd(N[i],T[i])*N[i]);
        real det = (dotProd(T[i], crossProd(B[i], N[i])) < 0.f ? -1.f:1.f);
        tg_.push_back(Real4(T[i][0],T[i][1],T[i][2],det));
    }
}

void Object::centerPivot() {
    // calc pivot
    Real3 pivot(0.);
    for (unsigned i=0;i<vx_.size();i++) {
        pivot+=vx_[i];
    }
    pivot/=static_cast<real>(vx_.size());
    for (unsigned i=0;i<vx_.size();i++) {
        vx_[i]-=pivot;
    }
}

void Object::uniformScale(real value) {
    for (unsigned i=0;i<vx_.size();i++) {
        vx_[i]=value*vx_[i];
    }
}

void Object::uniformTranslate(Real3 vec) {
    for (unsigned i=0;i<vx_.size();i++) {
        vx_[i]+=vec;
    }
}

void prepare4SphereCollision(Object& obj, real radius) {
    for (unsigned i=0;i<obj.vx_.size();i++) {
        obj.vx_[i]+=obj.vn_[i]*radius;
    }
}

DrawObject::DrawObject()
:mx_(Matrix4Identity())
,vao_(0)
,shader_(0)
{
    for (unsigned i=0;i<eTextureTypeSize;i++) {
        textId_[i]=0;
    }
    for (unsigned i=0;i<eVBOTypeSize;i++) {
        vbo_[i]=0;
    }
    
}

void DrawObject::init(const Object& model, unsigned buffUsage) {

    usage_ = buffUsage;
    
    // Create a vertex array object (VAO) to cache model parameters
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_[e_vx]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[e_vx]);
    glBufferData(GL_ARRAY_BUFFER, model.vx_.size()*sizeof(Real3), &model.vx_[0], usage_);
    glEnableVertexAttribArray(eVxIndex);
    glVertexAttribPointer(eVxIndex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        
    if(model.vn_.size()) {
        glGenBuffers(1, &vbo_[e_vn]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_[e_vn]);
        glBufferData(GL_ARRAY_BUFFER, model.vn_.size()*sizeof(Real3), &model.vn_[0], usage_);
        glEnableVertexAttribArray(eVnIndex);
        glVertexAttribPointer(eVnIndex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }
    if(model.tg_.size()) {
        glGenBuffers(1, &vbo_[e_tg]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_[e_tg]);
        glBufferData(GL_ARRAY_BUFFER, model.tg_.size()*sizeof(Real4), &model.tg_[0], usage_);
        glEnableVertexAttribArray(eTgIndex);
        glVertexAttribPointer(eTgIndex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }
    if(model.uv_.size()) {
        glGenBuffers(1, &vbo_[e_uv]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_[e_uv]);
        glBufferData(GL_ARRAY_BUFFER, model.uv_.size()*sizeof(Real2), &model.uv_[0], usage_);
        glEnableVertexAttribArray(eUvIndex);
        glVertexAttribPointer(eUvIndex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }

    if(model.fc_.size()) {
        glGenBuffers(1, &vbo_[e_fc]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[e_fc]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.fc_.size()*sizeof(UShort3), &model.fc_[0], usage_);
    }

    GetGLError();
}

void DrawObject::updateVertexArray(const std::vector<Real3> vx) {
    if (vao_==0) return;
    
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[e_vx]);
    glBufferData(GL_ARRAY_BUFFER, vx.size()*sizeof(Real3), &vx[0], usage_);

}

void DrawObject::destroy()
{
    GLuint index;
    GLuint bufName;
    
    if (vao_==0) return;
    
    glBindVertexArray(vao_);
    
    // For every possible attribute set in the VAO
    for(index = 0; index < 16; index++)
    {
        // Get the VBO set for that attibute
        glGetVertexAttribiv(index , GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (GLint*)&bufName);
        if(bufName)	{
            glDeleteBuffers(1, &bufName);
        }
    }
    // Get any element array VBO set in the VAO
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&bufName);
    if(bufName)	{
        glDeleteBuffers(1, &bufName);
    }
    
    // Finally, delete the VAO
    glDeleteVertexArrays(1, &vao_);
    
    GetGLError();
}

