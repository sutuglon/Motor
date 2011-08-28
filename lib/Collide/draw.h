/*
 *  draw.h
 *  GC
 *
 *  Created by Chema on 10/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef Draw_H
#define Draw_H

#include "vec.h"
#include <vector>

namespace Draw {

    void init(void);
    void reshape(int w, int h);
    void drawSegment (int x0, int y0, int x1, int y1);
    void setOrthographicProjection(int w, int h);
    void setOrthographicCubeProjection(real size);
    void resetPerspectiveProjection();
 
    void drawCube(real size);
    void drawAxis(real size);
    void drawTri(const DReal3& a, const DReal3& b, const DReal3& c);
    void drawSegment2D(const DReal3& a, const DReal3& b);
}

#endif