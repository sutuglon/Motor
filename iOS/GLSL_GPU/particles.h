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
    real radius_;
    
    void init(unsigned size, real particleRadius);
    void update(const real& dt, const Real3& gravity, const Real3& shake);
    void step();
    unsigned size() const;
};


// draw
struct DrawParticles {

    enum eTexture {eDiffuse, eNormal};
    int textId_[2];
    Shader shader_;
    std::vector<unsigned> draworder_;

    void init(const Particles& particles, const std::vector<std::string>& textFiles);
    void draw(const Particles& particles, const Real3& camPos, const Matrix4& camInv, int cubeText);
//    void draw(const Particles& particles, const Real3& camPos);

private:
    void order(const Particles& particles, const Real3& camPos);
};

namespace ozcollide {
    class AABBTreePoly;
}

void randomizeInSphere(Particles& particles, float sphereSize);
void collideElastic(Particles& particles);
void collideEachOther(Particles& particles);
void collideWithPolyTrees(Particles& particles, const std::vector<Object>& objs, std::vector<ozcollide::AABBTreePoly*> coltrees);
void collideWithSphere(Particles& particles, real sphereRadius);


#endif