#include <es_system.h>

SYS_PROCESS_PARAM(1001, 0x10000);

namespace
{
	volatile bool	want_to_die = false;
	volatile bool	want_to_sleep = false;
};

static void			sysutil_callback		(uint64_t status, uint64_t param, void *userdata)
{
	(void)param;
	(void)userdata;

	switch (status)
	{
		case CELL_SYSUTIL_REQUEST_EXITGAME:	want_to_die = true; break;
//TODO:
//		case SYSUTIL_MENU_OPEN:				want_to_sleep = true; break; 
//		case SYSUTIL_MENU_CLOSE: 			want_to_sleep = false; break;
	}

	return;
}


void				ESSUB_Init				()
{
	sys_spu_initialize(6, 1);
	cellSysutilRegisterCallback(0, (CellSysutilCallback)sysutil_callback, NULL);

	while(true)
	{
		CellVideoOutState videoState;
		cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &videoState);

		if(videoState.state == CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED)
		{
			break;
		}
	}

	cellSysmoduleLoadModule(CELL_SYSMODULE_FS);		
	cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
}

void				ESSUB_Quit				()
{
	cellSysmoduleUnloadModule(CELL_SYSMODULE_NET);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);	

	cellSysutilUnregisterCallback(0);
}

ESVideo*			ESSUB_MakeVideo			()
{
	return new DorkVideo();
}

ESAudio*			ESSUB_MakeAudio			()
{
	return new DorkAudio();
}

ESInput*			ESSUB_MakeInput			()
{
	return new DorkInput();
}

ESNetwork*			ESSUB_MakeNetwork		()
{
	return new DorkNetwork();
}

ESThreads*			ESSUB_MakeThreads		()
{
	return new DorkThreads();
}


bool				ESSUB_WantToDie			()
{
	cellSysutilCheckCallback();
	return want_to_die;
}

bool				ESSUB_WantToSleep		()
{
	cellSysutilCheckCallback();
	return want_to_sleep;
}

std::string			ESSUB_GetBaseDirectory	()
{
	return "/dev_hdd0/game/MDFN90002/USRDIR/";
}

