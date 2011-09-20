#include <mednafen/mednafen.h>
#include <mednafen/git.h>
#include <mednafen/driver.h>
#include <mednafen/general.h>
#include <mednafen/mempatcher.h>
#include <mednafen/md5.h>
#include <stdio.h>

#include <mednafen/snes/src/lib/libco/libco.h>

#define MODULENAMESPACE		mupen64plus
#include <module_helper.h>
using namespace mupen64plus;

#define M64P_CORE_PROTOTYPES
#include "api/m64p_frontend.h"
#include "api/m64p_types.h"
#include "plugin/plugin.h"
#include "main/version.h"

//SYSTEM
extern "C"
{
	cothread_t								n64MainThread;
	cothread_t								n64EmuThread;
}

namespace mupen64plus
{
	void									EmuThreadFunction		()
	{
		CoreDoCommand(M64CMD_EXECUTE, 0, NULL);	
	}

	void									mdfnDebugCallback		(void* aContext, int aLevel, const char* aMessage)
	{
		printf("mupen: %s\n", aMessage);
	}

	EmulateSpecStruct*						ESpec;
}
using namespace mupen64plus;

namespace MODULENAMESPACE
{
	static const InputDeviceInputInfoStruct	GamepadIDII[] =
	{
		{ "right",		"RIGHT →",		3,	IDIT_BUTTON,			NULL	},
		{ "left",		"LEFT ←",		2,	IDIT_BUTTON,			NULL	},
		{ "down",		"DOWN ↓",		1,	IDIT_BUTTON,			NULL	},
		{ "up",			"UP ↑",			0,	IDIT_BUTTON,			NULL	},
		{ "start",		"START",		9,	IDIT_BUTTON,			NULL	},
		{ "z_trigger",	"Z Trigger",	8,	IDIT_BUTTON,			NULL	},
		{ "b",			"B",			4,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "a",			"A",			5,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "c_right",	"C RIGHT →",	13,	IDIT_BUTTON,			NULL	},
		{ "c_left",		"C LEFT ←",		12,	IDIT_BUTTON,			NULL	},
		{ "c_down",		"C DOWN ↓",		11,	IDIT_BUTTON,			NULL	},
		{ "c_up",		"C UP ↑",		10,	IDIT_BUTTON,			NULL	},
		{ "r_trigger",	"R Trigger",	7,	IDIT_BUTTON,			NULL	},
		{ "l_trigger",	"L Trigger",	6,	IDIT_BUTTON,			NULL	},
		{ 0,			"",				0,	IDIT_BUTTON,			NULL	},
		{ 0,			"",				0,	IDIT_BUTTON,			NULL	},
	};

	static InputDeviceInfoStruct 			InputDeviceInfo[] =
	{
		{"none",	"none",		NULL,	0,															NULL			},
		{"gamepad", "Gamepad",	NULL,	sizeof(GamepadIDII) / sizeof(InputDeviceInputInfoStruct),	GamepadIDII,	},
	};

	static const InputPortInfoStruct		PortInfo[] =
	{
		{0, "port1", "Port 1", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" },
		{0, "port2", "Port 2", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" },
		{0, "port3", "Port 3", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" },
		{0, "port4", "Port 4", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" },
	};

	static InputInfoStruct 					ModuleInput =
	{
		sizeof(PortInfo) / sizeof(InputPortInfoStruct), PortInfo
	};

	static FileExtensionSpecStruct			ModuleExtensions[] =
	{
		{".n64",	"Nintendo 64 ROM Image"				},
		{NULL, NULL}
	};

	static MDFNSetting						ModuleSettings[] =
	{
		{NULL}
	};

	static int								ModuleLoad				(const char *name, MDFNFILE *fp)
	{
		//Get Game MD5
		md5_context md5;
		md5.starts();
		md5.update(fp->data, fp->size);
		md5.finish(MDFNGameInfo->MD5);

		//Setup mupen
		if(CoreStartup(FRONTEND_API_VERSION, ".", ".", 0, mdfnDebugCallback, 0, 0))
		{
			MDFN_printf("mupen64plus: Failed to initialize core\n");
			return 0;
		}

		if(CoreDoCommand(M64CMD_ROM_OPEN, fp->size, (void*)fp->data))
		{
			MDFN_printf("mupen64plus: Failed to load ROM\n");
			return 0;
		}

		plugin_connect(M64PLUGIN_AUDIO, 0);
		plugin_connect(M64PLUGIN_INPUT, 0);
		plugin_connect(M64PLUGIN_RSP, 0);
		plugin_start(M64PLUGIN_AUDIO);
		plugin_start(M64PLUGIN_INPUT);
		plugin_start(M64PLUGIN_RSP);

		n64MainThread = co_active();
		n64EmuThread = co_create(65536 * sizeof(void*), EmuThreadFunction);

		return 1;
	}

	static bool								ModuleTestMagic			(const char *name, MDFNFILE *fp)
	{
		return true;
	}

	static void								ModuleCloseGame			()
	{
		//Clean up
		Resampler::Kill();
	}

	static int								ModuleStateAction			(StateMem *sm, int load, int data_only)
	{
		return 0;
	}

	static void								ModuleEmulate			(EmulateSpecStruct *espec)
	{
		ESpec = espec;

		//AUDIO PREP
		Resampler::Init(espec, 32000.0);	//TODO: Is 32000 the right number?

		//EMULATE
		co_switch(n64EmuThread);

		//AUDIO
		Resampler::Fetch(espec);

		//TODO: Real timing
		espec->MasterCycles = 1LL * 100;
	}

	static void								ModuleSetInput			(int port, const char *type, void *ptr)
	{
		Input::SetPort(port, (uint8_t*)ptr);
	}

	static void								ModuleDoSimpleCommand	(int cmd)
	{
	}

	MDFNGI									ModuleInfo =
	{
	/*	shortname:			*/	"mupen64plus",
	/*	fullname:			*/	"Nintendo 64 (mupen64plus)",
	/*	FileExtensions:		*/	ModuleExtensions,
	/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
	/*	Debugger:			*/	0,
	/*	InputInfo:			*/	&ModuleInput,

	/*	Load:				*/	ModuleLoad,
	/*	TestMagic:			*/	ModuleTestMagic,
	/*	LoadCD:				*/	0,
	/*	TestMagicCD:		*/	0,
	/*	CloseGame:			*/	ModuleCloseGame,
	/*	ToggleLayer:		*/	0,
	/*	LayerNames:			*/	0,
	/*	InstallReadPatch:	*/	0,
	/*	RemoveReadPatches:	*/	0,
	/*	MemRead:			*/	0,
	/*	StateAction:		*/	ModuleStateAction,
	/*	Emulate:			*/	ModuleEmulate,
	/*	SetInput:			*/	ModuleSetInput,
	/*	DoSimpleCommand:	*/	ModuleDoSimpleCommand,
	/*	Settings:			*/	ModuleSettings,
	/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
	/*	fps:				*/	0,
	/*	multires:			*/	false,
	/*	lcm_width:			*/	640,
	/*	lcm_height:			*/	480,
	/*  dummy_separator:	*/	0,
	/*	nominal_width:		*/	640,
	/*	nominal_height:		*/	480,
	/*	fb_width:			*/	640,
	/*	fb_height:			*/	480,
	/*	soundchan:			*/	2
	};
}

#ifdef MLDLL
#define VERSION_FUNC GetVersion
#define GETEMU_FUNC GetEmulator
#ifdef __WIN32__
#define DLL_PUBLIC __attribute__((dllexport))
#else
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#endif
#else
#define VERSION_FUNC mupen64plusGetVersion
#define GETEMU_FUNC mupen64plusGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x918;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &MODULENAMESPACE::ModuleInfo : 0;
}


