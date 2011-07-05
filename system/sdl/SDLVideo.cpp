#include <es_system.h>

void					ESVideo::Initialize				()
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
//	Screen = SDL_SetVideoMode(1280, 720, 32, SDL_OPENGL);
	if(!Screen)
	{
		printf("SDL Couldn't set video mode: %s\n", SDL_GetError());
		throw "SDL couldn't set video mode";
	}
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Mednafen PS3", "Mednafen PS3");
	
	ScreenWidth = dispinfo->current_w;
	ScreenHeight = dispinfo->current_h;
	WideScreen = false;

	//Some settings
	OpenGLHelp::InitializeState();

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
}

void					ESVideo::SetScreenSize			(uint32_t aX, uint32_t aY)
{
	ScreenWidth = aX;
	ScreenHeight = aY;
	OpenGLHelp::InitializeState();
	glDisable(GL_SCISSOR_TEST);
}

void					SetExit							();
void					ESVideo::Flip					()
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
	
	if(ESInput::ButtonDown(0, 0x80000000 | SDLK_F10))
	{
		SetExit();
	}
//TODO: Done with event loop

	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
	glClear(GL_COLOR_BUFFER_BIT);
}

void					ESVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	float xl = (float)aViewPort.X / (float)aTexture->GetWidth();
	float xr = (float)aViewPort.Right() / (float)aTexture->GetWidth();
	float yl = (float)aViewPort.Y / (float)aTexture->GetHeight();
	float yr = (float)aViewPort.Bottom() / (float)aTexture->GetHeight();
	Presenter->SetViewport(xl, xr, yl, yr);

	//Enter present state
	OpenGLHelp::EnterPresentState();

	const Area& output = OpenGLHelp::CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);
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
	OpenGLHelp::ExitPresentState();

	/* Reset vertex buffer */
	GLShader::ApplyVertexBuffer(VertexBuffer, true);
}

SDL_Surface*			ESVideo::Screen;
Texture*				ESVideo::FillerTexture;

CGcontext				ESVideo::ShaderContext;
GLShader*				ESVideo::Presenter;

GLfloat*				ESVideo::VertexBuffer;

uint32_t				ESVideo::ScreenWidth;
uint32_t				ESVideo::ScreenHeight;
bool					ESVideo::WideScreen;
Area					ESVideo::Clip;
Texture*				ESVideo::Border;

