#include <es_system.h>
#include "FrameBuffer.h"
#include "ShaderChain.h"

#include "../opengl_cg/cgProgram.h"
#include <Cg/cgGL.h>

#ifdef ES_OPENGLES
# define glOrtho					glOrthof
#endif

#if 0
#define glSplat() {uint32_t i = glGetError(); if(i) {printf("%X\n", i); abort();}}
#else
#define glSplat()
#endif

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


									GLShader::GLShader					(const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor) :
	Next(0),
	Program(new LibESGL::Program(aFileName.c_str(), aFileName.c_str(), true, true)),
	Output(0, 0, 0, 0),
	InWidth(0),
	InHeight(0),
	TextureWidth(0),
	TextureHeight(0),
	FrameCount(0),
	RenderTarget(0),
	ScaleFactor(aScaleFactor),
	Smooth(aSmooth)
{
	Viewport[0] = 0.0f;
	Viewport[1] = 1.0f;
	Viewport[2] = 0.0f;
	Viewport[3] = 1.0f;
}

									GLShader::~GLShader					()
{
	//Eat children (ungrateful bastards)
	if(Next)
	{
		delete Next;
	}


	delete RenderTarget;
}

void								GLShader::Present					(GLuint aSourceTexture, GLuint aBorderTexture)
{
	if(Next)
	{
		ESVideo::SetRenderTarget(RenderTarget);
	}
	else if(aBorderTexture)
	{
		glActiveTexture(GL_TEXTURE1); glSplat();
		glEnable(GL_TEXTURE_2D); glSplat();
		glBindTexture(GL_TEXTURE_2D, aBorderTexture); glSplat();
		glActiveTexture(GL_TEXTURE0); glSplat();
	}


	//Update projection
	glViewport(Output.X, Output.Y, Output.Width, Output.Height); glSplat();
	glMatrixMode(GL_PROJECTION); glSplat();
	glPushMatrix(); glSplat();
	glLoadIdentity(); glSplat();
	glOrtho(0, 1, 1, 0, -1, 1); glSplat();

	//Apply state buffer
	GLShader::ApplyVertexBuffer(VertexBuffer, false, ((Next == 0) && aBorderTexture) ? 1 : 0);
	Apply();

	//Prep texture
	glBindTexture(GL_TEXTURE_2D, aSourceTexture); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Smooth ? GL_LINEAR : GL_NEAREST); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Smooth ? GL_LINEAR : GL_NEAREST);	glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); glSplat();

	//Draw
	glDrawArrays(GL_QUADS, 0, 4); glSplat();

	//Update count
	FrameCount ++;

	//Clean up projection
	glMatrixMode(GL_PROJECTION); glSplat();
	glPopMatrix(); glSplat();

	if(Next)
	{
		ESVideo::SetRenderTarget(0);

		//Call next shader
		Next->Present(RenderTarget->GetID(), aBorderTexture);
	}
	else
	{
		if(aBorderTexture)
		{
			glActiveTexture(GL_TEXTURE1); glSplat();
			glBindTexture(GL_TEXTURE_2D, 0); glSplat();
			glDisable(GL_TEXTURE_2D); glSplat();
			glActiveTexture(GL_TEXTURE0); glSplat();
		}

		//Restore viewport
		glViewport(0, 0, ESVideo::GetScreenWidth(), ESVideo::GetScreenHeight()); glSplat();
	}
}

void								GLShader::Apply						()
{
	Program->Use();
	
	cgGLSetStateMatrixParameter((CGparameter)Program->ObtainToken("modelViewProj"), CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetParameter2f((CGparameter)Program->ObtainToken("IN.video_size", true), InWidth, InHeight);
	cgGLSetParameter2f((CGparameter)Program->ObtainToken("IN.texture_size", true), TextureWidth, TextureHeight);
	cgGLSetParameter2f((CGparameter)Program->ObtainToken("IN.output_size", true), Output.Width, Output.Height);
	cgGLSetParameter2f((CGparameter)Program->ObtainToken("IN.video_size", false), InWidth, InHeight);
	cgGLSetParameter2f((CGparameter)Program->ObtainToken("IN.texture_size", false), TextureWidth, TextureHeight);
	cgGLSetParameter2f((CGparameter)Program->ObtainToken("IN.output_size", false), Output.Width, Output.Height);
}

void								GLShader::SetViewport				(float aLeft, float aRight, float aTop, float aBottom)
{
	Viewport[0] = aLeft;
	Viewport[1] = aRight;
	Viewport[2] = aTop;
	Viewport[3] = aBottom;
	MakeVertexRectangle(VertexBuffer, Next ? 1 : 0, Viewport[0], Viewport[1], Viewport[2], Viewport[3]);
}

void								GLShader::Set						(const Area& aOutput, uint32_t aInWidth, uint32_t aInHeight, uint32_t aTextureWidth, uint32_t aTextureHeight)
{
	/* Copy settings */
	if(Output != aOutput || InWidth != aInWidth || InHeight != aInHeight || TextureWidth != aTextureWidth || TextureHeight != aTextureHeight)
	{
		Output = aOutput;
		InWidth = aInWidth;
		InHeight = aInHeight;
		TextureWidth = aTextureWidth;
		TextureHeight = aTextureHeight;

		delete RenderTarget; RenderTarget = 0;

		/* Update texture */
		if(Next)
		{
			//TODO: Round size up to next power of two
			Output = Area(0, 0, aInWidth * ScaleFactor, aInHeight * ScaleFactor);
			RenderTarget = new FrameBuffer(Output.Width, Output.Height);
		}

		/* Update vertex buffer */
		MakeVertexRectangle(VertexBuffer, Next ? 1 : 0, Viewport[0], Viewport[1], Viewport[2], Viewport[3]);
	}

	if(Next)
	{
		Next->Set(aOutput, aInWidth * ScaleFactor, aInHeight * ScaleFactor, Output.Width, Output.Height);
	}
}

//
#include "src/thirdparty/simpleini/SimpleIni.h"

GLShader*							GLShader::MakeChainFromPreset		(const std::string& aFile, uint32_t aPrescale)
{
	if(!aFile.empty() && Utility::FileExists(aFile))
	{
		CSimpleIniA ini;
		ini.LoadFile(aFile.c_str());

		std::string shader1 = LibES::BuildPath(std::string("assets/shaders/") + ini.GetValue("PS3General", "PS3CurrentShader", ""));
		std::string shader2 = LibES::BuildPath(std::string("assets/shaders/") + ini.GetValue("PS3General", "PS3CurrentShader2", ""));

		GLShader* output = new GLShader(shader1, ini.GetLongValue("PS3General", "Smooth", 0), ini.GetLongValue("PS3General", "ScaleFactor", 1));
		output->AttachNext(new GLShader(shader2, ini.GetLongValue("PS3General", "Smooth2", 0), 1));
		return output;
	}
	else
	{
		return new GLShader("", 0, 1);
	}
}

