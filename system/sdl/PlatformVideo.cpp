#include <es_system.h>
#include <opengl_common/Presenter.h>

namespace
{
	SDL_Surface*								Screen;
}

void											ESVideoPlatform::Initialize				(uint32_t& aWidth, uint32_t& aHeight)
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

	CgPresenter::Initialize();
}

void											ESVideoPlatform::Shutdown				()
{
	CgPresenter::Shutdown();

	//TODO
}

void											ESVideoPlatform::Flip					()
{
	SDL_GL_SwapBuffers();
}

//PRESENTER
bool											ESVideoPlatform::SupportsShaders		()
{
	return true;
}

void											ESVideoPlatform::AttachBorder			(Texture* aTexture)
{
	CgPresenter::AttachBorder(aTexture);
}

void											ESVideoPlatform::SetFilter				(const std::string& aName, uint32_t aPrescale)
{
	CgPresenter::SetFilter(aName, aPrescale);
}

void											ESVideoPlatform::Present				(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort, const Area& aOutput)
{
	CgPresenter::Present(aID, aWidth, aHeight, aViewPort, aOutput);
}


//OTHER
bool											ESVideoPlatform::SupportsVSyncSelect	()
{
	return false;
}

bool											ESVideoPlatform::SupportsModeSwitch		()
{
	return false;
}

void											ESVideoPlatform::SetVSync				(bool aOn) {assert(false);}
void											ESVideoPlatform::SetMode				(uint32_t aIndex) {assert(false);}
ESVideoPlatform::ModeList::const_iterator		ESVideoPlatform::GetModes				() {assert(false);}

