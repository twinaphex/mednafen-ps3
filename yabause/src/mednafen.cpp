#include <mednafen/mednafen.h>
#include <mednafen/git.h>
#include <mednafen/driver.h>
#include <mednafen/general.h>
#include <mednafen/mempatcher.h>
#include <mednafen/md5.h>
#include <mednafen/cdrom/cdromif.h>

#define MODULENAMESPACE Yabause
#include <module_helper.h>
using namespace Yabause;

#include <stdio.h>

//SYSTEM
extern "C"
{
	#include "yabause.h"
	#include "peripheral.h"
	#include "sh2core.h"
	#include "sh2int.h"
	#include "vidogl.h"
	#include "vidsoft.h"
	#include "cs0.h"
	#include "cs2.h"
	#include "cdbase.h"
	#include "scsp.h"
	#include "m68kcore.h"
	#include "sndmdfn.h"
	#include "permdfnjoy.h"

	void								YuiSwapBuffers				()
	{
	}

	void								YuiErrorMsg					(const char * string)
	{
		MDFN_printf("yabause: %s\n", string);
	}

	M68K_struct * M68KCoreList[] =
	{
		&M68KQ68,
		0
	};

	SH2Interface_struct *SH2CoreList[] =
	{
		&SH2Interpreter,
		0,
	};

	PerInterface_struct *PERCoreList[] =
	{
		&PERMDFNJoy,
		0
	};

	CDInterface *CDCoreList[] =
	{
		&ArchCD,
		0
	};

	SoundInterface_struct *SNDCoreList[] =
	{
		&SNDMDFN,
		NULL
	};

	VideoInterface_struct *VIDCoreList[] =
	{
		&VIDSoft,
		NULL
	};
}

namespace MODULENAMESPACE
{
	extern MDFNGI							ModuleInfo;

	static const InputDeviceInputInfoStruct	GamepadIDII[] =
	{
		{ "up",		"UP ↑",				0,	IDIT_BUTTON,			"down"	},
		{ "right",	"RIGHT →",			3,	IDIT_BUTTON,			"left"	},
		{ "down",	"DOWN ↓",			1,	IDIT_BUTTON,			"up"	},
		{ "left",	"LEFT ←",			2,	IDIT_BUTTON,			"right"	},
		{ "rt",		"RIGHT TRIGGER",	10,	IDIT_BUTTON,			NULL	},
		{ "lt",		"LEFT TRIGGER",		11,	IDIT_BUTTON,			NULL	},
		{ "start",	"START",			12,	IDIT_BUTTON,			NULL	},
		{ "a",		"A",				4,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "b",		"B",				5,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "c",		"C",				6,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "x",		"X",				7,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "y",		"Y",				8,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "z",		"Z",				9,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	};

	static InputDeviceInfoStruct 			InputDeviceInfo[] =
	{
		{"none",	"none",		NULL,	0,		NULL			},
		{"gamepad", "Gamepad",	NULL,	13,		GamepadIDII,	},
	};

	static const InputPortInfoStruct		PortInfo[] =
	{
		{0,	"port1",	"Port 1",	2,	InputDeviceInfo, "gamepad" },
		{0,	"port2",	"Port 2",	2,	InputDeviceInfo, "gamepad" },
	};

	static InputInfoStruct 					ModuleInput =
	{
		2,	PortInfo
	};

	static FileExtensionSpecStruct			ModuleExtensions[] =
	{
		{".cue", "Sega Saturn Disc Image"},
		{NULL, NULL}
	};

	static MDFNSetting						ModuleSettings[] =
	{
		{"yabause.bios",	MDFNSF_EMU_STATE,	"Path to Sega Satrun BIOS Image.",	NULL,	MDFNST_STRING,	"satbios.bin"},
		{NULL}
	};

	static int								ModuleLoad				()
	{
		//Get Settings
		static char biospath[1024];
		strncpy(biospath, MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("yabause.bios").c_str()).c_str(), 1024);

		//Init yabause
		yabauseinit_struct yinit;
		memset(&yinit, 0, sizeof(yabauseinit_struct));
		yinit.percoretype = PERCORE_MDFNJOY;
		yinit.sh2coretype = SH2CORE_DEFAULT;
		yinit.vidcoretype = VIDCORE_SOFT;
		yinit.sndcoretype = SNDCORE_MDFN;
		yinit.cdcoretype = CDCORE_ARCH;
		yinit.m68kcoretype = M68KCORE_Q68;
		yinit.carttype = CART_NONE;
		yinit.regionid = REGION_AUTODETECT;
		yinit.biospath = biospath;
		yinit.cdpath = "";
		yinit.buppath = "br.test";
		yinit.mpegpath = 0;
		yinit.cartpath = 0;
		yinit.netlinksetting = 0;
	//	yinit.flags = VIDEOFORMATTYPE_NTSC;

		if(YabauseInit(&yinit) < 0)
		{
			MDFN_printf("yabause: Failed to init");
			return 0;
		}

		ScspSetVolume(100);
		ScspUnMuteAudio();

		return 1;
	}

	static bool								ModuleTestMagic			()
	{
		uint8_t Buffer[4000];
		CDIF_ReadRawSector(Buffer, 0);

		//Works for shining force 3 anyway (only game I have on hand to test)
		return strncmp("SEGA SEGASATURN", (const char*)&Buffer[16], 15) == 0;
	}

	static void								ModuleCloseGame			()
	{
		//TODO: Anything else?
		YabauseDeInit();

		Resampler::Kill();
	}

	static int								ModuleStateAction		(StateMem *sm, int load, int data_only)
	{
		return 0;
	}

	static void								ModuleEmulate			(EmulateSpecStruct *espec)
	{
		//AUDIO PREP
		Resampler::Init(espec, 44100.0);

		//EMULATE
		PERCore->HandleEvents();

		//VIDEO
		int width, height;
		VIDSoftGetScreenSize(&width, &height);

		Video::SetDisplayRect(espec, 0, 0, width, height);
		Video::BlitRGB32<0, 1, 2, 2, 1, 0, -1>(espec, dispbuffer, width, height, width);

		//AUDIO
		Resampler::Fetch(espec);
	}

	static void								ModuleSetInput			(int port, const char *type, void *ptr)
	{
		Input::SetPort(port, (uint8_t*)ptr);
	}

	static void								ModuleDoSimpleCommand	(int cmd)
	{
		if(cmd == MDFN_MSC_RESET)
		{
			YabauseResetButton();
		}
		else if(cmd == MDFN_MSC_POWER)
		{
			YabauseReset();
		}
	}

	MDFNGI									ModuleInfo =
	{
	/*	shortname:			*/	"yabause",
	/*	fullname:			*/	"Sega Saturn (yabause)",
	/*	FileExtensions:		*/	ModuleExtensions,
	/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
	/*	Debugger:			*/	0,
	/*	InputInfo:			*/	&ModuleInput,

	/*	Load:				*/	0,
	/*	TestMagic:			*/	0,
	/*	LoadCD:				*/	ModuleLoad,
	/*	TestMagicCD:		*/	ModuleTestMagic,
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
	/*	lcm_width:			*/	704,
	/*	lcm_height:			*/	512,
	/*  dummy_separator:	*/	0,
	/*	nominal_width:		*/	704,
	/*	nominal_height:		*/	512,
	/*	fb_width:			*/	704,
	/*	fb_height:			*/	512,
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
#define VERSION_FUNC yabauseGetVersion
#define GETEMU_FUNC yabauseGetEmulator
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

