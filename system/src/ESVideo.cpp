#include <es_system.h>

#ifdef	MDCELL
#define	glOrtho	glOrthof
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
}

void					ESVideo::Shutdown				()
{
	delete FillerTexture;
	free(VertexBuffer);

	ESVideoPlatform::Shutdown();
}

void					ESVideo::SetScreenSize			(uint32_t aX, uint32_t aY)
{
	ScreenWidth = aX;
	ScreenHeight = aY;
	InitializeState();
	glDisable(GL_SCISSOR_TEST);
}

void					ESVideo::Flip					()
{
	ESVideoPlatform::Flip();
	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
	glClear(GL_COLOR_BUFFER_BIT);
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


void					ESVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	float xl = (float)aViewPort.X / (float)aTexture->GetWidth();
	float xr = (float)aViewPort.Right() / (float)aTexture->GetWidth();
	float yl = (float)aViewPort.Y / (float)aTexture->GetHeight();
	float yr = (float)aViewPort.Bottom() / (float)aTexture->GetHeight();
	Presenter->SetViewport(xl, xr, yl, yr);

	//Enter present state
	EnterPresentState();

	const Area& output = CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);
	Presenter->Set(output, aViewPort.Width, aViewPort.Height, aTexture->GetWidth(), aTexture->GetHeight());
	aTexture->Apply();

	GLuint borderTexture = 0;
	if(Border)
	{
		Border->Apply();
		borderTexture = Border ? Border->GetID() : 0;
	}

	Presenter->Present(aTexture->GetID(), borderTexture);

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
	glOrtho(0, ESVideo::GetScreenWidth(), ESVideo::GetScreenHeight(), 0, -1, 1);
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


const Area&				ESVideo::CalculatePresentArea	(int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	static Area PresentArea;
	static int32_t LastAspect;
	static int32_t LastUnderscan;
	static Area LastUnderscanFine;

	if(aAspectOverride != LastAspect || LastUnderscan != aUnderscan || aUnderscanFine != LastUnderscanFine)
	{
		LastAspect = aAspectOverride;
		LastUnderscan = aUnderscan;
		LastUnderscanFine = aUnderscanFine;

		int32_t xLeft = 0, xRight = ESVideo::GetScreenWidth(), yTop = 0, yBottom = ESVideo::GetScreenHeight();
		float fwidth = (float)ESVideo::GetScreenWidth();
		float fheight = (float)ESVideo::GetScreenHeight();

		if((LastAspect == 0 && ESVideo::IsWideScreen()) || (LastAspect < 0))
		{
			xLeft += (int32_t)(fwidth * .125f);
			xRight -= (int32_t)(fwidth * .125f);
			fwidth -= fwidth * .250f;
		}

		xLeft += (int32_t)(fwidth * ((float)(LastUnderscan + LastUnderscanFine.X) / 200.0f));
		xRight -= (int32_t)(fwidth * ((float)(LastUnderscan + LastUnderscanFine.Width) / 200.0f));
		yTop += (int32_t)(fheight * ((float)(LastUnderscan + LastUnderscanFine.Y) / 200.0f));
		yBottom -= (int32_t)(fheight * ((float)(LastUnderscan + LastUnderscanFine.Height) / 200.0f));

		PresentArea = Area(xLeft, yTop, xRight - xLeft, yBottom - yTop);
	}

	return PresentArea;
}


Texture*				ESVideo::FillerTexture;

CGcontext				ESVideo::ShaderContext;
GLShader*				ESVideo::Presenter;

GLfloat*				ESVideo::VertexBuffer;

uint32_t				ESVideo::ScreenWidth;
uint32_t				ESVideo::ScreenHeight;
bool					ESVideo::WideScreen;
Area					ESVideo::Clip;
Texture*				ESVideo::Border;

