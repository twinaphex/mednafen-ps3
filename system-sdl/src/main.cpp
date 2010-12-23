#include <ps3_system.h>

Logger*				ps3_log;

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

void				InitPS3					(void (*aExitFunction)())
{
	ExitFunction = aExitFunction;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	PS3Video::Init();
	PS3Audio::Init();	
	PS3Input::Init();

	ps3_log = new Logger();
}

void				QuitPS3					()
{
	delete ps3_log;
	
	PS3Input::Quit();
	PS3Audio::Quit();
	PS3Video::Quit();
	
	printf("DONE\n");
	
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
