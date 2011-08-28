//
//  FBufferObj.h
//  iPhoneGLEssentials
//
//  Created by Chema on 3/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef FBO_H
#define FBO_H

#include "vec.h"
#include "GLInclude.h"

struct FBO {
    enum eBufferType {eColor, eDepth, eStencil,eBufferTypeSize};
    enum eSizeType {eWidth, eHeight};   
private:
    GLuint name_;
    UInt2 size_;
    bool isTexture_;
    GLuint buffers_[eBufferTypeSize];
    
public:
    
    FBO(): name_(0), isTexture_(false) {
        for(unsigned i=0;i<eBufferTypeSize;i++) {
            buffers_[i]=0;
        }
    }
    ~FBO() {destroy();}
    
    int initToFramebuffer();
    int initToTexture(GLuint width, GLuint height);
    int attach();
    
    void bind();
//    void unbind();
    
    GLuint getName() {return name_;}
    GLuint getBuffer(unsigned ix) {return buffers_[ix];}
    
    UInt2 getSize() {return size_;}
    void setSize(const UInt2& size) {size_=size;}
    
    void destroy();


};

UInt2 getRenderBufferSize();

#endif