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

static void			sysutil_callback		(uint64_t status, uint64_t param, void *userdata)
{
	(void)param;
	(void)userdata;

	//TODO: Use constants from psl1ght
	switch (status)
	{
		case EVENT_REQUEST_EXITAPP:				want_to_die = true; break;
		case 0x121:								want_to_sleep = true; break; 
		case 0x122: 							want_to_sleep = false; break;
	}

	return;
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

	sysRegisterCallback(EVENT_SLOT0, (sysCallback)sysutil_callback, NULL);
	SysLoadModule(SYSMODULE_PNGDEC);
	
	netInitialize();

	es_video = new L1ghtVideo();
	es_audio = new L1ghtAudio();
	es_input = new L1ghtInput();
	
	FontManager::InitFonts();

	es_log = new Logger();
}

void				QuitES					()
{
	delete es_log;

	FontManager::QuitFonts();

	delete es_video;
	delete es_audio;
	delete es_input;

	SysUnloadModule(SYSMODULE_PNGDEC);
	sysUnregisterCallback(EVENT_SLOT0);
}

volatile bool		WantToDie				()
{
	sysCheckCallback();
	
	if(want_to_die)
	{
		ExitFunction();
		exit(1);
	}
	
	return want_to_die;
}

volatile bool		WantToSleep				()
{
	sysCheckCallback();
	return want_to_sleep;
}
