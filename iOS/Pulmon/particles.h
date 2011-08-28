/*
 *  particles.h
 *  GLSprite
 *
 *  Created by Chema on 11/23/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
 
 #ifndef Particles_H
 #define Particles_H

#include <vector>
#include <string>
#include "vec.h"
#include "texture.h"
#include "shader.h"

struct Particles
{ 
    typedef std::vector<Real3> Positions;
    typedef std::vector<Real3> Velocities;
    typedef std::vector<Real3> Accelerations;

    Positions pos_;
    Velocities vel_;
    Accelerations acc_;
    Velocities dv_;
    Accelerations da_;

    void init(unsigned size, real particleRadius);
    void update(const real& dt, const Real3& gravity, const Real3& shake);
    void step();

    unsigned size() const;
    
    
};


// draw
struct DrawParticles {

    DrawObject cloud_;
    bool moving_;

    // TODO: build VBO, use PNG library or imageUtil, use collision library
    void init(Particles& particles, const struct Shader& shader, bool moving);
    void draw(const Particles& particles);
    
};

namespace ozcollide {
    class AABBTreePoly;
}

void randomizeInSphere(Particles& particles, float sphereSize);
void collideLinearSweep (Particles& particles, real length);
void collideElasticLinear (Particles& particles, real radius);
void collideEachOtherLinear(Particles& particles, real radius);
void collideEachOtherQuad2(Particles& particles, real radius);
//void collideEachOther(Particles& particles, real radius);
void collideWithPolyTrees(Particles& particles, const Matrix4& txMx, const std::vector<Object>& objs, std::vector<ozcollide::AABBTreePoly*>& coltrees);
//void collideWithPolyTrees(Particles& particles, const std::vector<Object>& objs, std::vector<ozcollide::AABBTreePoly*>& coltrees);
void collideWithSphere(Particles& particles, real sphereRadius);


#endif