#include <es_system.h>

#include "opengl_common/FrameBuffer.h"

//Rename tokens for OpenGL ES Support
#ifdef ES_OPENGLES
# define	glGenFramebuffersEXT		glGenFramebuffersOES
# define	glDeleteFramebuffersEXT		glDeleteFramebuffersOES
# define	glBindFramebufferEXT		glBindFramebufferOES
# define	glFramebufferTexture2DEXT	glFramebufferTexture2DOES
# define	GL_FRAMEBUFFER_EXT			GL_FRAMEBUFFER_OES
# define	glOrtho						glOrthof
#endif

#if 0
#define glSplat() {uint32_t i = glGetError(); if(i) {printf("%X\n", i); abort();}}
#else
#define glSplat()
#endif

namespace
{
	//HACK:
	inline void							ApplyVertexBuffer	(GLfloat* aBuffer, bool aColors)
	{
		glEnableClientState(GL_VERTEX_ARRAY); glSplat();
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); glSplat();
		glVertexPointer(3, GL_FLOAT, (aColors ? 9 : 5) * sizeof(GLfloat), &aBuffer[0]); glSplat();
		glTexCoordPointer(2, GL_FLOAT, (aColors ? 9 : 5) * sizeof(GLfloat), &aBuffer[3]); glSplat();

		glClientActiveTexture(GL_TEXTURE1); glSplat();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY); glSplat();
		glClientActiveTexture(GL_TEXTURE0); glSplat();

		if(aColors)
		{
			glEnableClientState(GL_COLOR_ARRAY); glSplat();
			glColorPointer(4, GL_FLOAT, 9 * sizeof(GLfloat), &aBuffer[5]); glSplat();
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY); glSplat();
		}
	}
}


void					ESVideo::Initialize				()
{
	ESVideoPlatform::Initialize(ScreenWidth, ScreenHeight);

	//Some settings
	InitializeState();

	// Setup vertex buffer
	VertexBuffer = (GLfloat*)malloc(VertexBufferCount * VertexSize * sizeof(GLfloat));
	ApplyVertexBuffer(VertexBuffer, true);

	//Texture for FillRectangle
	FillerTexture = new Texture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Init framebuffer
	glGenFramebuffersEXT(1, &FrameBufferID); glSplat();
}

void					ESVideo::Shutdown				()
{
	SetRenderTarget(0);
	glDeleteFramebuffersEXT(1, &FrameBufferID); glSplat();

	delete FillerTexture;
	free(VertexBuffer);

	ESVideoPlatform::Shutdown();
}

void					ESVideo::SetScreenSize			(uint32_t aX, uint32_t aY)
{
	ScreenWidth = aX;
	ScreenHeight = aY;
	InitializeState();

	FontManager::InitFonts();
}

void					ESVideo::Flip					()
{
	ESVideoPlatform::Flip();
	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
	glClear(GL_COLOR_BUFFER_BIT); glSplat();
}

void					ESVideo::SetRenderTarget		(FrameBuffer* aBuffer)
{
	if(aBuffer)
	{
		glBindTexture(GL_TEXTURE_2D, aBuffer->GetID()); glSplat();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glSplat();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glSplat();
		glBindTexture(GL_TEXTURE_2D, 0); glSplat();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FrameBufferID); glSplat();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, aBuffer->GetID(), 0); glSplat();
	}
	else
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); glSplat();
		glViewport(0, 0, GetScreenWidth(), GetScreenHeight()); glSplat();
		ApplyVertexBuffer(VertexBuffer, true);
		glColor4f(0, 0, 0, 0); glSplat();
	}
}

void					ESVideo::PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor)
{
	float r = (float)((aColor >> 24) & 0xFF) / 256.0f;
	float g = (float)((aColor >> 16) & 0xFF) / 256.0f;	
	float b = (float)((aColor >> 8) & 0xFF) / 256.0f;
	float a = (float)((aColor >> 0) & 0xFF) / 256.0f;	

	float xl = ((float)aSource.X + .5f) / (float)aTexture->GetWidth();
	float xr = ((float)aSource.Right() - .5f) / (float)aTexture->GetWidth();
	float yl = ((float)aSource.Y + .5f) / (float)aTexture->GetHeight();
	float yr = ((float)aSource.Bottom() - .5f) / (float)aTexture->GetHeight();

	const Area& Clip = ESVideo::GetClip();
	aTexture->Apply();
	SetVertex(&VertexBuffer[0 * VertexSize], Clip.X + aDestination.X, Clip.Y + aDestination.Y, r, g, b, a, xl, yl);
	SetVertex(&VertexBuffer[1 * VertexSize], Clip.X + aDestination.Right(), Clip.Y + aDestination.Y, r, g, b, a, xr, yl);
	SetVertex(&VertexBuffer[2 * VertexSize], Clip.X + aDestination.Right(), Clip.Y + aDestination.Bottom(), r, g, b, a, xr, yr);
	SetVertex(&VertexBuffer[3 * VertexSize], Clip.X + aDestination.X, Clip.Y + aDestination.Bottom(), r, g, b, a, xl, yr);

	glDrawArrays(GL_QUADS, 0, 4);
}


void					ESVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort)
{
	assert(aTexture);

	aTexture->Apply();
	PresentFrame(aTexture->GetID(), aTexture->GetWidth(), aTexture->GetHeight(), aViewPort);
}

void					ESVideo::PresentFrame			(FrameBuffer* aFrameBuffer, const Area& aViewPort)
{
	assert(aFrameBuffer);
	PresentFrame(aFrameBuffer->GetID(), aFrameBuffer->GetWidth(), aFrameBuffer->GetHeight(), aViewPort);
}


void					ESVideo::UpdatePresentArea		(int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	//Cache size converted to float
	float fwidth = (float)ESVideo::GetScreenWidth();
	float fheight = (float)ESVideo::GetScreenHeight();

	//Calculate underscan adjustment
	int32_t usLeft = (int32_t)(fwidth * ((float)(aUnderscan + aUnderscanFine.X) / 200.0f));
	int32_t usRight = ESVideo::GetScreenWidth() - (int32_t)(fwidth * ((float)(aUnderscan + aUnderscanFine.Width) / 200.0f));
	int32_t usTop = (int32_t)(fheight * ((float)(aUnderscan + aUnderscanFine.Y) / 200.0f));
	int32_t usBottom = ESVideo::GetScreenHeight() - (int32_t)(fheight * ((float)(aUnderscan + aUnderscanFine.Height) / 200.0f));

	//Update float size to underscaned area
	fwidth = (float)(usRight - usLeft);
	fheight = (float)(usBottom - usTop);

	//1:1 mode
	NoAspect = (aAspectOverride == 2);

	//Apply pillarbox
	if(!NoAspect && ((aAspectOverride == 0 && ESVideo::IsWideScreen()) || (aAspectOverride < 0)))
	{
		usLeft += (int32_t)(fwidth * .125f);
		usRight -= (int32_t)(fwidth * .125f);
	}

	PresentArea = Area(usLeft, usTop, usRight - usLeft, usBottom - usTop);
}

void					ESVideo::PresentFrame			(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort)
{
	//Enter present state
	EnterPresentState();

	//Calculate output area
	Area presentArea;

	if(NoAspect)
	{
		uint32_t x = PresentArea.X, y = PresentArea.Y, w = PresentArea.Width, h = PresentArea.Height;
		Utility::CenterAndScale(x, y, w, h, aViewPort.Width, aViewPort.Height);

		presentArea = Area(x, y, w, h);
	}
	else
	{
		presentArea = PresentArea;
	}

	//Call kiddie present if possible
	if(SupportsShaders())
	{
		Present(aID, aWidth, aHeight, aViewPort, presentArea);

		/* Reset vertex buffer */
		ApplyVertexBuffer(VertexBuffer, true);
	}
	else
	{
		float xl = (float)aViewPort.X / (float)aWidth;
		float xr = (float)aViewPort.Right() / (float)aWidth;
		float yl = (float)aViewPort.Y / (float)aHeight;
		float yr = (float)aViewPort.Bottom() / (float)aHeight;

		glBindTexture(GL_TEXTURE_2D, aID); glSplat();

		SetVertex(&VertexBuffer[0 * VertexSize], presentArea.X, 		presentArea.Y, 			1.0f, 1.0f, 1.0f, 1.0f, xl, yl);
		SetVertex(&VertexBuffer[1 * VertexSize], presentArea.Right(), 	presentArea.Y, 			1.0f, 1.0f, 1.0f, 1.0f, xr, yl);
		SetVertex(&VertexBuffer[2 * VertexSize], presentArea.Right(),	presentArea.Bottom(),	1.0f, 1.0f, 1.0f, 1.0f, xr, yr);
		SetVertex(&VertexBuffer[3 * VertexSize], presentArea.X,			presentArea.Bottom(),	1.0f, 1.0f, 1.0f, 1.0f, xl, yr);

		glDrawArrays(GL_QUADS, 0, 4); glSplat();
	}

	//Exit present state
	ExitPresentState();
}


void					ESVideo::SetVertex				(GLfloat* aBase, float aX, float aY, float aR, float aG, float aB, float aA, float aU, float aV)
{
	*aBase++ = aX; *aBase++ = aY; *aBase++ = 0.0f;
	*aBase++ = aU; *aBase++ = aV;
	*aBase++ = aR; *aBase++ = aG; *aBase++ = aB; *aBase++ = aA;
}

void					ESVideo::InitializeState		()
{
	//Some settings
	glEnable(GL_SCISSOR_TEST); glSplat();
	glClearColor(0, 0, 0, 0); glSplat();
	glEnable(GL_BLEND); glSplat();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glSplat();
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight()); glSplat();

	//Setup Projection
	glMatrixMode(GL_PROJECTION); glSplat();
	glLoadIdentity(); glSplat();
	glOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1); glSplat();
}

void					ESVideo::EnterPresentState		()
{
	glColor4f(1, 1, 1, 1); glSplat();
	glDisable(GL_BLEND); glSplat();
	glDisable(GL_SCISSOR_TEST); glSplat();
}

void					ESVideo::ExitPresentState		()
{
	glEnable(GL_BLEND); glSplat();
	glEnable(GL_SCISSOR_TEST); glSplat();
	glColor4f(0, 0, 0, 0); glSplat();
}


Texture*				ESVideo::FillerTexture;
Area					ESVideo::Clip;

Area					ESVideo::PresentArea;

GLuint					ESVideo::FrameBufferID;

GLfloat*				ESVideo::VertexBuffer;

uint32_t				ESVideo::ScreenWidth;
uint32_t				ESVideo::ScreenHeight;
bool					ESVideo::WideScreen;

bool					ESVideo::NoAspect;
