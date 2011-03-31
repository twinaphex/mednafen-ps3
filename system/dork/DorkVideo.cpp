#include <es_system.h>

namespace
{
	inline void SetVertex(GLfloat* aBase, float aX, float aY, float aR, float aG, float aB, float aA, float aU, float aV)
	{
		*aBase++ = aX; *aBase++ = aY; *aBase++ = 0.0f;
		*aBase++ = aR; *aBase++ = aG; *aBase++ = aB; *aBase++ = aA;
		*aBase++ = aU; *aBase++ = aV;
	}
}

						DorkVideo::DorkVideo			() :
	Device(0),
	Context(0),
	FillerTexture(0)
{
	PSGLinitOptions initOpts = 
	{
		enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_HOST_MEMORY_SIZE,
		maxSPUs: 1, 
		initializeSPUs: false,
		persistentMemorySize: 0,
		transientMemorySize: 0,
		errorConsole: 0,
		fifoSize: 0,  
		hostMemorySize: 128 * 1024 * 1024
	};
	
	psglInit(&initOpts);
	
	Device = psglCreateDeviceAuto(GL_ARGB_SCE, GL_NONE, GL_MULTISAMPLING_NONE_SCE);
	Context = psglCreateContext();
	psglMakeCurrent(Context, Device);
    psglResetCurrentContext();

	psglGetRenderBufferDimensions(Device, &esScreenWidth, &esScreenHeight);
	esWideScreen = psglGetDeviceAspectRatio(Device) > 1.5f;

	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());
	glEnable(GL_SCISSOR_TEST);


	VertexBuffer = (GLfloat*)memalign(128, VertexBufferCount * VertexSize * sizeof(GLfloat));

	glEnable(GL_VSYNC_SCE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, VertexSize * sizeof(GLfloat), &VertexBuffer[0]);
	glTexCoordPointer(2, GL_FLOAT, VertexSize * sizeof(GLfloat), &VertexBuffer[7]);
	glColorPointer(4, GL_FLOAT, VertexSize * sizeof(GLfloat), &VertexBuffer[3]);

	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FillerTexture = new DorkTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);
}

						DorkVideo::~DorkVideo			()
{
	delete FillerTexture;

	psglDestroyContext(Context);
	psglDestroyDevice(Device);
	
	psglExit();						

	free(VertexBuffer);
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

	((DorkTexture*)aTexture)->Apply();
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

	((DorkTexture*)aTexture)->Apply();
	glDisable(GL_BLEND);

	SetVertex(&VertexBuffer[0 * VertexSize], output.X, output.Y, 1.0f, 1.0f, 1.0f, 1.0f, xl, yl);
	SetVertex(&VertexBuffer[1 * VertexSize], output.Right(), output.Y, 1.0f, 1.0f, 1.0f, 1.0f, xr, yl);
	SetVertex(&VertexBuffer[2 * VertexSize], output.Right(), output.Bottom(), 1.0f, 1.0f, 1.0f, 1.0f, xr, yr);
	SetVertex(&VertexBuffer[3 * VertexSize], output.X, output.Bottom(), 1.0f, 1.0f, 1.0f, 1.0f, xl, yr);

	glDrawArrays(GL_QUADS, 0, 4);

	glEnable(GL_BLEND);
}
