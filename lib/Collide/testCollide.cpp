/*
 *  testCollide.cpp
 *  Collide
 *
 *  Created by Chema on 11/7/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "GLInclude.h"
#include "vec.h"
#include "matrix.h"
#include "font.h"
#include "draw.h"
#include "resourcemap.h"
#include "icosahedron.h"
#include "bunny.h"
#include "bunnyh.h"
#include "ozcollide/aabbtree_poly.h"
#include "ozcollide/aabbtreepoly_builder.h"
#include "testCollide.h"


using namespace std;
using namespace Draw;
using namespace ozcollide;

struct Object {
    Real3 p_;
    Matrix4 rot_;

    std::vector<Real3> vx_;
    std::vector<Real3> normals_;
    std::vector<Color3> colors_;
    std::vector<Real3> tvx_;
    std::vector<Real3> tnormals_;
        
    std::vector<Int3> faces_;
    
    Object() 
        :p_(Real3(0.))
        {;}
};

namespace Collide {

    std::vector<string> console;

    int width,height;
    Font font;

    std::vector<Object> objs;
    std::vector<AABBTreePoly*> coltrees;
    
    Vec3f seg[2] = { Vec3f(0,0,0) , Vec3f(1,1,1) };
    std::vector<ozcollide::AABBTreePoly::SegmentColResult> colres; 

    int rotateX=0;
    int rotateY=0;
    dreal rotModelX = 0;
    dreal level = 0.1;
    dreal offY= 0;
    dreal scale=1.;
    bool multiple = true;
    Int2 deltaMousePos;
    Int2 oldMousePos;
    bool showGeom = true;
    bool showWire = true;
    bool showNormals = false;


    struct timeval start, end, idleold, idlenew;
    float lapse;
    float average0,average1,average2;

    void initModel(Object& obj, float data[][3], unsigned indices[][3], unsigned vsize, unsigned fsize, 
                   const Color3& color,real scale) {
        obj.colors_.reserve(vsize);
        for (unsigned i=0;i<vsize;i++) {
            obj.colors_.push_back(color);
        }
        obj.tvx_.resize(vsize);
        obj.vx_.reserve(vsize);
        for (unsigned i=0;i<vsize;i++) {
            obj.vx_.push_back(scale*Real3(data[i][0],data[i][1],data[i][2]));
        }
        
        obj.faces_.reserve(fsize);
        for (unsigned i=0;i<fsize;++i) {
            obj.faces_.push_back(Int3(indices[i][0],indices[i][1],indices[i][2]));
        }
        obj.tnormals_.resize(vsize);
        obj.normals_.reserve(vsize);
        for (unsigned i = 0; i < vsize; i++) {
            obj.normals_[i]+=Real3(0.);
        }
        for (unsigned i = 0; i < fsize; i++) {
            unsigned ia=indices[i][0];
            unsigned ib=indices[i][1];
            unsigned ic=indices[i][2];
            
            Real3 a = obj.vx_[ia];
            Real3 b = obj.vx_[ib];
            Real3 c = obj.vx_[ic];
            Real3 n = normalize( crossProd( b-a, c-a) );
            
            obj.normals_[ia]+=n;
            obj.normals_[ib]+=n;
            obj.normals_[ic]+=n;
        }
        for (unsigned i = 0; i < vsize; i++) {
            obj.normals_[i]=normalize( obj.normals_[i] );
        }
    }


    void transformCoords() {

        //rotModelX += lapse*0.001;
        for (unsigned i=0;i<objs.size();++i) {
            objs[i].rot_ = Rotation(rotModelX, Real3(0,1,0));
        }

        for (unsigned i=0;i<objs.size();++i) {
            Matrix4 rmx = objs[i].rot_;
            SetTranslation(rmx,objs[i].p_+Real3(0,offY,0));
        
            for (unsigned j=0;j<objs[i].vx_.size();++j) {
                objs[i].tvx_[j] =Matrix4Affine (rmx, objs[i].vx_[j]);
                objs[i].tnormals_[j] = Matrix4Rotate(rmx,objs[i].normals_[j]);
            }
        }
    }

    void buildTree(const std::vector<Object>& objs, std::vector<AABBTreePoly*>& coltrees) {
        AABBTreePolyBuilder builder;
        for (unsigned i=0;i<objs.size();i++) {
            std::vector<Polygon> polys;
            polys.resize(objs[i].faces_.size());
            for (unsigned j=0;j<objs[i].faces_.size();j++) {
                polys[j].setIndicesMemory(3, const_cast<int*> (&objs[i].faces_[j][0]));
            }
            
            AABBTreePoly* coltree = builder.buildFromPolys( &polys[0],
												polys.size(),
												reinterpret_cast<const Vec3f *> (&objs[i].tvx_[0]),
												objs[i].tvx_.size());
            coltrees.push_back(coltree);
        }
    }
    
    void testTextOverlay() {
    
        // text overlay
        glColor3ub(220,220,220);
        setOrthographicProjection(width,height);

        glPushMatrix();
        glLoadIdentity();

        glScalef(1,-1,1);

        font.preDrawString();
        real lineFeed = 0;
        for (unsigned i=0;i<console.size();i++) {
            font.drawString(console[i],0,lineFeed-=font.lineHeight());
        }
        font.posDrawString();

        glPopMatrix();

        resetPerspectiveProjection();
    }


    void draw(void)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
//        glEnable(GL_DEPTH_TEST);

        //setOrthographicCubeProjection(1.f);

        glPushMatrix();
        glLoadIdentity();

        glRotatef(rotateX,1,0,0);
        glRotatef(-rotateY,0,1,0);

        glScalef(scale,scale,scale);
        glTranslatef(-0.5,-0.5,-0.5); 

        console.push_back("Hello collisions");
        
        
        glColor3ub(155,155,155);        
        if (showWire) {
            //const size_t size = tris.size();
            glBegin(GL_LINES);
            for (unsigned i=0;i<objs.size();i++) {
                for (unsigned j=0;j<objs[i].faces_.size();j++) {
                    
                    const Real3& a = objs[i].tvx_[objs[i].faces_[j][0]];
                    const Real3& b = objs[i].tvx_[objs[i].faces_[j][1]];
                    const Real3& c = objs[i].tvx_[objs[i].faces_[j][2]];
                    glVertex3fv(&a[0]);
                    glVertex3fv(&b[0]);
                    glVertex3fv(&b[0]);
                    glVertex3fv(&c[0]);
                    glVertex3fv(&c[0]);
                    glVertex3fv(&a[0]);
                }
                if (showNormals) {
                    glColor3ub(0,255,0);
                    for (unsigned j=0;j<objs[i].tnormals_.size();j++) {
                        const Real3& n = objs[i].tnormals_[j];
                        const Real3& v = objs[i].tvx_[j];
                        glVertex3fv(&v[0]);
                        glVertex3fv(&(v+0.01*n)[0]);    
                    }
                }
            }
            glEnd();
        }
        glColor3ub(255,0,0);
//        glBegin(GL_LINES);
//        if (true) {
//            glVertex3f(seg[0].x,seg[0].y,seg[0].z);
//            glVertex3f(seg[1].x,seg[1].y,seg[1].z);
//
//        }
//        glEnd();
//        
//        	public:
//		Vector<const Polygon *> polys_;
//		Vector<int> users_;
//		Vec3f segmentPt0_;
//		Vec3f segmentPt1_;
//	};
      
        
        
        glColor3ub(255,0,0);
        if (showGeom) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
//            glEnable(GL_DEPTH_TEST);

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);

            for(unsigned i=0;i<objs.size();i++) {
                glVertexPointer(3, GL_FLOAT, 0, &objs[i].tvx_[0]);
                glNormalPointer(GL_FLOAT, 0, &objs[i].tnormals_[0]);
                glColorPointer(3,GL_UNSIGNED_BYTE, 0, &objs[i].colors_[0]);
                glDrawElements(	GL_TRIANGLES, objs[i].faces_.size()*3, GL_UNSIGNED_INT, &objs[i].faces_[0]);
            }

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            
            glDisable(GL_LIGHTING);
//            glDisable(GL_DEPTH_TEST);
        }
        
        glDisable(GL_DEPTH_TEST);

        glBegin(GL_LINES);
        for(unsigned i=0;i<objs.size();i++) {
            glColor3ub(0,255,0);
            
            for (unsigned j=0;j<12;j++) {
                BoxEdge e = coltrees[i]->getRootNode()->aabb.getEdge(j);
                Vec3f s = e.p0;
                Vec3f p = e.p1;
                glVertex3f(s.x,s.y,s.z);
                glVertex3f(p.x,p.y,p.z);
            }

            for (unsigned j=0;j<colres[i].polys_.size();j++) {
                const Real3& a = objs[i].tvx_[colres[i].polys_[j]->getIndex(0)];
                const Real3& b = objs[i].tvx_[colres[i].polys_[j]->getIndex(1)];
                const Real3& c = objs[i].tvx_[colres[i].polys_[j]->getIndex(2)];
                    glVertex3fv(&a[0]);
                    glVertex3fv(&b[0]);
                    glVertex3fv(&b[0]);
                    glVertex3fv(&c[0]);
                    glVertex3fv(&c[0]);
                    glVertex3fv(&a[0]);
            }
            Vec3f s = colres[i].segmentPt0_;
            Vec3f p = colres[i].segmentPt1_;
            glVertex3f(s.x,s.y,s.z);
            glVertex3f(p.x,p.y,p.z);
        }
        glEnd();
        glEnable(GL_DEPTH_TEST); 


        drawCube(1.);
        drawAxis(0.01);
//        glDisable(GL_DEPTH_TEST);
            

        glPopMatrix();

        testTextOverlay();

        glutSwapBuffers();
    }
    
    void idle(void) {
        console.clear();
        
        gettimeofday(&idlenew, 0);
        lapse = (idlenew.tv_sec - idleold.tv_sec)* (1e03f) + (idlenew.tv_usec - idleold.tv_usec)*(1e-03f);
        idleold = idlenew;
        
        //transformCoords();

        glutPostRedisplay();
    }


    void keyboard(unsigned char key, int x, int y) {
        switch (key) {
            case 27:  /*  Escape Key  */
                exit(0);
                break;
            case 'x':
                rotateX++;
                break;
            case 'X':
                rotateX--;
                break;
            case 'y':
                rotateY++;
                break;
            case 'Y':
                rotateY--;
                break;
            case '+':
                for(unsigned i=0;i<objs.size();i++) {
                    objs[i].p_[1]+=0.01;
                }
                
//                offY+=0.01;
                break;
            case '-':
                for(unsigned i=0;i<objs.size();i++) {
                    objs[i].p_[1]-=0.01;
                }
//                offY-=0.01;
                break;
            case 'm':
                multiple =!multiple;
                break;
            case 'c':
                init(width,height);
                break;
            case 's':
                showGeom=!showGeom;
                break;
            case 'w':
                showWire=!showWire;
                break;
            case 'a':
                scale+=0.1;
                break;
            case 'A':
                scale-=0.1;
                break;
            case 'r':
                rotModelX+=0.1;
                break;
            default:
                break;
        }
    }
    
    void processMouseActiveMotion(int x, int y) {
        Int2 p(x,y);
        deltaMousePos = p - oldMousePos;
        rotateX += deltaMousePos[1];
        rotateY -= deltaMousePos[0];
        oldMousePos = p;
    }

    void processMousePassiveMotion(int x, int y) {
        Int2 p(x,y);
        deltaMousePos = p - oldMousePos;
        oldMousePos = p;
    }

    void init(unsigned w, unsigned h) {

        ResourceMap resource;
        resource.addPath("Fonts","");
        std::string fontPath = resource.getPath("Fonts")+"monaco11";
        font.load(fontPath.c_str());

        console.reserve(height/font.lineHeight());

        width=w;height=h;

        objs.clear();

        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 2000.0);
        
        glEnable(GL_DEPTH_TEST);
        glutIdleFunc (idle);
        glutDisplayFunc (draw);
        glutKeyboardFunc (keyboard);
               

        //adding here the mouse processing callbacks
        glutMotionFunc(processMouseActiveMotion);
        glutPassiveMotionFunc(processMousePassiveMotion);
        
        GLfloat mat_specular[] = { .5, .5, .5, 1.0 };
        GLfloat mat_shininess[] = { 5.0 };
        GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glShadeModel (GL_SMOOTH);

        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        
        Color3 colors [] = {
            Color3(255,0,0),
            Color3(0,255,0),
            Color3(0,0,255),
            Color3(255,255,0),
            Color3(0,255,255),
            Color3(255,0,255),
            Color3(255,255,255),
        };
        
        int n = 1;
        

        for (unsigned i=0;i<n;i++) {
             size_t size;
//            size = objs.size();
//            objs.push_back(Object());
//            objs[size].p_ = DReal3(randnorm<dreal>(),randnorm<dreal>(),randnorm<dreal>());
//            initModel(objs[objs.size()-1], icosa_vertex, icosa_indices,icosa_vertex_size,icosa_indices_size, colors[(i+5)%7],(7./n));
            size = objs.size();
            objs.push_back(Object());
//            objs[size].p_ =0.8*DReal3(0.1+randnorm<dreal>(),0.1+randnorm<dreal>(),randnorm<dreal>());
            //objs[size].p_ =0.5*DReal3(randnorm<dreal>(),randnorm<dreal>(),randnorm<dreal>());
            objs[size].p_=Real3(0.5,0.,0.5)+0.1*Real3(randnorm<real>(),randnorm<real>(),randnorm<real>());
            initModel(objs[objs.size()-1], bunny_vertex, bunny_indices,bunny_vertex_size,bunny_indices_size, colors[(i)%7],5/*(1./n)*/);            
//            initModel(objs[objs.size()-1], bunnyh_vertex, bunnyh_indices,bunnyh_vertex_size,bunnyh_indices_size, colors[(i)%7],5/*(1./n)*/);
        }

        gettimeofday(&idleold, 0);        
        transformCoords();

        buildTree(objs,coltrees);
        colres.resize(coltrees.size());
        for (unsigned i=0;i<coltrees.size();i++) {
            coltrees[i]->collideWithSegment(seg[0], seg[1], colres[i]);
        }
    }

} // namespace
