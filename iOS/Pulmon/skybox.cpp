/*
 *  skybox.cpp
 *  CubeMapFilter
 *
 *  Created by Chema on 2/1/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "texture.h"
#include "shader.h"
#include "skybox.h"

namespace {
    
    // cube 
    //    v6----- v5
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v7---|-|v4
    //  |/      |/
    //  v2------v3

    // vertex coords array
    GLfloat vertices[] = {1,1,1,  1,-1,1,  1,-1,-1,  1,1,-1,        // v0-v3-v4-v5
                          -1,1,1,  -1,1,-1,  -1,-1,-1,  -1,-1,1,    // v1-v6-v7-v2
                          1,1,1,  1,1,-1,  -1,1,-1,  -1,1,1,        // v0-v5-v6-v1
                          -1,-1,-1,  1,-1,-1,  1,-1,1,  -1,-1,1,    // v7-v4-v3-v2
                          1,1,1,  -1,1,1,  -1,-1,1,  1,-1,1,        // v0-v1-v2-v3
                          1,-1,-1,  -1,-1,-1,  -1,1,-1,  1,1,-1   // v4-v7-v6-v5
                          };        

    // normal array
//    GLfloat normals[] = {0,0,1,  0,0,1,  0,0,1,  0,0,1,             // v0-v1-v2-v3
//                         1,0,0,  1,0,0,  1,0,0, 1,0,0,              // v0-v3-v4-v5
//                         0,1,0,  0,1,0,  0,1,0, 0,1,0,              // v0-v5-v6-v1
//                         -1,0,0,  -1,0,0, -1,0,0,  -1,0,0,          // v1-v6-v7-v2
//                         0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0,         // v7-v4-v3-v2
//                         0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1};        // v4-v7-v6-v5


    GLubyte indices[] = {0,1,2, 0,2,3,
                         4,5,6, 4,6,7,
                         8,9,10, 8,10,11,
                         12,13,14, 12,14,15,
                         16,17,18, 16,18,19,
                         20,21,22, 20,22,23};

    GLfloat tcoords[] = {
        1.0f,1.0f,  1.0f,0.0f,  0.0f,0.0f,  0.0f,1.0f, // Right face
        0.0f,1.0f,  1.0f,1.0f,  1.0f,0.0f,  0.0f,0.0f, // Left Face
        1.0f,1.0f,  1.0f,0.0f,  0.0f,0.0f,  0.0f,1.0f, // Top Face
        0.0f,1.0f,  1.0f,1.0f,  1.0f,0.0f,  0.0f,0.0f, // Bottom Face
        1.0f,0.0f,  0.0f,0.0f,  0.0f,1.0f,  1.0f,1.0f, // Front Face (from outside cube)
        1.0f,0.0f,  0.0f,0.0f,  0.0f,1.0f,  1.0f,1.0f  // Back Face
    };
}



void DrawSkybox::init(const std::vector<std::string>& faceFiles, const struct Shader& shader) {

    Texture<unsigned char> textpx = make2DTexture(faceFiles[0].c_str(), true, true);
    cube_.textId_[DrawObject::eText0] = makeGL2DTexture(textpx);
    SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    
    Texture<unsigned char> textnx = make2DTexture(faceFiles[1].c_str(), true, true);
    cube_.textId_[DrawObject::eText1] = makeGL2DTexture(textnx);
    SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    
    cube_.textId_[DrawObject::eText2] = makeGL2DTexture(faceFiles[2].c_str());
    SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    cube_.textId_[DrawObject::eText3] = makeGL2DTexture(faceFiles[3].c_str());
    SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    cube_.textId_[DrawObject::eText4] = makeGL2DTexture(faceFiles[4].c_str());
    SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    Texture<unsigned char> textnz = make2DTexture(faceFiles[5].c_str(), true, true);
    cube_.textId_[DrawObject::eText5] = makeGL2DTexture(textnz);
    SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    GetGLError();
    
    Object obj;
    for (unsigned i=0;i<24;i++) {
        unsigned ix = 3*i;
        Real3 vx(vertices[ix],vertices[ix+1],vertices[ix+2]);
        obj.vx_.push_back(vx);
    }
    for (unsigned i=0;i<24;i++) {
        unsigned ix = 2*i;
        Real2 uv(tcoords[ix],tcoords[ix+1]);
        obj.uv_.push_back(uv);
    }
    for (unsigned i=0;i<12;i++) {
        unsigned ix = 3*i;
        UShort3 fc(indices[ix],indices[ix+1],indices[ix+2]);
        obj.fc_.push_back(fc);
    }
    cube_.init(obj, GL_STATIC_DRAW);
    cube_.shader_ = &shader;
}

void DrawSkybox::draw()
{
    // TODO use texture shader
   	glBindVertexArray(cube_.vao_);
    
    // draw first half (12 elements) {0,1,2,3, 4,5,6,7, 8,9,10,11}
    // tell the driver we use vertices from index 0 to index 11, which means 11-0+1 = 12 vertices
    // So, the driver can prefetch an amount of 12 vertex data prior to rendering
    //
    // TODO: draw using this
    glBindTexture(GL_TEXTURE_2D, cube_.textId_[DrawObject::eText0]);
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    glBindTexture(GL_TEXTURE_2D, cube_.textId_[DrawObject::eText1]);
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(6*sizeof(GLushort)) );
    glBindTexture(GL_TEXTURE_2D, cube_.textId_[DrawObject::eText2]);
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(12*sizeof(GLushort)) );
    glBindTexture(GL_TEXTURE_2D, cube_.textId_[DrawObject::eText3]);
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(18*sizeof(GLushort)) );
    glBindTexture(GL_TEXTURE_2D, cube_.textId_[DrawObject::eText4]);
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(24*sizeof(GLushort)) );
    glBindTexture(GL_TEXTURE_2D, cube_.textId_[DrawObject::eText5]);
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(30*sizeof(GLushort)) );
}
