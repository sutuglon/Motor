/*
 *  shader.cpp
 *  GLSL Demo
 *
 *  Created by Chema on 12/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <iostream>
#include <sstream>
#include <fstream>
#include "bundle.h"
#include "shader.h"


namespace  {
    
    std::string readShader(const char* pszFilename)
    {
        using std::getline;
        using std::ifstream;
        
        using std::string;
        std::ostringstream ostr;
        
        string		str, strFile;
        ifstream	ifs(pszFilename);
        if ( ifs.is_open() )
        {
            while(getline(ifs, str)) {
                ostr << str << std::endl;
                //std::cout<<ostr.str();
            }
            
        }
        return ostr.str();
    }

}


void Shader::compile(const std::string& shader) {
    compile(GetBundleResourcePath(shader+".vsh"),GetBundleResourcePath(shader+".fsh"));
}

void Shader::compile(const std::string& vxshader, const std::string& fragshader )
{
	using std::string;
    g_program = glCreateProgram();
	g_vertex_shader	= glCreateShader(GL_VERTEX_SHADER);
	g_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //	g_geometry_shader	= glCreateShader(GL_GEOMETRY_SHADER_EXT);
    
	string	fs			= readShader(fragshader.c_str());
	const char*	pfs		= fs.c_str();
	glShaderSource(g_fragment_shader, 1, &pfs, NULL);
	glCompileShader(g_fragment_shader);
    //cout<<"fs source"<<std::endl<<fs.c_str();
    
	string	vs			= readShader(vxshader.c_str());
	const char*	pvs		= vs.c_str();
	glShaderSource(g_vertex_shader, 1, &pvs, NULL);
	glCompileShader(g_vertex_shader);
    //cout<<"vs source"<<std::endl<<vs.c_str();
    
    std::cout<<vxshader<<std::endl;
	printShaderLog(std::cout, g_vertex_shader);
    std::cout<<fragshader<<std::endl;
	printShaderLog(std::cout, g_fragment_shader);
    
}

void Shader::link() {
    
	glAttachShader(g_program, g_fragment_shader);
	glAttachShader(g_program, g_vertex_shader);
	glLinkProgram(g_program);
    
	printProgramLog(std::cout, g_program);
    printActiveAttribs();
    printActiveUniforms();
}

unsigned int Shader::getId() const{
	return g_program;
}

void Shader::bind() const {
	glUseProgram(g_program);
}

void Shader::unbind() const {
	glUseProgram(0);
}

Shader::~Shader() {
	glDetachShader(g_program, g_fragment_shader);
	glDetachShader(g_program, g_vertex_shader);
    
	glDeleteShader(g_fragment_shader);
	glDeleteShader(g_vertex_shader);
	glDeleteProgram(g_program);
}


void Shader::printShaderLog(std::ostream& os, GLuint obj) const
{
	int cbInfoLog = 0, status = 0;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &cbInfoLog);
	if ( cbInfoLog > 0 ) {
		char*	pbuffInfoLog = new char[cbInfoLog];
		glGetShaderInfoLog(obj, cbInfoLog, &cbInfoLog, pbuffInfoLog);
		os << "ShaderInfoLog: " << pbuffInfoLog << std::endl;
		delete [] pbuffInfoLog;
	}
	else {	
    	printf("Shader Info Log: OK\n");
	}
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (status == 0) {
		os << "Failed to compile shader:"<< std::endl;
	}
}


void Shader::printProgramLog(std::ostream& os, GLuint obj) const
{
	int		cbInfoLog = 0;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &cbInfoLog);
	if ( cbInfoLog > 0 ) {
		char*	pbuffInfoLog = new char[cbInfoLog];
		int		cbInfoRead  = 0;
		glGetProgramInfoLog(obj, cbInfoLog, &cbInfoRead, pbuffInfoLog);
		os << "ProgramLog: " << pbuffInfoLog << "\n";
		delete [] pbuffInfoLog;
	}
	else {
		os << "Program Info Log: OK\n";
	}
}


void Shader::printActiveAttribs() const {
    char *name;
    GLint activeSize, nameLength;

    glGetProgramiv(g_program, GL_ACTIVE_ATTRIBUTES, &activeSize);
    glGetProgramiv(g_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLength);

    name = (char*) malloc(nameLength + 1);

    printf("Active attributes:\n");
    if (activeSize==0) {
        printf("none\n");
    }
    for (unsigned i = 0; i < activeSize; i++) {
        GLint size;
        GLenum type;

        glGetActiveAttrib(g_program, i, nameLength + 1, NULL, &size, &type, name);
        int attribloc =  glGetAttribLocation(g_program, name);
        printf("%d %d %s\n", type, attribloc, name );
    }
    free(name);
}

void Shader::printActiveUniforms() const {
    char *name;
    GLint activeSize, nameLength;
    
    glGetProgramiv(g_program, GL_ACTIVE_UNIFORMS, &activeSize);
    glGetProgramiv(g_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameLength);
    
    name = (char*) malloc(nameLength + 1);

    printf("Active uniforms:\n");
    if (activeSize==0) {
        printf("none\n");
    }
    for (unsigned i = 0; i < activeSize; i++) {
        GLint size;
        GLenum type;

        glGetActiveUniform(g_program, i, nameLength + 1, NULL, &size, &type, name);
        int loc =  glGetUniformLocation(g_program, name);
        printf("%d %d %s\n", type, loc, name );
    }

    free(name);
}
