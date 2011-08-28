/*
 *  particles.cpp
 *  GLSprite
 *
 *  Created by Chema on 11/23/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "glUtil.h"
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

namespace  {
    const int cubediv = 4;
    std::vector<unsigned> cube[cubediv][cubediv][cubediv];
    
}
void Particles::init(unsigned size,real radius) {

    if (size>pos_.size()) {
        pos_.resize(size);
        vel_.resize(size);
        acc_.resize(size);
        dv_.resize(size);
        da_.resize(size);
    }
    
    
    for (unsigned i=0;i<cubediv;++i) {
        for (unsigned j=0;j<cubediv;++j) {
            for (unsigned k=0;k<cubediv;++k) {
                cube[i][j][k].reserve(size/cubediv);
            }
        }
    }
}

unsigned Particles::size () const {
    return pos_.size();
}
    
void Particles::update(const real& dt, const Real3& gravity, const Real3& shake) {

    const unsigned size = pos_.size();

    static Real3 oldG;

    const real gdiffNorm = (oldG-gravity).norm();
    const real timelapse = 0.3; //sec
    static real cc = timelapse;
    
    // dont overfeed accelerometer
    if (gdiffNorm>0.05 || cc>=timelapse) {
        cc=0;
        for(unsigned i=0;i<size;++i) {
            acc_[i]=gravity;
        }
    }
    else {
        for(unsigned i=0;i<size;++i) {
            acc_[i]=Real3(0.);
        }
        cc+=dt;
    }
    oldG= gravity;

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
//        real dragNorm = dragAcceleration.norm();
//        real aNorm  = a.norm();
//        a -= dragNorm < aNorm ? dragAcceleration : a;
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
//        dv_[i] = Real3(0.);
//        da_[i] = Real3(0.);
    }
}


void randomizeInSphere(Particles& particles, real sphereSize)
{
    const unsigned size = particles.size();
    //const Real3& spherePos = sphere.p_;
    
    particles.pos_.resize(size);
    for (unsigned i=0;i<size;++i) {
        particles.pos_[i]= sphereSize*( Real3(-.5f,-.5f,-.5f)+Real3(randnorm<real>(),randnorm<real>(),randnorm<real>()) ); 
//        particles.pos_[i]= i%2?Real3(sphereSize-0.1,0,0):Real3(-sphereSize+0.1,0,0);
//        particles.vel_[i]= i%2?Real3(2.5,0.5,0):Real3(-3,0,0);
//        particles.acc_[i]=Real3(0.f,0.f,0.f);
    }
}


void collideElastic(Particles& particles, const std::vector<unsigned>& index, real radius) {
    Particles::Positions& pos = particles.pos_;
    Particles::Velocities& vel = particles.vel_;
    Particles::Velocities& dv = particles.dv_;
    //    Particles::Accelerations& da = particles.da_;
    const unsigned size = index.size();
    
    real rad = radius;
    for(unsigned i=0;i<size;++i) {
        unsigned ix = index[i];    
        
        real tmin = std::numeric_limits<float>::max();
        int  jmin =-1;
        
        for (unsigned j=i+1; j<size; ++j) {
            unsigned jx = index[j];
            if (ix!=jx) {
                Real3 vi = vel[ix];
                Real3 pi = pos[ix];
                Real3 pj = pos[jx];
                Real3 d = pi-pj;
                
                real u1,u2;
                if (RaySphere(pi,pi+dv[ix]-dv[jx],pj,2*rad,u1,u2) ) {
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
                        jmin=jx;
                    }
                }
            }
        }
        // collision
        if (jmin!=-1) {
            Real3 pi = pos[ix]+tmin*dv[ix];
            Real3 pj = pos[jmin]+tmin*dv[jmin];
            Real3 d = pi-pj;
            Real3 dnorm = normalize(d);
            
            Real3 dpreacti = dotProd(dv[ix],dnorm)*dnorm;
            Real3 dpreactj = dotProd(dv[jmin],dnorm)*dnorm;
            Real3 dpconti = dv[ix]-dpreacti;
            Real3 dpcontj = dv[jmin]-dpreactj;
            
            Real3 vreacti = dotProd(vel[ix],dnorm)*dnorm;
            Real3 vreactj = dotProd(vel[jmin],dnorm)*dnorm;
            Real3 vconti = vel[ix]-vreacti;
            Real3 vcontj = vel[jmin]-vreactj;
            
            
            // reaction
            dv[ix]=tmin*dv[ix]+(1.-tmin)*(dpconti+dpreactj);
            vel[ix]=vconti+vreactj;
            
            dv[jmin]=tmin*dv[jmin]+(1.-tmin)*(dpcontj+dpreacti);
            vel[jmin]=vcontj+vreacti;
            
            //da[ix] = Real3(0.);
            //da[jmin] = Real3(0.);
        }
    }
}

//
void collideEachOther(Particles& particles, const std::vector<unsigned>& index, real radius) {
    
    Particles::Positions& pos = particles.pos_;
    Particles::Velocities& dv = particles.dv_;
    const unsigned size = index.size();
    
    Particles::Velocities dvic;
    dvic.resize(pos.size(),Real3(0.));
    
    // intercollision
    real rad = radius;
    real r = 2*rad;
    for (unsigned i=0; i<size; ++i) {
        unsigned ix = index[i];
        
        Real3 pi = pos[ix]+dv[ix];
        
        for (unsigned j=0; j<size; ++j) {
            unsigned jx = index[j];
            if (ix!=jx) {
                Real3 pj = pos[jx]+dv[jx];
                Real3 d = pi-pj;
                real dsqn = d.sqrnorm();                
                if (dsqn < r*r) {
                    Real3 nd = normalize(d);
                    dvic[i]+=-d+nd*r;
                }
            }
        }
    }
    for (unsigned i=0; i<size; ++i) {
        if (dvic[i].sqrnorm()!=0) {
            dv[i]+=dvic[i]*0.15;
        }
    }
}


void collideLinearSweep (Particles& particles, real length) {
    
    const real cubedivLength = 2*length/cubediv;
    
    for (unsigned i=0;i<cubediv;++i) {
        for (unsigned j=0;j<cubediv;++j) {
            for (unsigned k=0;k<cubediv;++k) {
                cube[i][j][k].clear();
            }
        }
    }
    
    Particles::Positions& pos = particles.pos_;
    for (unsigned i=0;i<pos.size();++i) {
        Real3 p = Real3(length,length,length) + pos[i];
        int x = p[0]/cubedivLength;
        int y = p[1]/cubedivLength;
        int z = p[2]/cubedivLength;
        cube[x][y][z].push_back(i);
    }
}

void collideElasticLinear (Particles& particles, real radius) {
    for (unsigned i=0;i<cubediv;++i) {
        for (unsigned j=0;j<cubediv;++j) {
            for (unsigned k=0;k<cubediv;++k) {
                collideElastic(particles, cube[i][j][k], radius);
            }
        }
    }
}

void collideEachOtherLinear(Particles& particles, real radius) {
    for (unsigned i=0;i<cubediv;++i) {
        for (unsigned j=0;j<cubediv;++j) {
            for (unsigned k=0;k<cubediv;++k) {
                collideEachOther(particles, cube[i][j][k], radius);
            }
        }
    }
}

//void collideEachOtherQuad(Particles& particles, real radius) {
//    
//    Particles::Positions& pos = particles.pos_;
//    Particles::Velocities& dv = particles.dv_;
//    const unsigned size = pos.size();
//    
//    // intercollision
//    real rad = radius;
//    real r = 2*rad;
//    for (unsigned i=0; i<size; ++i) {
//        real dmin = std::numeric_limits<float>::max();
//        //int  jx = -1;
//        Real3 pi = pos[i]+dv[i];
//        for (unsigned j=0; j<size; ++j) {
//            if (i!=j) {
//                Real3 pj = pos[j]+dv[j];
//                Real3 distance = pi-pj;
//                real dist = distance.sqrnorm();                
//                if (dist < r*r) {
//                    if (dist<dmin) {
//                        Real3 d = normalize(distance);
//                        dv[i]=pj-pi+r*d;
//                    }
//                }
//            }
//        }
//        
//    }
//}

void collideEachOtherQuad2(Particles& particles, real radius) {
    
    Particles::Positions& pos = particles.pos_;
    Particles::Velocities& dv = particles.dv_;
    const unsigned size = pos.size();
    
    Particles::Velocities dvic;
    dvic.resize(pos.size(),Real3(0.));
                
    // intercollision
    real rad = radius;
    real r = 2*rad;
    for (unsigned i=0; i<size; ++i) {

        Real3 pi = pos[i]+dv[i];
        for (unsigned j=0; j<size; ++j) {
            if (i!=j) {
                Real3 pj = pos[j]+dv[j];
                Real3 d = pi-pj;
                real dsqn = d.sqrnorm();                

                if (dsqn < r*r) {
                    Real3 nd = normalize(d);
                    dvic[i]+=-d+nd*r;
                }
            }
        }
    }
    for (unsigned i=0; i<size; ++i) {
        if (dvic[i].sqrnorm()!=0) {
            dv[i]+=dvic[i]*0.15;
        }
    }
}

    //elastic collisions
//void collideElasticQuad(Particles& particles, real radius) {
//    Particles::Positions& pos = particles.pos_;
//    Particles::Velocities& vel = particles.vel_;
//    Particles::Velocities& dv = particles.dv_;
////    Particles::Accelerations& da = particles.da_;
//    const unsigned size = pos.size();
//    
//    real rad = radius;
//    for(unsigned i=0;i<size;++i) {
//        real tmin = std::numeric_limits<float>::max();
//        int  jx =-1;
//
//        for (unsigned j=0; j<size; ++j) {
//            if (i!=j) {
//                Real3 vi = vel[i];
//                Real3 pi = pos[i];
//                Real3 pj = pos[j];
//                Real3 d = pi-pj;
//
//                real u1,u2;
//                if (RaySphere(pi,pi+dv[i]-dv[j],pj,2*rad,u1,u2) ) {
//                    real t = -1;
//                    if ((0<=u1 && u1<=1) && (0<=u2 && u2<=1)) {
//                        t = u2<u1? u2:u1;
//                    }
//                    else if(0<=u1 && u1<=1) {
//                        t = u1;
//                    }
//                    else if(0<=u2 && u2<=1) {
//                        t = u2;
//                    }
//                    else {
//                        t=tmin;
//                    }
//                    
//                    if (t<tmin) {
//                        tmin=t;
//                        jx=j;
//                    }
//                }
//            
//            }
//        }
//        // collision
//        if (jx!=-1) {
//            Real3 pi = pos[i]+tmin*dv[i];
//            Real3 pj = pos[jx]+tmin*dv[jx];
//            Real3 d = pi-pj;
//            Real3 dnorm = normalize(d);
//
//            Real3 dpreacti = dotProd(dv[i],dnorm)*dnorm;
//            Real3 dpreactj = dotProd(dv[jx],dnorm)*dnorm;
//            Real3 dpconti = dv[i]-dpreacti;
//            Real3 dpcontj = dv[jx]-dpreactj;
//            
//            Real3 vreacti = dotProd(vel[i],dnorm)*dnorm;
//            Real3 vreactj = dotProd(vel[jx],dnorm)*dnorm;
//            Real3 vconti = vel[i]-vreacti;
//            Real3 vcontj = vel[jx]-vreactj;
//
//
//            // reaction
//            dv[i]=tmin*dv[i]+(1.-tmin)*(dpconti+dpreactj);
//            vel[i]=vconti+vreactj;
//            
//            dv[jx]=tmin*dv[jx]+(1.-tmin)*(dpcontj+dpreacti);
//            vel[jx]=vcontj+vreacti;
//            
//            //da[i] = Real3(0.);
//            //da[jx] = Real3(0.);
//        }
//    }
//}


namespace  {
    ozcollide::Vec3f toVec3f(const Real3& v) {
        return ozcollide::Vec3f(v[0],v[1],v[2]);
    }
    Real3 toReal3(const ozcollide::Vec3f& v) {
        return Real3(v.x,v.y,v.z);
    }
}

void collideWithPolyTrees(Particles& particles, const Matrix4& txMx, const std::vector<Object>& objs, std::vector<ozcollide::AABBTreePoly*>& coltrees) {

    Particles::Positions pos = particles.pos_;
    Particles::Velocities& vel = particles.vel_;
    Particles::Velocities& dv = particles.dv_;
//    Particles::Accelerations& da = particles.da_;
    const unsigned size = pos.size();
    
    for (unsigned i=0;i<size;i++) {
        pos[i] = Matrix4AffineReal3(txMx, pos[i]);
    }
    
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
                    real k = 1.8;
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

namespace {
    enum EVertexAttrib {
        ePosition
    };
}

void DrawParticles::init(Particles& particles, const Shader& shader, bool moving) {

    moving_ = moving;
    
    Object obj;
    for (unsigned i=0;i<particles.pos_.size();i++) {
        obj.vx_.push_back(particles.pos_[i]);
    }
        
    cloud_.init(obj, moving_? GL_STREAM_DRAW : GL_STATIC_DRAW);
    cloud_.shader_ = &shader;
}

void DrawParticles::draw(const Particles& particles) {
    
    if (moving_) {
        cloud_.updateVertexArray(particles.pos_);
    }
    
    // draw particles
	glBindVertexArray(cloud_.vao_);
	glDrawArrays(GL_POINTS, 0, particles.pos_.size());
}
