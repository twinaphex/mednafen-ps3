#include <es_system.h>
#include "CgProgram.h"

//Ugly hack to support both normal and PS3 modes in one file.
#ifndef __CELLOS_LV2__
# include	<Cg/cg.h>
# include	<Cg/cgGL.h>
# define	FRAG_PROFILE				cgGLGetLatestProfile(CG_GL_FRAGMENT)
# define	VERT_PROFILE				cgGLGetLatestProfile(CG_GL_VERTEX)
#else
# define	FRAG_PROFILE				CG_PROFILE_SCE_FP_RSX
# define	VERT_PROFILE				CG_PROFILE_SCE_VP_RSX
#endif

namespace
{
	//TODO: Delete this when done!
	std::map<std::string, CgProgram*>	ShaderCache;
}


										CgProgram::CgProgram			(CGcontext& aContext, const std::string& aFileName) :
	Context(aContext),
	VertexProgram(0),
	FragmentProgram(0),
	Projection(0),
	FragmentVideoSize(0),
	FragmentTextureSize(0),
	FragmentOutputSize(0),
	FragmentFrameCount(0),
	VertexVideoSize(0),
	VertexTextureSize(0),
	VertexOutputSize(0),
	VertexFrameCount(0)
{
	//Setup is different on PS3
#ifndef __CELLOS_LV2__
	cgGLSetOptimalOptions(cgGLGetLatestProfile(CG_GL_VERTEX));
	cgGLSetOptimalOptions(cgGLGetLatestProfile(CG_GL_FRAGMENT));
	static const char** args = 0;
#else
	static const char* args[] = { "-fastmath", "-unroll=all", "-ifcvt=all", 0 };		
#endif

	//Create the programs
	if(!aFileName.empty() && Utility::FileExists(aFileName))
	{
		VertexProgram = cgCreateProgramFromFile(Context, CG_SOURCE, aFileName.c_str(), VERT_PROFILE, "main_vertex", args);
		FragmentProgram = cgCreateProgramFromFile(Context, CG_SOURCE, aFileName.c_str(), FRAG_PROFILE, "main_fragment", args);
	}

	//Fetch the argument ids from the loaded programs
	if(Valid())
	{
		cgGLLoadProgram(VertexProgram);
		cgGLLoadProgram(FragmentProgram);

		cgGLEnableProfile(VERT_PROFILE);
		cgGLEnableProfile(FRAG_PROFILE);

		cgGLBindProgram(VertexProgram);
		cgGLBindProgram(FragmentProgram);

		Projection = cgGetNamedParameter(VertexProgram, "modelViewProj");
		FragmentVideoSize = cgGetNamedParameter(FragmentProgram, "IN.video_size");
		FragmentTextureSize = cgGetNamedParameter(FragmentProgram, "IN.texture_size");
		FragmentOutputSize = cgGetNamedParameter(FragmentProgram, "IN.output_size");
		FragmentFrameCount = cgGetNamedParameter(FragmentProgram, "IN.frame_count");
		VertexVideoSize = cgGetNamedParameter(VertexProgram, "IN.video_size");
		VertexTextureSize = cgGetNamedParameter(VertexProgram, "IN.texture_size");
		VertexOutputSize = cgGetNamedParameter(VertexProgram, "IN.output_size");
		VertexFrameCount = cgGetNamedParameter(VertexProgram, "IN.frame_count");

		cgGLDisableProfile(VERT_PROFILE);
		cgGLDisableProfile(FRAG_PROFILE);
	}
}

void									CgProgram::Apply				(uint32_t aInWidth, uint32_t aInHeight, uint32_t aTextureWidth, uint32_t aTextureHeight, uint32_t aOutWidth, uint32_t aOutHeight, uint32_t aFrameCount)
{
	if(Valid())
	{
		cgGLEnableProfile(VERT_PROFILE);
		cgGLEnableProfile(FRAG_PROFILE);

		cgGLBindProgram(VertexProgram);
		cgGLBindProgram(FragmentProgram);

		//Set shader arguments
		if(Projection)			cgGLSetStateMatrixParameter(Projection, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
		if(FragmentVideoSize)	cgGLSetParameter2f(FragmentVideoSize, aInWidth, aInHeight);
		if(FragmentTextureSize)	cgGLSetParameter2f(FragmentTextureSize, aTextureWidth, aTextureHeight);
		if(FragmentOutputSize)	cgGLSetParameter2f(FragmentOutputSize, aOutWidth, aOutHeight);
		if(FragmentFrameCount)	cgGLSetParameter1f(FragmentFrameCount, aFrameCount);
		if(VertexVideoSize)		cgGLSetParameter2f(VertexVideoSize, aInWidth, aInHeight);
		if(VertexTextureSize)	cgGLSetParameter2f(VertexTextureSize, aTextureWidth, aTextureHeight);
		if(VertexOutputSize)	cgGLSetParameter2f(VertexOutputSize, aOutWidth, aOutHeight);
		if(VertexFrameCount)	cgGLSetParameter1f(VertexFrameCount, aFrameCount);
	}
	else
	{
		Unapply();
	}
}

void									CgProgram::Unapply				()
{
	cgGLDisableProfile(VERT_PROFILE);
	cgGLDisableProfile(FRAG_PROFILE);
}

bool									CgProgram::Valid				()
{
	return (VertexProgram && FragmentProgram) ? true : false;
}

CgProgram*								CgProgram::Get					(CGcontext& aContext, const std::string& aFileName)
{
	if(ShaderCache.find(aFileName) == ShaderCache.end())
	{
		ShaderCache[aFileName] = new CgProgram(aContext, aFileName);
	}

	return ShaderCache[aFileName];
}


