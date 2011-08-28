/*
 *  skybox.h
 *  CubeMapFilter
 *
 *  Created by Chema on 2/1/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <vector>

// GL Skybox
struct Skybox {
    int textureId[6];
    
    void init(const std::vector<std::string>& files);
    void drawCube();
};