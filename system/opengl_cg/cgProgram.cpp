#define GL_GLEXT_PROTOTYPES

#ifndef __CELLOS_LV2__
# include	<GL/gl.h>
# include	<Cg/cg.h>
# include	<Cg/cgGL.h>
# define	FRAG_PROFILE				cgGLGetLatestProfile(CG_GL_FRAGMENT)
# define	VERT_PROFILE				cgGLGetLatestProfile(CG_GL_VERTEX)
#else
# include	<PSGL/psgl.h>
# define	FRAG_PROFILE				CG_PROFILE_SCE_FP_RSX
# define	VERT_PROFILE				CG_PROFILE_SCE_VP_RSX
#endif

#include <stdio.h>
#include <stdlib.h>
#include "cgProgram.h"

namespace						LibESGL
{
	CGcontext					Context;

	struct						ProgramPrivate
	{
		CGprogram				VertexProgram;
		CGprogram				FragmentProgram;
	};

	void						CgErrorCallback						(void)
	{
		printf("Cg error: %s\n", cgGetErrorString(cgGetError()));
	}
}

								LibESGL::Program::Program			(const char* aVertex, const char* aFragment, bool aVertexFile, bool aFragmentFile) :
	Private(new ProgramPrivate)
{
	cgSetErrorCallback(CgErrorCallback);

	if(!Context)
	{
		Context = cgCreateContext();
	}

#ifndef __CELLOS_LV2__
	cgGLSetOptimalOptions(cgGLGetLatestProfile(CG_GL_VERTEX));
	cgGLSetOptimalOptions(cgGLGetLatestProfile(CG_GL_FRAGMENT));
	static const char** args = 0;
#else
	static const char* args[] = { "-fastmath", "-unroll=all", "-ifcvt=all", 0 };		
#endif

	if(aVertexFile)
	{
		Private->VertexProgram = cgCreateProgramFromFile(Context, CG_SOURCE, aVertex, VERT_PROFILE, "main_vertex", args);
	}
	else
	{
		Private->VertexProgram = cgCreateProgram(Context, CG_SOURCE, aVertex, VERT_PROFILE, "main_vertex", args);
	}

	if(aFragmentFile)
	{
		Private->FragmentProgram = cgCreateProgramFromFile(Context, CG_SOURCE, aFragment, FRAG_PROFILE, "main_fragment", args);
	}
	else
	{
		Private->FragmentProgram = cgCreateProgram(Context, CG_SOURCE, aFragment, FRAG_PROFILE, "main_fragment", args);
	}

	cgGLLoadProgram(Private->VertexProgram);
	cgGLLoadProgram(Private->FragmentProgram);
}

								LibESGL::Program::~Program			()
{
	delete Private;
}

void							LibESGL::Program::Revert			()
{
	cgGLDisableProfile(VERT_PROFILE);
	cgGLDisableProfile(FRAG_PROFILE);
}

void							LibESGL::Program::Use				()
{
	cgGLEnableProfile(VERT_PROFILE);
	cgGLEnableProfile(FRAG_PROFILE);

	cgGLBindProgram(Private->VertexProgram);
	cgGLBindProgram(Private->FragmentProgram);
}

LibESGL::Program::TokenID		LibESGL::Program::ObtainToken		(const char* aName, bool aFragment)
{
	if(!aFragment)
	{
		CGparameter param = cgGetNamedParameter(Private->VertexProgram, aName);
		return param ? param : cgGetNamedParameter(Private->FragmentProgram, aName);
	}
	else
	{
		CGparameter param = cgGetNamedParameter(Private->FragmentProgram, aName);
		return param ? param : cgGetNamedParameter(Private->VertexProgram, aName);
	}
}


