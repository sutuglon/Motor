/*
 *  collide_utility.cpp
 *  GLSL Demo
 *
 *  Created by Chema on 3/1/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ozcollide/aabbtree_poly.h"
#include "ozcollide/aabbtreepoly_builder.h"
#include "collide_utility.h"

using namespace ozcollide;
void buildCollisionTree(const std::vector<Object>& objs, std::vector<ozcollide::AABBTreePoly*>& coltrees) {
    AABBTreePolyBuilder builder;
    for (unsigned i=0;i<objs.size();i++) {
        std::vector<Polygon> polys;
        polys.resize(objs[i].fc_.size());
        for (unsigned j=0;j<objs[i].fc_.size();j++) {
            polys[j].setNbIndices(3);
            polys[j].setIndex(0, objs[i].fc_[j][0]);
            polys[j].setIndex(1, objs[i].fc_[j][1]);
            polys[j].setIndex(2, objs[i].fc_[j][2]);
//            polys[j].setIndicesMemory(3, const_cast<int*> (&objs[i].fc_[j][0]));
        }
        
        AABBTreePoly* coltree = builder.buildFromPolys( &polys[0],
                                            polys.size(),
                                            reinterpret_cast<const Vec3f *> (&objs[i].vx_[0]),
                                            objs[i].vx_.size());
        coltrees.push_back(coltree);
    }
}