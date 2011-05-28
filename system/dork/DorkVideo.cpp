#include <es_system.h>

namespace
{
	inline void SetVertex(GLfloat* aBase, float aX, float aY, float aR, float aG, float aB, float aA, float aU, float aV)
	{
		*aBase++ = aX; *aBase++ = aY; *aBase++ = 0.0f;
		*aBase++ = aU; *aBase++ = aV;
		*aBase++ = aR; *aBase++ = aG; *aBase++ = aB; *aBase++ = aA;
	}
}

						DorkVideo::DorkVideo			() :
	Device(0),
	Context(0),
	ShaderContext(0),
	VertexBuffer(0),
	FillerTexture(0),
	Presenter(0)
{
	//Init PSGL
	PSGLinitOptions initOpts = {PSGL_INIT_MAX_SPUS | PSGL_INIT_HOST_MEMORY_SIZE, 1, false, 0, 0, 0, 0, 32 * 1024 * 1024};
	psglInit(&initOpts);
	
	Device = psglCreateDeviceAuto(GL_ARGB_SCE, GL_NONE, GL_MULTISAMPLING_NONE_SCE);
	Context = psglCreateContext();
	psglMakeCurrent(Context, Device);
	psglResetCurrentContext();

	//Get Screen Info
	psglGetRenderBufferDimensions(Device, &esScreenWidth, &esScreenHeight);
	esWideScreen = psglGetDeviceAspectRatio(Device) > 1.5f;
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Some settings
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_VSYNC_SCE);
	glClearColor(0, 0, 0, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup vertex buffer
	VertexBuffer = (GLfloat*)memalign(128, VertexBufferCount * VertexSize * sizeof(GLfloat));
	GLShader::ApplyVertexBuffer(VertexBuffer, true);

	//Setup Projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);

	//Texture for FillRectangle
	FillerTexture = new GLTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Init shaders
	cgRTCgcInit();
	ShaderContext = cgCreateContext();
	Presenter = new GLShader(ShaderContext, "", false, 1);
}

						DorkVideo::~DorkVideo			()
{
	delete FillerTexture;
	delete Presenter;

	//Destory ShaderContext

	psglDestroyContext(Context);
	psglDestroyDevice(Device);
	
	psglExit();						

	free(VertexBuffer);
}

void					DorkVideo::EnableVsync			(bool aOn)
{
	if(aOn)
	{
		glEnable(GL_VSYNC_SCE);
	}
	else
	{
		glDisable(GL_VSYNC_SCE);
	}
}

void					DorkVideo::SetClip				(const Area& aClip)
{
	ESVideo::SetClip(aClip);
	glScissor(esClip.X, GetScreenHeight() - esClip.Bottom(), esClip.Width, esClip.Height);
}

void					DorkVideo::Flip					()
{
	psglSwap();

	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
	glClear(GL_COLOR_BUFFER_BIT);
}

void					DorkVideo::PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor)
{
	float r = (float)((aColor >> 24) & 0xFF) / 256.0f;
	float g = (float)((aColor >> 16) & 0xFF) / 256.0f;	
	float b = (float)((aColor >> 8) & 0xFF) / 256.0f;
	float a = (float)((aColor >> 0) & 0xFF) / 256.0f;	

	float xl = 0, xr = 1, yl = 0, yr = 1;
	xl = (float)aSource.X / (float)aTexture->GetWidth();
	xr = (float)aSource.Right() / (float)aTexture->GetWidth();
	yl = (float)aSource.Y / (float)aTexture->GetHeight();
	yr = (float)aSource.Bottom() / (float)aTexture->GetHeight();

	((GLTexture*)aTexture)->Apply();
	SetVertex(&VertexBuffer[0 * VertexSize], esClip.X + aDestination.X, esClip.Y + aDestination.Y, r, g, b, a, xl, yl);
	SetVertex(&VertexBuffer[1 * VertexSize], esClip.X + aDestination.Right(), esClip.Y + aDestination.Y, r, g, b, a, xr, yl);
	SetVertex(&VertexBuffer[2 * VertexSize], esClip.X + aDestination.Right(), esClip.Y + aDestination.Bottom(), r, g, b, a, xr, yr);
	SetVertex(&VertexBuffer[3 * VertexSize], esClip.X + aDestination.X, esClip.Y + aDestination.Bottom(), r, g, b, a, xl, yr);

	glDrawArrays(GL_QUADS, 0, 4);
}

void					DorkVideo::FillRectangle		(const Area& aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea, Area(0, 0, 2, 2), aColor);
}

void					DorkVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	const Area& output = CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);

	float xl = (float)aViewPort.X / (float)aTexture->GetWidth();
	float xr = (float)aViewPort.Right() / (float)aTexture->GetWidth();
	float yl = (float)aViewPort.Y / (float)aTexture->GetHeight();
	float yr = (float)aViewPort.Bottom() / (float)aTexture->GetHeight();
	Presenter->SetViewport(xl, xr, yl, yr);

	//Enter present state
	glColor4f(1, 1, 1, 1);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);

	Presenter->Set(output, aViewPort.Width, aViewPort.Height, aTexture->GetWidth(), aTexture->GetHeight());
	((GLTexture*)aTexture)->Apply();

	GLuint borderTexture = 0;
	if(esBorder)
	{
		((GLTexture*)esBorder)->Apply();
		borderTexture = esBorder ? ((GLTexture*)esBorder)->GetID() : 0;
	}

	Presenter->Present(((GLTexture*)aTexture)->GetID(), borderTexture);

	//Exit present state
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);
	glColor4f(0, 0, 0, 0);

	/* Reset vertex buffer */
	GLShader::ApplyVertexBuffer(VertexBuffer, true);
}

