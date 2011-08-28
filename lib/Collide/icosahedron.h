/*
 *  icosahedron.h
 *  VSweep
 *
 *  Created by Chema on 8/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
 
 #ifndef icosahedron_H
 #define icosahedron_H

#define X .0525731112119133606
#define Z .0850650808352039932


unsigned icosa_vertex_size = 12;
float icosa_vertex[12][3] = {    
    {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
    {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
    {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};
unsigned icosa_indices_size = 20;
unsigned int icosa_indices[20][3] = { 
    {0,1,4}, {0,4,9}, {4,1,8}, 
    {8,1,10}, {8,10,3}, {5,8,3}, {5,3,2},
    {9,5,2}, {9,2,11},{9,11,0}, 
    {2,3,7} , {7,11,2},{9,4,5},{4,8,5},
    {7,3,10}, {7,10,6},{7,6,11}, {11,6,0},{0,6,1}, {6,10,1},};
    
#endif