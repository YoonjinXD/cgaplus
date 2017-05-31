

#pragma once

#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>


class Shader
{
public:
	GLuint program;
	GLuint vaoHandle;
	GLuint vboHandles[2];

public:
	Shader(void);
	~Shader(void);

public:
	char* readShaderSource(const char* shaderFile);
	void InitShader(const char* vShaderFile, const char* fShaderFile);
	inline GLuint GetProgram(){return program;};
};

