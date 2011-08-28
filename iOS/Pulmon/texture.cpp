/*
 *  Texture.cpp
 *  TOPD
 *
 *  Created by Chema on 3/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GLInclude.h"
#include "readimg.h"
#include "texture.h"


GLuint CreateGLTexture(const GLenum target)
{
    GLuint ret;    
    glGenTextures( 1, &ret ); 
    glBindTexture( target, ret );
    return ret;
}

void SetGLTexturePixels( const Image& img, const GLenum texTarget, const GLenum mipmapTarget) {
    GLenum format = 
        img.getChannels()==1? GL_LUMINANCE:
        img.getChannels()==3? GL_RGB:GL_RGBA;

    glTexImage2D(texTarget, 0, format, img.getWidth(), img.getHeight(), 0, format, GL_UNSIGNED_BYTE, img.getBits());
    if (mipmapTarget)
        glGenerateMipmap(mipmapTarget);
    GetGLError();
}

void SetGLTexturePixels( const Texture<unsigned char>& text, const GLenum texTarget, const GLenum mipmapTarget) {
    GLenum format = 
        text.getChannels()==1? GL_LUMINANCE:
        text.getChannels()==3? GL_RGB:GL_RGBA;
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// Indicate that pixel rows are tightly packed
	glTexImage2D(texTarget, 0, format, text.getWidth(), text.getHeight(), 0, format, GL_UNSIGNED_BYTE, &text[0]);
    glGenerateMipmap(mipmapTarget);
}

void SetGLTextureFilter(const GLenum target, const GLenum minfilter, const GLenum maxfilter) {
    glTexParameterf( target, GL_TEXTURE_MIN_FILTER, minfilter);
    glTexParameterf( target, GL_TEXTURE_MAG_FILTER, maxfilter );
}

void SetGLTextureBorder(const GLenum target, const GLenum wraps, const GLenum wrapt) {
    glTexParameterf( target, GL_TEXTURE_WRAP_S, wraps );
    glTexParameterf( target, GL_TEXTURE_WRAP_T, wrapt );
}

GLuint makeGL2DTexture(const char* file) {
    GLuint ret = 0;
    ret = CreateGLTexture(GL_TEXTURE_2D);
    Image img;
    if(!img.load(file)) return 0;
    SetGLTexturePixels(img,GL_TEXTURE_2D,GL_TEXTURE_2D);
    return ret;
}


GLuint makeGL2DTexture(const Texture<unsigned char>& text) {
    GLuint ret = 0;
    ret = CreateGLTexture(GL_TEXTURE_2D);
    SetGLTexturePixels(text, GL_TEXTURE_2D, GL_TEXTURE_2D);
    return ret;
}

GLuint makeGLCubeMapTexture(const std::vector<std::string>& faceFiles)
{
    GLuint ret;
    GLenum faceTarget[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    ret = CreateGLTexture(GL_TEXTURE_CUBE_MAP);
    
    for (int i=0; i<6; i++) {
        Image img;
        if(!img.load(faceFiles[i].c_str())) return 0;
        SetGLTexturePixels(img, faceTarget[i]);
    }
    
    GetGLError();
    return ret;
}

Texture<unsigned char> make2DTexture(const char* file, bool flipH, bool flipV) {

    Image img;
    if(!img.load(file) ) return Texture<unsigned char>();
    
    Texture<unsigned char> ret(img.getWidth(),img.getHeight(),img.getChannels());
    memcpy(&ret[0],img.getBits(),img.getWidth()*img.getHeight()*img.getChannels());
    if (flipH) ret.flipH();
    if (flipV) ret.flipV();
    return ret;
}

