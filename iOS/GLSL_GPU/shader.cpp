/*
 *  shader.cpp
 *  GLSL Demo
 *
 *  Created by Chema on 12/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <GLUT/glut.h>
#include <fstream>
#include "shader.h"


void Shader::retrieveShaderLog(std::ostream& os, GLuint obj)
{
	int cbInfoLog = 0;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &cbInfoLog);
	
	if ( cbInfoLog > 0 )
	{
		char*	pbuffInfoLog = new char[cbInfoLog];
		int		cbInfoRead  = 0;
		glGetShaderInfoLog(obj, cbInfoLog, &cbInfoRead, pbuffInfoLog);

		os << "ShaderInfoLog: " << pbuffInfoLog << "\n";

		delete [] pbuffInfoLog;
	}
	else
	{
		printf("Shader Info Log: OK\n");
	}
}


/* =====================================================================================================================
 * ---------------------------------------------------------------------------------------------------------------------
 */

void Shader::retrieveProgramLog(std::ostream& os, GLuint obj)
{
	int		cbInfoLog = 0;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &cbInfoLog);
	
	if ( cbInfoLog > 0 )
	{
		char*	pbuffInfoLog = new char[cbInfoLog];
		int		cbInfoRead  = 0;
		glGetProgramInfoLog(obj, cbInfoLog, &cbInfoRead, pbuffInfoLog);

		os << "ProgramLog: " << pbuffInfoLog << "\n";

		delete [] pbuffInfoLog;
	}
	else
	{
		os << "Program Info Log: OK\n";
	}
}


std::string Shader::readShader(const char* pszFilename)
{
	using std::getline;
	using std::ifstream;
	using std::string;
	
	string		str, strFile;
	ifstream	ifs(pszFilename);
	if ( ifs.is_open() )
	{
		while ( getline(ifs, str) )	{
			strFile += str;
		}
	}
    //std::cout<<strFile;
	
	return strFile;
}


void Shader::setup(const std::string& vxshader, const std::string& fragshader )
{
	using std::cout;
	using std::string;
	
	g_vertex_shader		= glCreateShader(GL_VERTEX_SHADER);
	g_fragment_shader	= glCreateShader(GL_FRAGMENT_SHADER);
	g_geometry_shader	= glCreateShader(GL_GEOMETRY_SHADER_EXT);

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


	g_program = glCreateProgram();

	glAttachShader(g_program, g_fragment_shader);
	glAttachShader(g_program, g_vertex_shader);
	glLinkProgram(g_program);

    cout<<vxshader<<std::endl;
	retrieveShaderLog(cout, g_vertex_shader);
    cout<<fragshader<<std::endl;
	retrieveShaderLog(cout, g_fragment_shader);
	retrieveProgramLog(cout, g_program);
    printActiveAttribs();
    printActiveUniforms();
}

void Shader::printActiveAttribs() {
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

void Shader::printActiveUniforms() {
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

Shader::~Shader() {
	glDetachShader(g_program, g_fragment_shader);
	glDetachShader(g_program, g_vertex_shader);
    
	glDeleteShader(g_fragment_shader);
	glDeleteShader(g_vertex_shader);
	glDeleteProgram(g_program);
}

unsigned int Shader::getId() {
	return g_program;
}

void Shader::bind() {
	glUseProgram(g_program);
}

void Shader::unbind() {
	glUseProgram(0);
}