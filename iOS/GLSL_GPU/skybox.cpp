/*
 *  skybox.cpp
 *  CubeMapFilter
 *
 *  Created by Chema on 2/1/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "texture.h"
#include "skybox.h"

void Skybox::init(const std::vector<std::string>& faceFiles) {

    Texture<unsigned char> textpx = make2DTexture(faceFiles[0].c_str(), true, true);
    textureId[0] = makeGL2DTexture(textpx);
    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);
    Texture<unsigned char> textnx = make2DTexture(faceFiles[1].c_str(), true, true);
    textureId[1] = makeGL2DTexture(textnx);
    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);
    textureId[2] = makeGL2DTexture(faceFiles[2].c_str());
    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);
    textureId[3] = makeGL2DTexture(faceFiles[3].c_str());
    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);
    textureId[4] = makeGL2DTexture(faceFiles[4].c_str());
    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);
    Texture<unsigned char> textnz = make2DTexture(faceFiles[5].c_str(), true, true);
    textureId[5] = makeGL2DTexture(textnz);
    SetGLTextureFilter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
    SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP, GL_CLAMP);
}

namespace {
    // cube ///////////////////////////////////////////////////////////////////////
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

    // color array
    GLfloat colors[] = {1,1,1,  1,1,1,  1,1,1,  1,1,1,              // v0-v1-v2-v3
                        1,1,1,  1,1,1,  1,1,1,  1,1,1,              // v0-v3-v4-v5
                        1,1,1,  1,1,1,  1,1,1,  1,1,1,              // v0-v5-v6-v1
                        1,1,1,  1,1,1,  1,1,1,  1,1,1,              // v1-v6-v7-v2
                        1,1,1,  1,1,1,  1,1,1,  1,1,1,              // v7-v4-v3-v2
                        1,1,1,  1,1,1,  1,1,1,  1,1,1};             // v4-v7-v6-v5

    // index array of vertex array for glDrawElements()
    // Notice the indices are listed straight from beginning to end as exactly
    // same order of vertex array without hopping, because of different normals at
    // a shared vertex. For this case, glDrawArrays() and glDrawElements() have no
    // difference.
    //GLubyte indices[] = {0,1,2,3,
                         //4,5,6,7,
                         //8,9,10,11,
                         //12,13,14,15,
                         //16,17,18,19,
                         //20,21,22,23};

    GLubyte indices[] = {0,1,2, 0,2,3,
                         4,5,6, 4,6,7,
                         8,9,10, 8,10,11,
                         12,13,14, 12,14,15,
                         16,17,18, 16,18,19,
                         20,21,22, 20,22,23};

    GLfloat tcoords[] = {
        // Right face
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        
        // Left Face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        
        // Top Face
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        // Bottom Face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,


        // Front Face (from outside cube)
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,



        // Back Face
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

    };
}

void Skybox::drawCube()
{
    // enable and specify pointers to vertex arrays
    //glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    //glNormalPointer(GL_FLOAT, 0, normals);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, tcoords);

    glPushMatrix();
    //glTranslatef(-2, -2, 0);                // move to bottom-left

    // draw first half (12 elements) {0,1,2,3, 4,5,6,7, 8,9,10,11}
    // tell the driver we use vertices from index 0 to index 11, which means 11-0+1 = 12 vertices
    // So, the driver can prefetch an amount of 12 vertex data prior to rendering
    //
    glBindTexture(GL_TEXTURE_2D, textureId[0]);
    glDrawRangeElements(GL_TRIANGLES, 0, 5, 6, GL_UNSIGNED_BYTE, indices);

    glBindTexture(GL_TEXTURE_2D, textureId[1]);
    glDrawRangeElements(GL_TRIANGLES, 6, 11, 6, GL_UNSIGNED_BYTE, indices + 6);

    glBindTexture(GL_TEXTURE_2D, textureId[2]);
    glDrawRangeElements(GL_TRIANGLES, 12, 17, 6, GL_UNSIGNED_BYTE, indices + 12);

    glBindTexture(GL_TEXTURE_2D, textureId[3]);
    glDrawRangeElements(GL_TRIANGLES, 18, 23, 6, GL_UNSIGNED_BYTE, indices + 18);
 
    glBindTexture(GL_TEXTURE_2D, textureId[4]);
    glDrawRangeElements(GL_TRIANGLES, 24, 29, 6, GL_UNSIGNED_BYTE, indices + 24);

    glBindTexture(GL_TEXTURE_2D, textureId[5]);
    glDrawRangeElements(GL_TRIANGLES, 30, 35, 6, GL_UNSIGNED_BYTE, indices + 30);

    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}
