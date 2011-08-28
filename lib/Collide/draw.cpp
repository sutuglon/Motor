/*
 *  draw.cpp
 *  GC
 *
 *  Created by Chema on 10/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <vector>
#include <math.h>

#include "GLInclude.h"
#include "draw.h"

using namespace std;

namespace Draw {

void drawSegment (int x0, int y0, int x1, int y1) {
    glBegin (GL_LINES);
      glVertex2i (x0,y0);
      glVertex2i (x1,y1);
    glEnd ();
}

void init(void)
{
//   glEnable (GL_LINE_SMOOTH);
   glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//   glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
   glLineWidth (1.0);
   glPointSize(6.0);

   glClearColor(0.0, 0.0, 0.0, 0.0);
}

void reshape(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h) gluOrtho2D (-1.0, 1.0, -1.0*(GLfloat)h/(GLfloat)w, 1.0*(GLfloat)h/(GLfloat)w);
   else gluOrtho2D (-1.0*(GLfloat)w/(GLfloat)h, 1.0*(GLfloat)w/(GLfloat)h, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void setOrthographicProjection(int w, int h) {
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, w, 0, h);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void setOrthographicCubeProjection(real size) {
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	glOrtho(-size/2,size/2,-size/2,size/2, -size/2, size/2);
    
	glMatrixMode(GL_MODELVIEW);
}


void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawCube(real size) {
//    real hside = size/2;
    glBegin(GL_LINES) ;

    glColor3ub(255,127,127); 
    // back face
    glVertex3f(0,0,0);
    glVertex3f(0,size,0);
    glVertex3f(0,size,0);
    glVertex3f(size,size,0);
    glVertex3f(size,size,0);
    glVertex3f(size,0,0);
    glVertex3f(size,0,0);
    glVertex3f(0,0,0);


    // sides
    glColor3ub(0,255,0); 
    glVertex3f(0,0,size);
    glVertex3f(0,0,0);
    glVertex3f(0,size,size);
    glVertex3f(0,size,0);
    glVertex3f(size,size,size);
    glVertex3f(size,size,0);
    glVertex3f(size,0,size);
    glVertex3f(size,0,0);

    // front face
    glColor3ub(255,0,0); 

    glVertex3f(0,0,size);
    glVertex3f(0,size,size);
    glVertex3f(0,size,size);
    glVertex3f(size,size,size);
    glVertex3f(size,size,size);
    glVertex3f(size,0,size);
    glVertex3f(size,0,size);
    glVertex3f(0,0,size);
    glEnd();

}

void drawAxis(real size) {
    glBegin(GL_LINES) ;
    glColor3ub(255,0,0); 
    glVertex3f(0,0,0);
    glVertex3f(size,0,0);
    glColor3ub(0,255,0); 
    glVertex3f(0,0,0);
    glVertex3f(0,size,0);
    glColor3ub(0,0,255); 
    glVertex3f(0,0,0);
    glVertex3f(0,0,size);
    glEnd();
}

void drawTri(const DReal3& a, const DReal3& b, const DReal3& c){
    glVertex3dv(&a[0]);
    glVertex3dv(&b[0]);
    glVertex3dv(&b[0]);
    glVertex3dv(&c[0]);
    glVertex3dv(&c[0]);
    glVertex3dv(&a[0]);
}

void drawSegment2D(const DReal3& a, const DReal3& b){
    glVertex3dv(&a[0]);
    glVertex3dv(&b[0]);
}



} // namespace
