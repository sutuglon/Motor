/*
 *  shader.h
 *  GLSL Demo
 *
 *  Created by Chema on 12/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef SHADER_H
#define SHADER_H

#include <iostream>

struct Shader {

    GLuint	g_vertex_shader;
    GLuint	g_fragment_shader;
    GLuint	g_geometry_shader;
    GLuint	g_program;

    ~Shader();

    unsigned int getId();
        
    void retrieveShaderLog(std::ostream& os, GLuint obj);
    void retrieveProgramLog(std::ostream& os, GLuint obj);
    std::string readShader(const char* pszFilename);
    void setup(const std::string& vxshader, const std::string& fragshader);
    void printActiveAttribs();
    void printActiveUniforms();
    void bind();
    void unbind();
};

#endif