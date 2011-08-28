/*
 *  readimg.h
 *  PNG
 *
 *  Created by Chema on 3/4/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <vector>

#ifndef Image_H
#define Image_H

struct Image {
private:
    //std::vector<unsigned char> bits_;
    unsigned char* data_;
    unsigned width_;
    unsigned height_;
    unsigned channels_; // 1 grayscale, 2 grayscale+alpha, 3 rgb, 4 rgba
    
public:
    ~Image();
    bool load(const std::string& file);
    const unsigned char* const getBits() const;
    unsigned getWidth() const;
    unsigned getHeight() const;
    unsigned getChannels() const;

//    bool write(const std::string& file) const;
};

#endif
