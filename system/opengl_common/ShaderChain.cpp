#include <es_system.h>
#include "FrameBuffer.h"
#include "ShaderChain.h"

#include "../opengl_cg/cgProgram.h"
#include <Cg/cgGL.h>

#if 0
#define glSplat() {uint32_t i = glGetError(); if(i) {printf("%X\n", i); abort();}}
#else
#define glSplat()
#endif

namespace
{
	void							MakeVertexRectangle					(LibESGL::Vertex* aBuffer, uint32_t aVerticalFlip, float aLeft, float aRight, float aTop, float aBottom)
	{
		aBuffer[0].Set(-1.0f,	-1.0f,	aLeft, aVerticalFlip ? aBottom : aTop, 0xFFFFFFFF);
		aBuffer[1].Set(1.0f,	-1.0f,	aRight, aVerticalFlip ? aBottom : aTop, 0xFFFFFFFF);
		aBuffer[2].Set(1.0f,	1.0f,	aRight, aVerticalFlip ? aTop : aBottom, 0xFFFFFFFF);
		aBuffer[3].Set(-1.0f,	1.0f,	aLeft, aVerticalFlip ? aTop : aBottom, 0xFFFFFFFF);
	}

	void							ApplyVertexBuffer					(LibESGL::Vertex* aBuffer, bool aBorder)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, LibESGL::Vertex::Size(), &aBuffer[0].X);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, LibESGL::Vertex::Size(), &aBuffer[0].U);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, LibESGL::Vertex::Size(), &aBuffer[0].R);

		if(aBorder)
		{
			glClientActiveTexture(GL_TEXTURE1);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			static const float texCoordS[] = {0, 0, 1, 0, 1, 1, 0, 1};
			glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(float), texCoordS);
			glClientActiveTexture(GL_TEXTURE0);
		}
		else
		{
			glClientActiveTexture(GL_TEXTURE1);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glClientActiveTexture(GL_TEXTURE0);
		}
	}
}


									LibESGL::ShaderChain::ShaderChain			(const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor) :
	Next(0),
	ShaderProgram(new Program(aFileName.c_str(), aFileName.c_str(), true, true)),
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

									LibESGL::ShaderChain::~ShaderChain			()
{
	//Eat children (ungrateful bastards)
	if(Next)
	{
		delete Next;
	}


	delete RenderTarget;
}

void								LibESGL::ShaderChain::Present				(GLuint aSourceTexture, GLuint aBorderTexture)
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

	//Apply state buffer
	ApplyVertexBuffer(VertexBuffer, ((Next == 0) && aBorderTexture) ? 1 : 0);
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

void								LibESGL::ShaderChain::Apply					()
{
	ShaderProgram->Use();
	
	cgGLSetStateMatrixParameter((CGparameter)ShaderProgram->ObtainToken("modelViewProj"), CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetParameter2f((CGparameter)ShaderProgram->ObtainToken("IN.video_size", true), InWidth, InHeight);
	cgGLSetParameter2f((CGparameter)ShaderProgram->ObtainToken("IN.texture_size", true), TextureWidth, TextureHeight);
	cgGLSetParameter2f((CGparameter)ShaderProgram->ObtainToken("IN.output_size", true), Output.Width, Output.Height);
	cgGLSetParameter2f((CGparameter)ShaderProgram->ObtainToken("IN.video_size", false), InWidth, InHeight);
	cgGLSetParameter2f((CGparameter)ShaderProgram->ObtainToken("IN.texture_size", false), TextureWidth, TextureHeight);
	cgGLSetParameter2f((CGparameter)ShaderProgram->ObtainToken("IN.output_size", false), Output.Width, Output.Height);
}

void								LibESGL::ShaderChain::SetViewport			(float aLeft, float aRight, float aTop, float aBottom)
{
	Viewport[0] = aLeft;
	Viewport[1] = aRight;
	Viewport[2] = aTop;
	Viewport[3] = aBottom;
	MakeVertexRectangle(VertexBuffer, Next ? 0 : 1, Viewport[0], Viewport[1], Viewport[2], Viewport[3]);
}

void								LibESGL::ShaderChain::Set					(const Area& aOutput, uint32_t aInWidth, uint32_t aInHeight, uint32_t aTextureWidth, uint32_t aTextureHeight)
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
		MakeVertexRectangle(VertexBuffer, Next ? 0 : 1, Viewport[0], Viewport[1], Viewport[2], Viewport[3]);
	}

	if(Next)
	{
		Next->Set(aOutput, aInWidth * ScaleFactor, aInHeight * ScaleFactor, Output.Width, Output.Height);
	}
}

//
#include "src/thirdparty/simpleini/SimpleIni.h"

LibESGL::ShaderChain*				LibESGL::ShaderChain::MakeChainFromPreset	(const std::string& aFile, uint32_t aPrescale)
{
	if(!aFile.empty() && Utility::FileExists(aFile))
	{
		CSimpleIniA ini;
		ini.LoadFile(aFile.c_str());

		std::string shader1 = LibES::BuildPath(std::string("assets/shaders/") + ini.GetValue("PS3General", "PS3CurrentShader", ""));
		std::string shader2 = LibES::BuildPath(std::string("assets/shaders/") + ini.GetValue("PS3General", "PS3CurrentShader2", ""));

		ShaderChain* output = new ShaderChain(shader1, ini.GetLongValue("PS3General", "Smooth", 0), ini.GetLongValue("PS3General", "ScaleFactor", 1));
		output->AttachNext(new ShaderChain(shader2, ini.GetLongValue("PS3General", "Smooth2", 0), 1));
		return output;
	}
	else
	{
		return new ShaderChain(LibES::BuildPath("assets/shaders/stock.cg").c_str(), 0, 1);
	}
}

