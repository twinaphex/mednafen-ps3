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
}

void				ESSUB_Quit				()
{
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

ESNetwork*			ESSUB_MakeNetwork		()
{
	return new L1ghtNetwork();
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

static bool			KillError = false;
static void			DialogCallback			(msgButton button, void *userdata)
{
	if(button != MSGDIALOG_BUTTON_NONE)
	{
		KillError = true;
	}
	return;
}

void				ESSUB_Error				(const char* aMessage)
{
	KillError = false;
	msgDialogOpen(MSGDIALOG_ERROR, aMessage, DialogCallback, 0, 0);

	while(!KillError)
	{
		es_video->Flip();
		sysCheckCallback();
	}

	msgDialogClose();

	exit(100000);
}

