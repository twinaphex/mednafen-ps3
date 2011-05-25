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


						SDLVideo::SDLVideo				()
{
	const SDL_VideoInfo* dispinfo = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	Screen = SDL_SetVideoMode(dispinfo->current_w, dispinfo->current_h, 32, SDL_OPENGL | SDL_FULLSCREEN);
	if(!Screen)
	{
		printf("SDL Couldn't set video mode: %s\n", SDL_GetError());
		throw "SDL couldn't set video mode";
	}
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Mednafen PS3", "Mednafen PS3");
	
	esScreenWidth = dispinfo->current_w;
	esScreenHeight = dispinfo->current_h;
	esWideScreen = false;

	//Some settings
	glEnable(GL_SCISSOR_TEST);
	glClearColor(0, 0, 0, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup vertex buffer
	VertexBuffer = (GLfloat*)malloc(VertexBufferCount * VertexSize * sizeof(GLfloat));
	GLShader::ApplyVertexBuffer(VertexBuffer, true);

	//Setup Projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);

	//Texture for FillRectangle
	FillerTexture = new GLTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Init shaders
	ShaderContext = cgCreateContext();
	Presenter = new GLShader(ShaderContext, "", false, 1);
}

						SDLVideo::~SDLVideo				()
{
	delete FillerTexture;
	free(VertexBuffer);
}

void					SDLVideo::EnableVsync			(bool aOn)
{
	const SDL_VideoInfo* dispinfo = SDL_GetVideoInfo();
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, aOn ? 1 : 0);
	Screen = SDL_SetVideoMode(dispinfo->current_w, dispinfo->current_h, 32, SDL_OPENGL | SDL_FULLSCREEN);
}

void					SDLVideo::SetClip				(const Area& aClip)
{
	ESVideo::SetClip(aClip);
	glScissor(esClip.X, GetScreenHeight() - esClip.Bottom(), esClip.Width, esClip.Height);
}

void					SetExit							();
void					SDLVideo::Flip					()
{
	SDL_GL_SwapBuffers();

//TODO: Maybe a better place to handle the event loop	
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			SetExit();
		}
	}
	
	if(es_input->ButtonDown(0, 0x80000000 | SDLK_F10))
	{
		SetExit();
	}
//TODO: Done with event loop

	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
	glClear(GL_COLOR_BUFFER_BIT);
}

void					SDLVideo::PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor)
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

void					SDLVideo::FillRectangle			(const Area& aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea, Area(0, 0, 2, 2), aColor);
}

void					SDLVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
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

	Presenter->Set(output, aViewPort.Width, aViewPort.Height);
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

