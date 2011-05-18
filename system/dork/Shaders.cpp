#include <es_system.h>

namespace
{
	inline void						SetVertex							(GLfloat* aBase, float aX, float aY, float aU, float aV)
	{
		*aBase++ = aX; *aBase++ = aY; *aBase++ = 0.0f; *aBase++ = aU; *aBase++ = aV;
	}

	void							MakeVertexRectangle					(GLfloat* aBuffer, uint32_t aVerticalFlip, float aLeft, float aRight, float aTop, float aBottom)
	{
		SetVertex(aBuffer + 0,  0.0f,	0.0f,	aLeft, aVerticalFlip ? aBottom : aTop);
		SetVertex(aBuffer + 5,  1.0f,	0.0f,	aRight, aVerticalFlip ? aBottom : aTop);
		SetVertex(aBuffer + 10, 1.0f,	1.0f,	aRight, aVerticalFlip ? aTop : aBottom);
		SetVertex(aBuffer + 15, 0.0f,	1.0f,	aLeft, aVerticalFlip ? aTop : aBottom);
	}
}

ShaderMap DorkShaderProgram::Shaders;

									DorkShaderProgram::DorkShaderProgram	(CGcontext& aContext, const std::string& aFileName) :
	Context(aContext),
	VertexProgram(0),
	FragmentProgram(0),
	Projection(0),
	FragmentVideoSize(0),
	FragmentTextureSize(0),
	FragmentOutputSize(0),
	VertexVideoSize(0),
	VertexTextureSize(0),
	VertexOutputSize(0)	
{
	static const char* args[] = { "-fastmath", "-unroll=all", "-ifcvt=all", 0 };

	VertexProgram = cgCreateProgramFromFile(Context, CG_SOURCE, aFileName.c_str(), CG_PROFILE_SCE_VP_RSX, "main_vertex", args);
	FragmentProgram = cgCreateProgramFromFile(Context, CG_SOURCE, aFileName.c_str(), CG_PROFILE_SCE_FP_RSX, "main_fragment", args);

	cgGLEnableProfile(CG_PROFILE_SCE_VP_RSX);
	cgGLEnableProfile(CG_PROFILE_SCE_FP_RSX);

	cgGLBindProgram(VertexProgram);
	cgGLBindProgram(FragmentProgram);

	Projection = cgGetNamedParameter(VertexProgram, "modelViewProj");
	FragmentVideoSize = cgGetNamedParameter(FragmentProgram, "IN.video_size");
	FragmentTextureSize = cgGetNamedParameter(FragmentProgram, "IN.texture_size");
	FragmentOutputSize = cgGetNamedParameter(FragmentProgram, "IN.output_size");
	VertexVideoSize = cgGetNamedParameter(VertexProgram, "IN.video_size");
	VertexTextureSize = cgGetNamedParameter(VertexProgram, "IN.texture_size");
	VertexOutputSize = cgGetNamedParameter(VertexProgram, "IN.output_size");

	cgGLDisableProfile(CG_PROFILE_SCE_VP_RSX);
	cgGLDisableProfile(CG_PROFILE_SCE_FP_RSX);
}

void								DorkShaderProgram::Apply			(uint32_t aInWidth, uint32_t aInHeight, uint32_t aOutWidth, uint32_t aOutHeight)
{
	if(FragmentProgram && VertexProgram)
	{
		cgGLEnableProfile(CG_PROFILE_SCE_VP_RSX);
		cgGLEnableProfile(CG_PROFILE_SCE_FP_RSX);

		cgGLBindProgram(VertexProgram);
		cgGLBindProgram(FragmentProgram);

		/* Update shader params */
		if(Projection)			cgGLSetStateMatrixParameter(Projection, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
		if(FragmentVideoSize)	cgGLSetParameter2f(FragmentVideoSize, aInWidth, aInHeight);
		if(FragmentTextureSize)	cgGLSetParameter2f(FragmentTextureSize, aInWidth, aInHeight);
		if(FragmentOutputSize)	cgGLSetParameter2f(FragmentOutputSize, aOutWidth, aOutHeight);
		if(VertexVideoSize)		cgGLSetParameter2f(VertexVideoSize, aInWidth, aInHeight);
		if(VertexTextureSize)	cgGLSetParameter2f(VertexTextureSize, aInWidth, aInHeight);
		if(VertexOutputSize)	cgGLSetParameter2f(VertexOutputSize, aOutWidth, aOutHeight);
	}
}

DorkShaderProgram*					DorkShaderProgram::Get				(CGcontext& aContext, const std::string& aFileName)
{
	if(Shaders.find(aFileName) == Shaders.end())
	{
		Shaders[aFileName] = new DorkShaderProgram(aContext, aFileName);
	}

	return Shaders[aFileName];
}


									DorkShader::DorkShader				(CGcontext& aContext, const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor) :
	Context(aContext),
	Next(0),
	Program(DorkShaderProgram::Get(aContext, aFileName)),
	Output(0, 0, 0, 0),
	InWidth(0),
	InHeight(0),
	TextureID(0),
	FrameBufferID(0),
	ScaleFactor(aScaleFactor),
	Smooth(aSmooth)
{
	Viewport[0] = 0.0f;
	Viewport[1] = 1.0f;
	Viewport[2] = 0.0f;
	Viewport[3] = 1.0f;

	glGenTextures(1, &TextureID);
	glGenFramebuffersOES(1, &FrameBufferID);
}

									DorkShader::~DorkShader				()
{
	//Eat children (ungrateful bastards)
	if(Next)
	{
		delete Next;
	}

	glDeleteTextures(1, &TextureID);
	glDeleteFramebuffersOES(1, &FrameBufferID);
}

void								DorkShader::Present					(GLuint aSourceTexture)
{
	if(Next)
	{
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, FrameBufferID);
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, TextureID, 0);
	}

	//Update projection
	glViewport(Output.X, Output.Y, Output.Width, Output.Height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrthof(0, 1, 1, 0, -1, 1);


	//Apply state buffer
	Apply();
	DorkVideo::ApplyVertexBuffer(VertexBuffer, false);

	//Prep texture
	glBindTexture(GL_TEXTURE_2D, aSourceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Smooth ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Smooth ? GL_LINEAR : GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//Draw
	glDrawArrays(GL_QUADS, 0, 4);

	//Clean up projection
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	if(Next)
	{
		//Call next shader
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
		Next->Present(TextureID);
	}
	else
	{
		//Restore viewport
		glViewport(0, 0, es_video->GetScreenWidth(), es_video->GetScreenHeight());
	}
}

void								DorkShader::Apply					()
{
	Program->Apply(InWidth, InHeight, Output.Width, Output.Height);
}

void								DorkShader::SetViewport				(float aLeft, float aRight, float aTop, float aBottom)
{
	Viewport[0] = aLeft;
	Viewport[1] = aRight;
	Viewport[2] = aTop;
	Viewport[3] = aBottom;
	MakeVertexRectangle(VertexBuffer, Next ? 1 : 0, Viewport[0], Viewport[1], Viewport[2], Viewport[3]);
}

void								DorkShader::Set						(const Area& aOutput, uint32_t aInWidth, uint32_t aInHeight)
{
	/* Copy settings */
	Output = aOutput;
	InWidth = aInWidth;
	InHeight = aInHeight;

	/* Update smoothing */
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* Update texture */
	if(Next)
	{
		Output = Area(0, 0, aInWidth * ScaleFactor, aInHeight * ScaleFactor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ARGB_SCE, Output.Width, Output.Height, 0, GL_ARGB_SCE, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
	}
	else
	{
		/* Delete the texture ? */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ARGB_SCE, 1, 1, 0, GL_ARGB_SCE, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
	}

	/* Update vertex buffer */
	MakeVertexRectangle(VertexBuffer, Next ? 1 : 0, Viewport[0], Viewport[1], Viewport[2], Viewport[3]);

	if(Next)
	{
		Next->Set(aOutput, aInWidth * ScaleFactor, aInHeight * ScaleFactor);
	}
}

//
#include "SimpleIni.h"

DorkShader*							DorkShader::MakeChainFromPreset		(CGcontext& aContext, const std::string& aFile)
{
	CSimpleIniA ini;
	ini.LoadFile(aFile.c_str());

	DorkShader* output = new DorkShader(aContext, ini.GetValue("PS3General", "PS3CurrentShader", ""), ini.GetLongValue("PS3General", "Smooth", 0), ini.GetLongValue("PS3General", "ScaleFactor", 1));
	output->SetNext(new DorkShader(aContext, ini.GetValue("PS3General", "PS3CurrentShader2", ""), ini.GetLongValue("PS3General", "Smooth2", 0), 1));

	return output;
}


