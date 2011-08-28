/*
 *  Texture.h
 *  TOPD
 *
 *  Created by Chema on 3/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef Texture_H
#define Texture_H

#include "GLInclude.h"
#include <vector>
#include <iostream>
#include <string>

template <typename T = unsigned char>
struct Texture {

    std::vector<T> data_;
    unsigned width_;
    unsigned height_;
    unsigned channels_;
    unsigned linew_;

public:
    Texture() 
    :width_(0)
    ,height_(0)
    ,channels_(0) 
    ,linew_(0){;}
    
    Texture(unsigned w, unsigned h, unsigned channels) 
    :data_(h*((w*channels+3)& ~3),0)
    ,width_(w)
    ,height_(h)
    ,channels_(channels)
    ,linew_((w*channels+3)& ~3) {;}
    
    T& operator[](size_t i) {return data_[i];}
    const T& operator[](size_t i) const {return data_[i];}
    
    unsigned getLineSize() const {return linew_; } // four byte aligned rows
    unsigned getWidth() const {return width_; }
    unsigned getHeight() const {return height_; }
    unsigned getChannels() const {return channels_; }
    
    //TODO:revise access for RGBA
    T& operator()(unsigned row, unsigned col, unsigned cc=0) {return data_[row*linew_+col*channels_+cc];}
    const T& operator()(unsigned row, unsigned col,unsigned cc=0) const {return data_[row*linew_+col*channels_+cc];}

    void flipV() {
        Texture ret(width_,height_,channels_);
        for (unsigned i=0;i<height_;++i) {
            for (unsigned j=0;j<width_;++j) {
                for (unsigned k=0;k<channels_;++k) {
                    ret(height_-1-i,j,k)=(*this)(i,j,k);
                }
            }
        }
        std::swap((*this).data_,ret.data_);
        //return ret;
    }
    
    void flipH() {
        Texture ret(width_,height_,channels_);
        for (unsigned i=0;i<height_;++i) {
            for (unsigned j=0;j<width_;++j) {
                for (unsigned k=0;k<channels_;++k) {
                    ret(i,width_-1-j,k)=(*this)(i,j,k);
                }
            }
        }
        std::swap((*this).data_,ret.data_);
//        return ret;
    }
    
    void print() const {
    for (unsigned i=0;i<height_;++i) {
        for (unsigned j=0;j<width_;++j) {
            std::cout<<"\t(";
            for (unsigned k=0;k<channels_;++k) {
                std::cout<<"\t"<<static_cast<int>((*this).operator()(i,j,k));
            }
            std::cout<<"\t)";
        }
        std::cout<<std::endl;
    }
}
};
 
GLuint CreateGLTexture(const GLenum target);
void SetGLTexturePixels( const Texture<unsigned char>& text, const GLenum texTarget, const GLenum mipmapTarget=0);
void SetGLTexturePixels( const class Image& img, const GLenum texTarget, const GLenum mipmapTarget=0);
void SetGLTextureFilter(const GLenum target, const GLenum minfilter, const GLenum maxfilter);
void SetGLTextureBorder(const GLenum target, const GLenum wraps, const GLenum wrapt);
GLuint makeGLCubeMapTexture(const std::vector<std::string>& faceFiles);
GLuint makeGL2DTexture(const char* file);
GLuint makeGL2DTexture(const Texture<unsigned char>& file);
Texture<unsigned char> make2DTexture(const char* file, bool flipH=false, bool flipV=false); 


#endif
