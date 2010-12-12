#include <ps3_system.h>

Logger*				ps3_log;

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
		case EVENT_REQUEST_EXITAPP:
			want_to_die = true;
			break;
			
		case 0x121: //CELL_SYSUTIL_DRAWING_BEGIN:
			want_to_sleep = true;
			break;
			
		case 0x122: //CELL_SYSUTIL_DRAWING_END:
			want_to_sleep = false;
			break;

		default:
			break;
	}

	return;
}

void				TestError				(bool aCondition, const char* aMessage)
{
	if(!aCondition)
	{
		FILE* file = fopen("/dev_usb006/error.message", "w");
		if(file)
		{
			fprintf(file, "%s\n", aMessage);
			fclose(file);
		}
		
		exit(1);
	}
}

int					printf					(const char * format, ... )
{
	char array[1024];
	va_list args;
	va_start (args, format);
	vsnprintf(array, 1024, format, args);
	va_end(args);
	
	ps3_log->Log(array);
	return strlen(array);
}


void				InitPS3					()
{
	sysRegisterCallback(EVENT_SLOT0, (sysCallback)sysutil_callback, NULL);
	SysLoadModule(SYSMODULE_PNGDEC);

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


	if(ps3_log)
	{
		delete ps3_log;
	}

	sysUnregisterCallback(EVENT_SLOT0);
}

volatile bool		WantToDie				()
{
	sysCheckCallback();
	return want_to_die;
}

volatile bool		WantToSleep				()
{
	sysCheckCallback();
	return want_to_sleep;
}
