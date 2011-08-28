/*
 *  skybox.h
 *  CubeMapFilter
 *
 *  Created by Chema on 2/1/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "object.h"
#include <string>
#include <vector>


struct DrawSkybox {

    DrawObject cube_;
    
    void init(const std::vector<std::string>& files, const struct Shader& shader);
    void draw();
};