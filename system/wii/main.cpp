#include <es_system.h>

int dup(int a){printf("dup() stub!"); return a;}

namespace
{
	bool			want_to_die = false;
};

void				SetExit					()
{
	want_to_die = true;
}

void				ESSUB_Init				()
{
	//HACK: Dumb, but it works
	while(!fatInitDefault());

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
}

void				ESSUB_Quit				()
{
	SDL_Quit();
}

ESVideo*			ESSUB_MakeVideo			()
{
	return new WiiVideo();
}

ESAudio*			ESSUB_MakeAudio			()
{
	return new WiiAudio();
}

ESInput*			ESSUB_MakeInput			()
{
	return new WiiInput();
}

ESNetwork*			ESSUB_MakeNetwork		()
{
	return new WiiNetwork();
}

volatile bool		ESSUB_WantToDie			()
{
	return want_to_die;
}

volatile bool		ESSUB_WantToSleep		()
{
	return false;
}

std::string			ESSUB_GetBaseDirectory	()
{
	//TODO:
	return std::string("usb:/mednafen/");
}

