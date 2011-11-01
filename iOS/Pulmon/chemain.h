//
//  chemain.h
//  iPhoneGLEssentials
//
//  Created by Chema on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef Chemain_H
#define Chemain_H

#include "vec.h"

struct RunData {
    // multitouch
    Real2 pos_;
    Real2 deltaPos_;
    real deltaDistance_;
    int touchType_;
    int touchFingers_;
    int touchTaps_;
    float slider0Value;

    
    // accelerometer
    Real3 lowAcc_;
    Real3 hiAcc_;
    
    RunData()
    :deltaPos_(0.)
    ,deltaDistance_(0.)
    ,touchType_(0)
    ,touchFingers_(0)
    ,touchTaps_(0)
    ,lowAcc_(0.)
    ,hiAcc_(0.)
    {;}
};

void Init(int backingWidth, int backingHeight);
void ReshapeHandler(int w, int h);

void DisplayHandler(void);
void Cleanup(void);
void IdleHandler(const RunData& data);

#endif