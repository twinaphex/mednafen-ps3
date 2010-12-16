#include <ps3_system.h>

Logger*				ps3_log;

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

void				InitPS3					(void (*aExitFunction)())
{
	ExitFunction = aExitFunction;

	sysRegisterCallback(EVENT_SLOT0, (sysCallback)sysutil_callback, NULL);
	SysLoadModule(SYSMODULE_PNGDEC);
	
	netInitialize();

	PS3Video::Init();
	PS3Audio::Init();
	PS3Input::Init();

	ps3_log = new Logger();
}

void				QuitPS3					()
{
	PS3Video::Quit();
	PS3Audio::Quit();
	PS3Input::Quit();

	SysUnloadModule(SYSMODULE_PNGDEC);

	delete ps3_log;

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
