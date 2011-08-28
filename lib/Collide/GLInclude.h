#ifndef GLInclude_H
#define GLInclude_H


#ifdef WIN32
#include <windows.h>   // Standard Header For MSWindows Applications
#include <gl/glut.h>   // The GL Utility Toolkit (GLUT) Header
#include <gl/glext.h>
#include <gl/gl.h>
#include <gl/glu.h>
#else
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#endif
