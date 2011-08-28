/*
 *  readimg.cpp
 *  PNG
 *
 *  Created by Chema on 3/4/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <algorithm>
#include <cctype>
#include "readpng.h"
#include "readbmp.h"
#include "readimg.h"

#include "imageUtil.h"


// TODO : include objective C code for reading png

namespace  {
    std::string getExtension(const std::string& file) {
        return file.substr(file.length()-3);
    }
    void toUpper(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) toupper);
    }
}

const unsigned char* const Image::getBits() const {return data_;}
const unsigned char*  Image::getBits()  {return data_;}
void Image::setBits(const unsigned char* bits) {memcpy(data_, bits, width_*height_*channels_);}
unsigned Image::getWidth() const {return width_;}
unsigned Image::getHeight() const {return height_;}
unsigned Image::getChannels() const {return channels_;}

Image::~Image() {
    delete data_;
}

bool Image::load(const std::string& file) {
    bool ret=false;

    std::string type =getExtension(file);
    toUpper(type);
    if (type=="PNG") {
#if IOS_CPNG_PREPROCESSOR_MACRO
        iOSLoadImage( file.c_str(), data_, width_, height_, channels_,1);
#else 
        FILE* pngFile = fopen(file.c_str(), "rb");
        if (!pngFile) { return ret;}
        
        readpng_init(pngFile, &width_, &height_);
        
        unsigned rowBytes;
        readpng_get_image( &data_, &channels_, &rowBytes);
        if (!data_) { return ret; }
        
        readpng_cleanup(FALSE);
        fclose(pngFile);
#endif
    }
    else if (type=="BMP") {
#if IOS_CPNG_PREPROCESSOR_MACRO
#else
        BITMAPINFO *BitmapInfo=0;
        unsigned char* datargb = LoadDIBitmap(file.c_str(), &BitmapInfo);
        if (!BitmapInfo) {
            return ret;
        }
        width_=BitmapInfo->bmiHeader.biWidth;
        height_=BitmapInfo->bmiHeader.biHeight;
        channels_=4;
        
        if (BitmapInfo->bmiHeader.biBitCount>>3!=3) {return ret;}
        
        unsigned char* pDataRGBA=data_=(unsigned char *)malloc(width_*height_*channels_);
        int length = (width_ * 3 + 3) & ~3; // four byte aligned GBR packed rows
        for (int y = 0; y < height_; y ++) {
            unsigned char* pDataRGB = datargb + y*length;
            unsigned char* pDataRGBEnd = pDataRGB + width_*3;
            while (pDataRGB!=pDataRGBEnd) {
                *pDataRGBA++=*(pDataRGB+2);
                *pDataRGBA++=*(pDataRGB+1);
                *pDataRGBA++=*(pDataRGB);
                *pDataRGBA++=255;
                pDataRGB+=3;
            }
        }
        free(BitmapInfo);
        free(datargb);
#endif
    }
    return ret=true;
}

bool Image::write(const std::string& file) {
    bool ret=false;
#if IOS_CPNG_PREPROCESSOR_MACRO
#else
    std::string type =getExtension(file);
    toUpper(type);
    if (type=="PNG") {
        FILE *pngFile = fopen(file.c_str(), "wb");
        if (!pngFile) { return ret;}

        if (writepng_image(pngFile, data_, width_, height_, channels_)!=0) 
            return ret;

        writepng_cleanup();
        fclose(pngFile);
    }
    else if (type=="BMP") {
        // TODO
    }
#endif
    return ret=true;
}

