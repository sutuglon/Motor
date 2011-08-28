//
//  FBufferObj.cpp
//  iPhoneGLEssentials
//
//  Created by Chema on 3/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "GLInclude.h"
#include "texture.h"
#include "FBO.h"

namespace {
    void deleteAttachment(GLenum attachment)
    { 
        GLint type,name;
        GLuint objName;
        
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                              attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                              &type);
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, 
                                              attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                              &name);

        objName = ((GLuint*)(&name))[0];
        switch (type) {
            case GL_RENDERBUFFER:
                glDeleteRenderbuffers(1, &objName);
                break;
            case GL_TEXTURE:
                glDeleteTextures(1, &objName);
                break;
        }
    }

    GLuint createFrameBuffer() {
        GLuint ret;
        glGenFramebuffers(1, &ret);
        glBindFramebuffer(GL_FRAMEBUFFER, ret);
        return ret;
    }
    
    GLuint createRenderBuffer() {
        GLuint ret;
        glGenRenderbuffers(1, &ret);
        glBindRenderbuffer(GL_RENDERBUFFER, ret);
        return ret;
    }
    
    GLuint createTextureBuffer(unsigned width, unsigned height) {
        GLuint ret;
        ret = CreateGLTexture(GL_TEXTURE_2D);
#if ESSENTIAL_GL_PRACTICES_IPHONE_OS
        SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
#else
        SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
#endif
        SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        return ret;
    }
}

void FBO::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, name_);
    glBindRenderbuffer(GL_RENDERBUFFER, buffers_[eColor]);
}

int FBO::initToFramebuffer() {
    isTexture_=false;
    name_=createFrameBuffer();
    return buffers_[eColor]= createRenderBuffer();
}

int FBO::initToTexture(GLuint width, GLuint height) {
    isTexture_= true;
    size_[eWidth]=width;
    size_[eHeight]=width;
    name_=createFrameBuffer();
    return buffers_[eColor]= createTextureBuffer(width,height);
}


int FBO::attach() {

    buffers_[eDepth]= createRenderBuffer();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, size_[eWidth], size_[eHeight]);
    
    if (isTexture_) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffers_[eColor], 0);
    }
    else {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffers_[eColor]);
    }
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffers_[eDepth]);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)	{
        std::cout<<"failed to make complete framebuffer object:"<<std::endl<<glCheckFramebufferStatus(GL_FRAMEBUFFER);
		return 0;
	}
    
	GetGLError();
    return name_;
}

// TODO: default destruction, resize
void FBO::destroy()
{ 
	if(0 == name_) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, name_);
    
	// MacOSX has many attachments, OpenGL ES only 1
    GLint maxColorAttachments = 1;
#if !ESSENTIAL_GL_PRACTICES_IPHONE_OS
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
#endif
    for(unsigned i = 0; i < maxColorAttachments; i++) {
        deleteAttachment(GL_COLOR_ATTACHMENT0+i);
	}
    deleteAttachment(GL_DEPTH_ATTACHMENT);
    deleteAttachment(GL_STENCIL_ATTACHMENT);
	
    glDeleteFramebuffers(1,&name_);
    
    name_=0;
    isTexture_=false;
    for(unsigned i=0;i<eBufferTypeSize;i++) {
        buffers_[i]=0;
    }
    size_ = UInt2();
}

UInt2 getRenderBufferSize() {
    int width, height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    return UInt2(width,height);
}

