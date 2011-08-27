#include <es_system.h>

#include "opengl_common/FrameBuffer.h"

#ifndef __CELLOS_LV2__
# define	glGenFramebuffersES			glGenFramebuffersEXT
# define	glDeleteFramebuffersES		glDeleteFramebuffersEXT
# define	glBindFramebufferES			glBindFramebufferEXT
# define	glFramebufferTexture2DES	glFramebufferTexture2DEXT
# define	GL_FRAMEBUFFER_ES			GL_FRAMEBUFFER_EXT
#else
# define	glGenFramebuffersES			glGenFramebuffersOES
# define	glDeleteFramebuffersES		glDeleteFramebuffersOES
# define	glBindFramebufferES			glBindFramebufferOES
# define	glFramebufferTexture2DES	glFramebufferTexture2DOES
# define	GL_FRAMEBUFFER_ES			GL_FRAMEBUFFER_OES
# define	glOrtho						glOrthof
#endif


void					ESVideo::Initialize				()
{
	ESVideoPlatform::Initialize(ScreenWidth, ScreenHeight);

	//Some settings
	InitializeState();

	// Setup vertex buffer
	VertexBuffer = (GLfloat*)malloc(VertexBufferCount * VertexSize * sizeof(GLfloat));
	GLShader::ApplyVertexBuffer(VertexBuffer, true);

	//Texture for FillRectangle
	FillerTexture = new Texture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Init shaders
	ShaderContext = cgCreateContext();
	Presenter = new GLShader(ShaderContext, "", false, 1);

	//Init framebuffer
	glGenFramebuffersES(1, &FrameBufferID);
}

void					ESVideo::Shutdown				()
{
	SetRenderTarget(0);
	glDeleteFramebuffersES(1, &FrameBufferID);

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
	glClear(GL_COLOR_BUFFER_BIT);
}

void					ESVideo::SetRenderTarget		(FrameBuffer* aBuffer)
{
	if(aBuffer)
	{
		glBindTexture(GL_TEXTURE_2D, aBuffer->GetID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebufferES(GL_FRAMEBUFFER_ES, FrameBufferID);
		glFramebufferTexture2DES(GL_FRAMEBUFFER_ES, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, aBuffer->GetID(), 0);
	}
	else
	{
		glBindFramebufferES(GL_FRAMEBUFFER_ES, 0);
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
	float xl = (float)aViewPort.X / (float)aWidth;
	float xr = (float)aViewPort.Right() / (float)aWidth;
	float yl = (float)aViewPort.Y / (float)aHeight;
	float yr = (float)aViewPort.Bottom() / (float)aHeight;
	Presenter->SetViewport(xl, xr, yl, yr);

	//Enter present state
	EnterPresentState();

	if(NoAspect)
	{
		uint32_t x = PresentArea.X, y = PresentArea.Y, w = PresentArea.Width, h = PresentArea.Height;
		Utility::CenterAndScale(x, y, w, h, aViewPort.Width, aViewPort.Height);

		Presenter->Set(Area(x,y,w,h), aViewPort.Width, aViewPort.Height, aWidth, aHeight);
	}
	else
	{
		Presenter->Set(PresentArea, aViewPort.Width, aViewPort.Height, aWidth, aHeight);
	}

	GLuint borderTexture = 0;
	if(Border)
	{
		Border->Apply();
		borderTexture = Border ? Border->GetID() : 0;
	}

	Presenter->Present(aID, borderTexture);

	//Exit present state
	ExitPresentState();

	/* Reset vertex buffer */
	GLShader::ApplyVertexBuffer(VertexBuffer, true);
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
	glEnable(GL_SCISSOR_TEST);
	glClearColor(0, 0, 0, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Setup Projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);
}

void					ESVideo::EnterPresentState		()
{
	glColor4f(1, 1, 1, 1);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
}

void					ESVideo::ExitPresentState		()
{
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);
	glColor4f(0, 0, 0, 0);
}


Texture*				ESVideo::FillerTexture;
Area					ESVideo::Clip;

Area					ESVideo::PresentArea;

GLuint					ESVideo::FrameBufferID;

GLfloat*				ESVideo::VertexBuffer;

CGcontext				ESVideo::ShaderContext;
GLShader*				ESVideo::Presenter;
Texture*				ESVideo::Border;

uint32_t				ESVideo::ScreenWidth;
uint32_t				ESVideo::ScreenHeight;
bool					ESVideo::WideScreen;

bool					ESVideo::NoAspect;
