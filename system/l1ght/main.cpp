#include <ps3_system.h>

namespace
{
	volatile bool	want_to_die = false;
	volatile bool	want_to_sleep = false;
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


void				ESSUB_Init				()
{
	sysRegisterCallback(EVENT_SLOT0, (sysCallback)sysutil_callback, NULL);
	SysLoadModule(SYSMODULE_PNGDEC);
	
	netInitialize();
}

void				ESSUB_Quit				()
{
	SysUnloadModule(SYSMODULE_PNGDEC);
	sysUnregisterCallback(EVENT_SLOT0);
}

ESVideo*			ESSUB_MakeVideo			()
{
	return new L1ghtVideo();
}

ESAudio*			ESSUB_MakeAudio			()
{
	return new L1ghtAudio();
}

ESInput*			ESSUB_MakeInput			()
{
	return new L1ghtInput();
}

bool				ESSUB_WantToDie			()
{
	sysCheckCallback();
	return want_to_die;
}

bool				ESSUB_WantToSleep		()
{
	sysCheckCallback();
	return want_to_sleep;
}

std::string			ESSUB_GetBaseDirectory	()
{
	return "/dev_hdd0/game/MDFN90002/USRDIR/";
}