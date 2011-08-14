#include <es_system.h>

void					ESVideoPlatform::Initialize				(uint32_t& aWidth, uint32_t& aHeight)
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

	if(ESHasArgument("-window"))
	{
		Screen = SDL_SetVideoMode(1280, 720, 32, SDL_OPENGL);
	}
	else
	{
		Screen = SDL_SetVideoMode(dispinfo->current_w, dispinfo->current_h, 32, SDL_OPENGL | SDL_FULLSCREEN);
	}

	if(!Screen)
	{
		printf("SDL Couldn't set video mode: %s\n", SDL_GetError());
		throw "SDL couldn't set video mode";
	}
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Mednafen PS3", "Mednafen PS3");
	
	aWidth = dispinfo->current_w;
	aHeight = dispinfo->current_h;

#ifdef __WIN32__
	glewInit();
#endif
}

void					ESVideoPlatform::Shutdown				()
{
}

void					ESVideoPlatform::Flip					()
{
	SDL_GL_SwapBuffers();
}

SDL_Surface*			ESVideoPlatform::Screen;

