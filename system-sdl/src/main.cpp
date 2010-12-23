#include <ps3_system.h>

Logger*				es_log;

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

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	ESVideo::Init();
	ESAudio::Init();	
	ESInput::Init();

	es_log = new Logger();
}

void				QuitES					()
{
	delete es_log;
	
	ESInput::Quit();
	ESAudio::Quit();
	ESVideo::Quit();

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
