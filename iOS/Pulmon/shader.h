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
#include "glUtil.h"

struct Shader {
private:
    GLuint	g_vertex_shader;
    GLuint	g_fragment_shader;
    GLuint	g_geometry_shader;
    GLuint	g_program;

public:
    ~Shader();

    unsigned int getId() const;

    void compile(const std::string& shader);
    void compile(const std::string& vxshader, const std::string& fragshader);
    void link();
    
    void printShaderLog(std::ostream& os, GLuint obj) const;
    void printProgramLog(std::ostream& os, GLuint obj) const;
    void printActiveAttribs() const;
    void printActiveUniforms() const;
    void bind() const;
    void unbind() const;
};

#endif