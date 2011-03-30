#include <es_system.h>

namespace
{
	GLfloat	verts[12];
	GLfloat	tverts[8];
	GLfloat	colors[16];

	void SetVertex(uint32_t aVert, float aX, float aY, float aR, float aG, float aB, float aA, float aU, float aV)
	{
		verts[aVert * 3 + 0] = aX;
		verts[aVert * 3 + 1] = aY;
		verts[aVert * 3 + 2] = 0;

		colors[aVert * 4 + 0] = aR;
		colors[aVert * 4 + 1] = aG;
		colors[aVert * 4 + 2] = aB;
		colors[aVert * 4 + 3] = aA;

		tverts[aVert * 2 + 0] = aU;
		tverts[aVert * 2 + 1] = aV;
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
//TODO:
	esWideScreen = false;

	glEnable(GL_VSYNC_SCE);
	glEnableClientState(GL_VERTEX_ARRAY);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");
	glEnableClientState(GL_COLOR_ARRAY);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");
		
	FillerTexture = new DorkTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);
}

						DorkVideo::~DorkVideo			()
{
	delete FillerTexture;

	psglDestroyContext(Context);
	psglDestroyDevice(Device);
	
	psglExit();						
}

void					DorkVideo::SetClip				(Area aClip)
{
	ESVideo::SetClip(aClip);

	Area clap = GetClip();
	glScissor(clap.X, GetScreenHeight() - clap.Bottom(), clap.Width, clap.Height);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");
}

void					DorkVideo::Flip					()
{
	psglSwap();

	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());
	glEnable(GL_SCISSOR_TEST);
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
	SetVertex(0, esClip.X + aDestination.X, esClip.Y + aDestination.Y, r, g, b, a, xl, yl);
	SetVertex(1, esClip.X + aDestination.Right(), esClip.Y + aDestination.Y, r, g, b, a, xr, yl);
	SetVertex(2, esClip.X + aDestination.Right(), esClip.Y + aDestination.Bottom(), r, g, b, a, xr, yr);
	SetVertex(3, esClip.X + aDestination.X, esClip.Y + aDestination.Bottom(), r, g, b, a, xl, yr);

	glVertexPointer(3, GL_FLOAT, 0, verts);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");

	glTexCoordPointer(2, GL_FLOAT, 0, tverts);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");

	glColorPointer(4, GL_FLOAT, 0, colors);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");

	glDrawArrays(GL_QUADS, 0, 4);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");
}

void					DorkVideo::FillRectangle		(const Area& aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea, Area(0, 0, 2, 2), aColor);
}

void					DorkVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	Area output = CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);

	float xl = (float)aViewPort.X / (float)aTexture->GetWidth();
	float xr = (float)aViewPort.Right() / (float)aTexture->GetWidth();
	float yl = (float)aViewPort.Y / (float)aTexture->GetHeight();
	float yr = (float)aViewPort.Bottom() / (float)aTexture->GetHeight();

	((DorkTexture*)aTexture)->Apply();
	glDisable(GL_BLEND);

	SetVertex(0, output.X, output.Y, 1.0f, 1.0f, 1.0f, 1.0f, xl, yl);
	SetVertex(1, output.Right(), output.Y, 1.0f, 1.0f, 1.0f, 1.0f, xr, yl);
	SetVertex(2, output.Right(), output.Bottom(), 1.0f, 1.0f, 1.0f, 1.0f, xr, yr);
	SetVertex(3, output.X, output.Bottom(), 1.0f, 1.0f, 1.0f, 1.0f, xl, yr);

	glVertexPointer(3, GL_FLOAT, 0, verts);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");

	glTexCoordPointer(2, GL_FLOAT, 0, tverts);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");

	glColorPointer(4, GL_FLOAT, 0, colors);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");

	glDrawArrays(GL_QUADS, 0, 4);
	ErrorCheck(glGetError() == GL_NO_ERROR, "a");

	glEnable(GL_BLEND);
}
