#include <src/mednafen.h>
#include <src/git.h>
#include <src/driver.h>
#include <src/general.h>
#include <src/mempatcher.h>
#include <src/md5.h>
#include <include/Fir_Resampler.h>

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

	extern int16_t*						mdfnyab_stereodata16;
	extern uint32_t						mdfnyab_soundcount;
	uint8_t*							mdfnyab_inputports[4];

	void								YuiSwapBuffers				()
	{
	}

	void								YuiErrorMsg					(const char * string)
	{
		MDFN_printf("yabause: %s\n", string);
	}

	M68K_struct * M68KCoreList[] =
	{
		&M68KC68K,
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
//SYSTEM DESCRIPTIONS
static const InputDeviceInputInfoStruct		GamepadIDII[] =
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

static InputDeviceInfoStruct 				InputDeviceInfo[] =
{
	{"none",	"none",		NULL,	0,		NULL			},
	{"gamepad", "Gamepad",	NULL,	13,		GamepadIDII,	},
};

static const InputPortInfoStruct			PortInfo[] =
{
	{0,	"port1",	"Port 1",	2,	InputDeviceInfo, "gamepad" },
	{0,	"port2",	"Port 2",	2,	InputDeviceInfo, "gamepad" },
};

static InputInfoStruct 						yabauseInput =
{
	2,	PortInfo
};

static MDFNSetting							yabauseSettings[] =
{
	{"yabause.bios",	MDFNSF_EMU_STATE,	"Path to Sega Satrun BIOS Image.",	NULL,	MDFNST_STRING,	"satbios.bin"},
	{NULL}
};

static FileExtensionSpecStruct				yabauseExtensions[] =
{
	{".cue", "Sega Saturn Disc Image"},
	{NULL, NULL}
};

//Implement MDFNGI:
static int			yabauseLoad				()
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
	yinit.m68kcoretype = 1;
	yinit.carttype = CART_NONE;
	yinit.regionid = REGION_AUTODETECT;
	yinit.biospath = biospath;
	yinit.cdpath = "";
	yinit.buppath = "br.test";
	yinit.mpegpath = 0;
	yinit.cartpath = 0;
	yinit.netlinksetting = 0;
	yinit.flags = VIDEOFORMATTYPE_NTSC;

	if(YabauseInit(&yinit) < 0)
	{
		MDFN_printf("yabause: Failed to init");
		return 0;
	}

	ScspSetVolume(100);
	ScspUnMuteAudio();

	return 1;
}

static bool			yabauseTestMagic			()
{
	return true;
}

static void			yabauseCloseGame			(void)
{
	//TODO: Anything else?
	YabauseDeInit();
}

static int			yabauseStateAction			(StateMem *sm, int load, int data_only)
{
	return 0;
}

static void			yabauseEmulate				(EmulateSpecStruct *espec)
{
	PERCore->HandleEvents();

	//VIDEO
	int width, height;
	VIDSoftGetScreenSize(&width, &height);

	espec->DisplayRect.x = 0;
	espec->DisplayRect.y = 0;
	espec->DisplayRect.w = width;
	espec->DisplayRect.h = height;


	uint32_t* dest = espec->surface->pixels;
	for(int i = 0; i != height; i ++)
	{
		for(int j = 0; j != width; j ++)
		{
			uint8_t r = (dispbuffer[i * width + j] >> 0) & 0xFF;
			uint8_t g = (dispbuffer[i * width + j] >> 8) & 0xFF;
			uint8_t b = (dispbuffer[i * width + j] >> 16) & 0xFF;

			dest[i * espec->surface->pitchinpix + j] = (r << 16) | (g << 8) | b;
		}
	}

	//AUDIO
	espec->SoundBufSize = mdfnyab_soundcount;
	memcpy(espec->SoundBuf, mdfnyab_stereodata16, mdfnyab_soundcount * 4);
	mdfnyab_soundcount = 0;
}

static void			yabauseSetInput				(int port, const char *type, void *ptr)
{
	if(port >= 0 && port <= 4)
	{
		mdfnyab_inputports[port] = (uint8_t*)ptr;
	}
}

static void			yabauseDoSimpleCommand		(int cmd)
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

//GAME INFO
static MDFNGI				yabauseInfo =
{
/*	shortname:			*/	"yabause",
/*	fullname:			*/	"Sega Saturn (yabause)",
/*	FileExtensions:		*/	yabauseExtensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&yabauseInput,

/*	Load:				*/	0,
/*	TestMagic:			*/	0,
/*	LoadCD:				*/	yabauseLoad,
/*	TestMagicCD:		*/	yabauseTestMagic,
/*	CloseGame:			*/	yabauseCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
/*	StateAction:		*/	yabauseStateAction,
/*	Emulate:			*/	yabauseEmulate,
/*	SetInput:			*/	yabauseSetInput,
/*	DoSimpleCommand:	*/	yabauseDoSimpleCommand,
/*	Settings:			*/	yabauseSettings,
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
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &yabauseInfo : 0;
}

