/*
 *  collide_utility.h
 *  GLSL Demo
 *
 *  Created by Chema on 3/1/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */



#include <vector>
#include "object.h"

namespace ozcollide {
    class AABBTreePoly;
}
void buildCollisionTree(const std::vector<Object>& objs, std::vector<ozcollide::AABBTreePoly*>& coltrees);