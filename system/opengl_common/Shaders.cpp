#include <es_system.h>
#include "FrameBuffer.h"
#include "Shaders.h"
#include "CgProgram.h"

#ifdef ES_OPENGLES
# define glOrtho					glOrthof
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


									GLShader::GLShader					(CGcontext& aContext, const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor) :
	Next(0),
	Program(CgProgram::Get(aContext, aFileName)),
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
	else if(Program->Valid() && aBorderTexture)
	{
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, aBorderTexture);
		glActiveTexture(GL_TEXTURE0);
	}


	//Update projection
	glViewport(Output.X, Output.Y, Output.Width, Output.Height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, -1, 1);

	//Apply state buffer
	GLShader::ApplyVertexBuffer(VertexBuffer, false, ((Next == 0) && aBorderTexture) ? 1 : 0);
	Apply();

	//Prep texture
	glBindTexture(GL_TEXTURE_2D, aSourceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Smooth ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Smooth ? GL_LINEAR : GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//Draw
	glDrawArrays(GL_QUADS, 0, 4);

	//Update count
	FrameCount ++;

	//Clean up projection
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	if(Next)
	{
		ESVideo::SetRenderTarget(0);

		//Call next shader
		Next->Present(RenderTarget->GetID(), aBorderTexture);
	}
	else
	{
		Program->Unapply();

		if(Program->Valid() && aBorderTexture)
		{
			glActiveTexture(GL_TEXTURE1);	
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);
		}

		//Restore viewport
		glViewport(0, 0, ESVideo::GetScreenWidth(), ESVideo::GetScreenHeight());
	}
}

void								GLShader::Apply						()
{
	Program->Apply(InWidth, InHeight, TextureWidth, TextureHeight, Output.Width, Output.Height, FrameCount);
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

GLShader*							GLShader::MakeChainFromPreset		(CGcontext& aContext, const std::string& aFile, uint32_t aPrescale)
{
	if(!aFile.empty() && Utility::FileExists(aFile))
	{
		CSimpleIniA ini;
		ini.LoadFile(aFile.c_str());

		std::string shader1 = es_paths->Build(std::string("assets/shaders/") + ini.GetValue("PS3General", "PS3CurrentShader", ""));
		std::string shader2 = es_paths->Build(std::string("assets/shaders/") + ini.GetValue("PS3General", "PS3CurrentShader2", ""));

		GLShader* output = new GLShader(aContext, shader1, ini.GetLongValue("PS3General", "Smooth", 0), ini.GetLongValue("PS3General", "ScaleFactor", 1));
		output->AttachNext(new GLShader(aContext, shader2, ini.GetLongValue("PS3General", "Smooth2", 0), 1));
		return output;
	}
	else
	{
		return new GLShader(aContext, "", 0, 1);
	}
}

