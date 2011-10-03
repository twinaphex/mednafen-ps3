#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "glslProgram.h"

namespace						LibESGL
{
	struct						ProgramPrivate
	{
		GLuint					ProgramID;
		GLuint					VertexShader;
		GLuint					FragmentShader;
	};

	void printShaderInfoLog(GLuint obj)
	{
	    int infologLength = 0;
	    int charsWritten  = 0;
	    char *infoLog;

		glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	    if (infologLength > 0)
	    {
	        infoLog = (char *)malloc(infologLength);
	        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("%s\n",infoLog);
	        free(infoLog);
	    }
	}

	void printProgramInfoLog(GLuint obj)
	{
	    int infologLength = 0;
	    int charsWritten  = 0;
	    char *infoLog;

		glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	    if (infologLength > 0)
	    {
	        infoLog = (char *)malloc(infologLength);
	        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("%s\n",infoLog);
	        free(infoLog);
	    }
	}
}

								LibESGL::Program::Program			(const char* aVertex, const char* aFragment, bool aVertexFile, bool aFragmentFile) :
	Private(new ProgramPrivate)
{
	Private->ProgramID = glCreateProgram();

	Private->VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Private->VertexShader, 1, &aVertex, 0);
	glCompileShader(Private->VertexShader);
	glAttachShader(Private->ProgramID, Private->VertexShader);
	printShaderInfoLog(Private->VertexShader);

	Private->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Private->FragmentShader, 1, &aFragment, 0);
	glCompileShader(Private->FragmentShader);
	glAttachShader(Private->ProgramID, Private->FragmentShader);
	printShaderInfoLog(Private->FragmentShader);

	glLinkProgram(Private->ProgramID);
	printProgramInfoLog(Private->ProgramID);
}

								LibESGL::Program::~Program			()
{
	glDeleteShader(Private->VertexShader);
	glDeleteShader(Private->FragmentShader);
	glDeleteProgram(Private->ProgramID);

	delete Private;
}

void							LibESGL::Program::Use				()
{
	glUseProgram(Private->ProgramID);
}

LibESGL::Program::TokenID		LibESGL::Program::ObtainToken		(const char* aName, bool aFragment)
{
	return glGetUniformLocation(Private->ProgramID, aName);
}


