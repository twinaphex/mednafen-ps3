#include <es_system.h>
#include <sysutil/sysutil_msgdialog.h>
#include <cell/sysmodule.h>
#include <sys/spu_initialize.h>
#include <sysutil/sysutil_sysparam.h>
#include <sys/process.h>

SYS_PROCESS_PARAM(1001, 1024 * 1024);

namespace
{
	volatile bool	want_to_die = false;
	volatile bool	want_to_sleep = false;
	uint32_t		sleep_counter = 10;

	void			sysutil_callback		(uint64_t status, uint64_t param, void *userdata)
	{
		switch (status)
		{
			case CELL_SYSUTIL_REQUEST_EXITGAME:	want_to_die = true; break;
			case CELL_SYSUTIL_SYSTEM_MENU_OPEN:	want_to_sleep = true; break; 
			case CELL_SYSUTIL_SYSTEM_MENU_CLOSE:want_to_sleep = false; sleep_counter = 180; break;
		}

		return;
	}
};

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
	cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC);
}

void				ESSUB_Quit				()
{
	cellSysmoduleUnloadModule(CELL_SYSMODULE_PNGDEC);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_NET);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);	

	cellSysutilUnregisterCallback(0);
}

struct				ErrorData
{
	bool			Accept;
	bool			Cancel;
};

static void			ErrorCallback			(int32_t aButton, void* aUserData)
{
	want_to_sleep = false;
	sleep_counter = 180;

	if(aUserData)
	{
		ErrorData* data = (ErrorData*)aUserData;
		data->Accept = (aButton == CELL_MSGDIALOG_BUTTON_YES);
		data->Cancel = (aButton == CELL_MSGDIALOG_BUTTON_ESCAPE);
	}
}

void				ESSUB_Error				(const char* aMessage, const char* aHeader)
{
	want_to_sleep = true;
	cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, aMessage, ErrorCallback, 0, 0);

	while(WantToSleep() && !WantToDie())
	{
		ESVideo::Flip();
	}
}

bool				ESSUB_Confirm			(const char* aMessage, bool* aCancel)
{
	ErrorData result;

	want_to_sleep = true;
	cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO, aMessage, ErrorCallback, &result, 0);

	while(WantToSleep() && !WantToDie())
	{
		ESVideo::Flip();
	}

	if(aCancel)
	{
		*aCancel = result.Cancel;
	}

	return result.Accept;
}

bool				ESSUB_WantToDie			()
{
	cellSysutilCheckCallback();
	return want_to_die;
}

bool				ESSUB_WantToSleep		()
{
	cellSysutilCheckCallback();

	if(sleep_counter)
	{
		sleep_counter --;
	}

	return want_to_sleep || (sleep_counter != 0);
}

std::string			ESSUB_BuildPath			(const std::string& aPath)
{
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#ifndef ES_HOME_PATH
#define ES_HOME_PATH /dev_hdd0/game/MDFN90002/USRDIR
#endif
#define ES_DIR TOSTRING(ES_HOME_PATH)

	return std::string(ES_DIR"/") + aPath;
}


