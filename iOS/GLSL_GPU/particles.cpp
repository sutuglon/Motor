/*
 *  particles.cpp
 *  GLSprite
 *
 *  Created by Chema on 11/23/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <limits>
#include "ozcollide/vec3f.h"
#include "ozcollide/polygon.h"
#include "ozcollide/aabbtree_poly.h"
#include "ozcollide/aabbtreepoly_builder.h"
#include "vec.h"

#include "object.h"
#include "texture.h"
#include "intersection.h"
#include "particles.h"


void Particles::init(unsigned size,real radius) {

    if (size>pos_.size()) {
        pos_.resize(size);
        vel_.resize(size);
        acc_.resize(size);
        dv_.resize(size);
        da_.resize(size);
    }
    radius_=radius;
}

unsigned Particles::size () const {
    return pos_.size();
}
    
void Particles::update(const real& dt, const Real3& gravity, const Real3& shake) {

    const unsigned size = pos_.size();
    // reset system
    for(unsigned i=0;i<size;++i) {
        acc_[i]=Real3(0.f,0.f,0.f);
    }


 //gravity
//    static int cc=0;
//    for(unsigned i=0;i<size;++i) {
//        if (cc) {
//        acc_[i]=i%2?Real3(0.f,0.f,-10.f):Real3(0.f,0.f,1.f);
//        cc--;
//        }
//        else {
//        acc_[i]=i%2?Real3(0.f,0.f,1.f):Real3(0.f,0.f,-1.f);
//        }
//    }


//    const real g = 9.8; // meters per square second
//    Real3 gDir = gravity;
//    if (gDir.sqrnorm() != 0) {
//        gDir.norm();
//        gDir=gDir*g;
//        //gDir=Real3(0.f,-g,0.f);
//    }
//    Real3 sDir = shake;
//    if (sDir.sqrnorm() != 0) {
//        sDir.norm();
//        sDir=sDir*g;
//    }    
    
//    const Real3 shakeDiff=shake-gravity;
////    const real shakelen = shakeDiff.norm();
//    
    for(unsigned i=0;i<size;++i) {
        acc_[i]+=/*gDir*(1.0f-shakelen) +*/ shake;
    }


  // drag (for small particles in viscous flows)
    // f = bv where b is 6*pi*n*r 
    // n =fluid viscosity =10e-3 Pa*s as dynamic viscosity of water in SI
    // r=10e-3 meters
    // v = ((m*a)/b)*(1-eE(-(b*t)/m))
//    const real n=1e-3f; 
//    const real r=10e-3f;
//    const real mass = 1e-5f;
//    
//    real b = 6*Pi*n*r;
//    
//    for(unsigned i=0;i<size;++i) {
//        Real3& v=vel_[i];
//        Real3& a =acc_[i];
//        Real3 dragForce = v*b;
//        Real3 dragAcceleration = dragForce/mass;
//        a -= dragAcceleration.norm() < a.norm() ? dragAcceleration : a;
//    }


 // weight over time these two (gravity and fluid acceleration)
//    // fuid acceleration
//        for(unsigned i=0;i<size;++i) {
//            const Real3 fluid = normalize( Real3(-.5f,-.5f,-.5f) + Real3(randnorm<real>(),randnorm<real>(),randnorm<real>()) );
//            acc_[i]+= shakelen*2.f*fluid;
//    }


    // gather acceleration & velocity deltas
    for(unsigned i=0;i<size;++i) {
        da_[i] = acc_[i]*dt;
        dv_[i] = (vel_[i]+da_[i])*dt;
    }    
}
    

// linear increment of velocity (half the area)
void Particles::step() {
    const unsigned size = pos_.size();
    for(unsigned i=0;i<size;++i) {
        pos_[i]+=dv_[i];
        vel_[i]+=da_[i];
    }
}

    
//    if (size>=3) {
//        pos_[0] = Real3(0.0f,0.5f,-1.0f);
//        pos_[1] = Real3(0.0f,0.5f,-1.0f)+gDir;
//        pos_[2] = Real3(0.0f,0.5f,-1.0f)+gDiff;
//    }


void randomizeInSphere(Particles& particles, real sphereSize)
{
    const unsigned size = particles.size();
    //const Real3& spherePos = sphere.p_;
    
    particles.pos_.resize(size);
    for (unsigned i=0;i<size;++i) {
        particles.pos_[i]= sphereSize*( Real3(-.5f,-.5f,-.5f)+Real3(randnorm<real>(),randnorm<real>(),randnorm<real>()) ); 
//        particles.pos_[i]= i%2?Real3(0,0,2):Real3(0,0,-2);
//        particles.vel_[i]= i%2?Real3(0,0.1,2.9):Real3(0,0,-3);
//        particles.acc_[i]=Real3(0.f,0.f,0.f);
    }
}

    //elastic collisions
void collideElastic(Particles& particles) {
    Particles::Positions& pos = particles.pos_;
    Particles::Velocities& vel = particles.vel_;
    Particles::Velocities& dv = particles.dv_;
//    Particles::Accelerations& da = particles.da_;
    const unsigned size = pos.size();
    
    real rad = particles.radius_*0.015;
    for(unsigned i=0;i<size;++i) {
        real tmin = std::numeric_limits<float>::max();
        int  jx =-1;

        for (unsigned j=0; j<size; ++j) {
            if (i!=j) {
                Real3 vi = vel[i];
                Real3 pi = pos[i];
                Real3 pj = pos[j];
                Real3 d = pi-pj;

                real u1,u2;
                if (RaySphere(pi,pi+dv[i]-dv[j],pj,2*rad,u1,u2) ) {
                    real t = -1;
                    if ((0<=u1 && u1<=1) && (0<=u2 && u2<=1)) {
                        t = u2<u1? u2:u1;
                    }
                    else if(0<=u1 && u1<=1) {
                        t = u1;
                    }
                    else if(0<=u2 && u2<=1) {
                        t = u2;
                    }
                    else {
                        t=tmin;
                    }
                    
                    if (t<tmin) {
                        tmin=t;
                        jx=j;
                    }
                }
            
            }
        }
        // collision
        if (jx!=-1) {
            Real3 pi = pos[i]+tmin*dv[i];
            Real3 pj = pos[jx]+tmin*dv[jx];
            Real3 d = pi-pj;
            Real3 dnorm = normalize(d);

            Real3 dpreacti = dotProd(dv[i],dnorm)*dnorm;
            Real3 dpreactj = dotProd(dv[jx],dnorm)*dnorm;
            Real3 dpconti = dv[i]-dpreacti;
            Real3 dpcontj = dv[jx]-dpreactj;
            
            Real3 vreacti = dotProd(vel[i],dnorm)*dnorm;
            Real3 vreactj = dotProd(vel[jx],dnorm)*dnorm;
            Real3 vconti = vel[i]-vreacti;
            Real3 vcontj = vel[jx]-vreactj;


            // reaction
            dv[i]=tmin*dv[i]+(1.-tmin)*(dpconti+dpreactj);
            vel[i]=vconti+vreactj;
            
            dv[jx]=tmin*dv[jx]+(1.-tmin)*(dpcontj+dpreacti);
            vel[jx]=vcontj+vreacti;
            
            //da[i] = Real3(0.);
            //da[jx] = Real3(0.);
        }
    }
}


namespace  {
    ozcollide::Vec3f toVec3f(const Real3& v) {
        return ozcollide::Vec3f(v[0],v[1],v[2]);
    }
    Real3 toReal3(const ozcollide::Vec3f& v) {
        return Real3(v.x,v.y,v.z);
    }
}

void collideWithPolyTrees(Particles& particles, const std::vector<Object>& objs, std::vector<ozcollide::AABBTreePoly*> coltrees) {

    Particles::Positions& pos = particles.pos_;
    Particles::Velocities& vel = particles.vel_;
    Particles::Velocities& dv = particles.dv_;
//    Particles::Accelerations& da = particles.da_;
    const unsigned size = pos.size();
    
    for (unsigned i=0;i<size;i++) {
        Real3 pi = pos[i];
        Real3 pf = pi+dv[i];
        std::vector<ozcollide::AABBTreePoly::SegmentColResult> colres(coltrees.size()); 
        for (unsigned j=0;j<coltrees.size();j++) {
            coltrees[j]->collideWithSegment(toVec3f(pi),toVec3f(pf), colres[j]);
        }
        for (unsigned j=0;j<coltrees.size();j++) {
            real tmin = std::numeric_limits<float>::max();
            Real3 nmin(0.);
            const ozcollide::Polygon* pmin = 0;
            
            for (unsigned k=0;k<colres[j].polys_.size();k++) {
                const ozcollide::Polygon* poly= colres[j].polys_[k];
                const Real3& a = objs[j].vx_[poly->getIndex(0)];
                const Real3& b = objs[j].vx_[poly->getIndex(1)];
                const Real3& c = objs[j].vx_[poly->getIndex(2)];
                

//                const Real3& an = objs[j].vn_[poly->getIndex(0)];
//                const Real3& bn = objs[j].vn_[poly->getIndex(1)];
//                const Real3& cn = objs[j].vn_[poly->getIndex(2)];
               
                ozcollide::Plane plane;
                plane.fromPoints(toVec3f(a),toVec3f(b), toVec3f(c) );
                real t;
                if (plane.intersectWithLine(toVec3f(pi),toVec3f(pf),t)) {
                    if(t<tmin) {
                        tmin=t;
                        pmin = poly;
                        nmin = normalize( crossProd( b-a, c-a) );
                    }
                }
                // reaction
                if (pmin) {
//                    Real3 dvi = dv[i];
//                    Real3 x = pi+tmin*dvi;
//                    pos[i]=x+(1.-tmin)*(dvi.norm())*nmin; // separate from geometry
                    dv[i] = tmin*dv[i]+(1.-tmin)*dv[i].norm()*nmin;

                    // perfect bounce == 2 slip walls = 1
                    real k = 1.5;
                    vel[i]+=-k*dotProd(vel[i],nmin)*nmin; // bounce
                    vel[i]*=0.95;
                    // this gets inside geometry often
//                    pos[i]=x+(1.-tmin)*(dv[i].norm())*nmin;
//                    vel[i]=vel[i].norm()*nmin;
                    
//                    dv[i]=Real3(0.);
//                    da[i]= Real3(0.);

                }
            }
        }
    }
}

void collideEachOther(Particles& particles) {

    Particles::Positions& pos = particles.pos_;
    Particles::Velocities& dv = particles.dv_;
    const unsigned size = pos.size();

    // intercollision
    real rad = particles.radius_*0.015;
    real r = 2*rad;
    for (unsigned i=0; i<size; ++i) {
        real dmin = std::numeric_limits<float>::max();
        //int  jx = -1;
        Real3 pi = pos[i]+dv[i];
        for (unsigned j=0; j<size; ++j) {
            if (i!=j) {
                Real3 pj = pos[j]+dv[j];
                Real3 distance = pi-pj;
                real dist = distance.sqrnorm();                
                if (dist < r*r) {
                    if (dist<dmin) {
                        Real3 d = normalize(distance);
                        dv[i]=pj-pi+r*d;
                    }
                }
            }
        }

    }
}

void collideWithSphere(Particles& particles, real sphereRadius)
{
    Particles::Positions& pos = particles.pos_;
    Particles::Velocities& dv = particles.dv_;
    Particles::Velocities& vel = particles.vel_;

    const unsigned size = pos.size();

    // collision with glass
    for (unsigned i=0; i<size; ++i) {
        Real3 distance = pos[i]+dv[i];
        if (distance.sqrnorm() > sphereRadius*sphereRadius) {
            Real3 d = normalize(distance);
            //pos[i]= sphereRadius*d;
            dv[i]=sphereRadius*d-pos[i];
            // perfect bounce == 2 slip walls = 1
            real k = 1.8; 
            vel[i]+= -k*dotProd(vel[i],d)*d;
            // friction
            vel[i]*=0.95;
        }
    }

}

// Draw

namespace {
    const std::vector<Real3>* sortPositions;
    Real3 sortRef;
    class furtherFirst {
    public:
        bool operator()(const size_t& lhs, const size_t& rhs) const {
            return ((*sortPositions)[lhs]-sortRef).sqrnorm() > ((*sortPositions)[rhs]-sortRef).sqrnorm();
        }
    };
    std::vector<Vec<unsigned char,4> > colors(1.0);
}

void DrawParticles::init(const Particles& particles, const std::vector<std::string>& textFiles) {

    shader_.setup("particles.vert","particles.frag");
//    textId_[eDiffuse] = makeGL2DTexture(textFiles[eDiffuse].c_str());
//    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
//    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);
//    textId_[eNormal] = makeGL2DTexture(textFiles[eNormal].c_str());
//    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
//    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);

    const size_t size = particles.pos_.size();
    draworder_.resize(size);
    for (unsigned i=0; i<size; ++i) {
        draworder_[i]=i;
    }

}

void DrawParticles::order(const Particles& particles, const Real3& camPos) {
    // sort particles before draw
    const size_t size = particles.pos_.size();
    for (unsigned i=0; i<size; ++i) {
        draworder_[i]=i;
    }
    sortPositions=&(particles.pos_);
    sortRef=camPos;
    std::sort(draworder_.begin(),draworder_.end(), furtherFirst());
}

void DrawParticles::draw(const Particles& particles, const Real3& camPos, const Matrix4& camInv, int cubeText)
{
    order(particles, camPos);
    
    // avoid problem with overlapping alpha blends
    //glDepthMask(false);  // protect depth buffer by making it read-only

    shader_.bind();
        
    glUniformMatrix4fv(glGetUniformLocation(shader_.getId(),"cam2world"), 1, 0, &camInv[0]);
    glUniform1f(glGetUniformLocation(shader_.getId(),"mixfactor"), 0.4);
    glUniform1i(glGetUniformLocation(shader_.getId(), "cubeMapTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeText);

    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);

    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    

    // GL_POINT_SMOOTH
    
//    SetTexture(particles.texture_);

//    real distance_attenuation[3] = {1.0f, .0f, 1.0f}; 
//    glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance_attenuation);


    glPointSize(particles.radius_);
    /*
    int loc= glGetUniformLocation(shader_.getId(), "colorTexture");
    glUniform1i(glGetUniformLocation(shader_.getId(), "colorTexture"), 0);
    glActiveTexture(GL_TEXTURE0);        
    glBindTexture(GL_TEXTURE_2D, textId_[eDiffuse]);*/
    

    glVertexPointer(3, GL_FLOAT, 0, &particles.pos_[0]);
    //glColorPointer(3, GL_FLOAT, 0, &colors[0]);
    glDrawElements(GL_POINTS,  draworder_.size(), GL_UNSIGNED_INT, &draworder_[0]);
    
    //glDrawArrays(GL_POINTS, 0, particles.pos_.size());

//    UnsetTexture();
    
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);
    glDisable(GL_POINT_SPRITE);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);  
    
    shader_.unbind();  

    //glDepthMask(true);
}
