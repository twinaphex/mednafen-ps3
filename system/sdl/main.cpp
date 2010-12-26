#include <ps3_system.h>

Logger*				es_log = 0;
ESVideo*			es_video = 0;
ESAudio*			es_audio = 0;
ESInput*			es_input = 0;

namespace
{
	volatile bool	want_to_die = false;
	volatile bool	want_to_sleep = false;
	void			(*ExitFunction)() = 0;
};

void				SetExit					()
{
	want_to_die = true;
}

void				Abort					(const char* aMessage)
{
	printf("ABORT: %s\n", aMessage);
	
	if(ExitFunction)
	{
		ExitFunction();
	}
	
	abort();
}

void				InitES					(void (*aExitFunction)())
{
	ExitFunction = aExitFunction;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER);

	es_video = new SDLVideo();
	es_audio = new SDLAudio();
	es_input = new SDLInput();

	FontManager::InitFonts();

	es_log = new Logger();
}

void				QuitES					()
{
	delete es_log;

	FontManager::QuitFonts();

	delete es_input;
	delete es_audio;
	delete es_video;

	SDL_Quit();
}

volatile bool		WantToDie				()
{
	if(want_to_die)
	{
		ExitFunction();
		exit(1);
	}
	
	return want_to_die;
}

volatile bool		WantToSleep				()
{
	return want_to_sleep;
}
