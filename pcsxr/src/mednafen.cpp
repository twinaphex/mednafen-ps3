//Includes from mednafen
#include <src/mednafen.h>
#include <src/cdrom/cdromif.h>
#include <src/mednafen-driver.h>
#include <src/git.h>
#include <include/Fir_Resampler.h>
#include <src/general.h>
#include <stdarg.h>

//HACK: We really shouldn't use this here!
//#include <ps3_system.h>

namespace
{
	EmulateSpecStruct*			ESpec;
	uint8_t*					Ports[8];
}

extern "C" void MDFNDC_Sleep(uint32_t aMS){MDFND_Sleep(aMS);}
extern "C" uint32_t MDFNDC_GetTime(){return MDFND_GetTime();}

//Definitions for PCSX
extern "C"
{
	void		SysLoad					();
	void		SysClose				();
	void		SysReset				();
	void		SysFrame				(uint32_t aSkip, uint32_t* aPixels, uint32_t aPitch, uint32_t aKeys, uint32_t* aWidth, uint32_t* aHeight, uint32_t* aSound, uint32_t* aSoundLen);
	void		SetMCDS					(const char* aOne, const char* aTwo);
	void		SetBIOS					(const char* aPath);

	void				SysPrintf		(const char *fmt, ...)
	{
		char buffer[2048];
		va_list args;
		va_start (args, fmt);
		vsnprintf(buffer, 2048, fmt, args);
		va_end (args);

		MDFND_Message(buffer);
	}

	void				SysMessage		(const char *fmt, ...)
	{
		char buffer[2048];
		va_list args;
		va_start (args, fmt);
		vsnprintf(buffer, 2048, fmt, args);
		va_end (args);

		MDFND_Message(buffer);
	}

	uint32_t	DoesFileExist			(const char* aPath)
	{
//		return Utility::FileExists(aPath) ? 1 : 0;
		return 0;
	}
}

static Fir_Resampler<8> resampler;

int				PcsxrLoad				()
{
	resampler.buffer_size(588 * 2 * 2 + 100);
	resampler.time_ratio((double)44100 / 48000.0, 0.9965);

	std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
	std::string filename2 = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav2");
	SetMCDS(filename.c_str(), filename2.c_str());

	std::string biospath = MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("pcsxr.bios").c_str());
	SetBIOS(biospath.c_str());

	SysLoad();
//	SysInit();
	return 1;
}

bool			PcsxrTestMagic			()
{
	//TODO: Does this work in all cases?
	uint8_t Buffer[4000];
	CDIF_ReadRawSector(Buffer, 4);
	return Buffer[56] == 'S' && Buffer[57] == 'o' && Buffer[58] == 'n' && Buffer[59] == 'y';
}

void			PcsxrCloseGame			(void)
{
	SysClose();
}

void			PcsxrInstallReadPatch	(uint32 address)
{
	//TODO:
}

void			PcsxrRemoveReadPatches	(void)
{
	//TODO:
}

uint8			PcsxrMemRead			(uint32 addr)
{
	//TODO:
	return 0;
}

int				PcsxrStateAction		(StateMem *sm, int load, int data_only)
{
	return 0;
}

uint16_t bingbang[48000];
void			PcsxrEmulate			(EmulateSpecStruct *espec)
{
    ESpec = espec;

    if(espec->SoundFormatChanged)
    {
    }

	uint32_t width, height;
	uint32_t sndsize;
	SysFrame(espec->skip, espec->surface->pixels, espec->surface->pitch32, Ports[0][0] | (Ports[0][1] << 8), &width, &height, (uint32_t*)bingbang, &sndsize);

	if(sndsize < 1500)
	{
		for(int i = 0; i != sndsize * 2; i ++)
		{
			resampler.buffer()[i] = /*(rand() & 0x7FFF) - 0x4000;*/ bingbang[i];
		}

		resampler.write(sndsize * 2);
		espec->SoundBufSize = resampler.read(espec->SoundBuf, resampler.avail()) >> 1;
	}

    //TODO: Support color shift
    //TODO: Support multiplayer

    espec->DisplayRect.x = 0;
    espec->DisplayRect.y = 0;
    espec->DisplayRect.w = width;
    espec->DisplayRect.h = height;

	espec->MasterCycles = 1LL * 100;
}

void			PcsxrSetInput			(int port, const char *type, void *ptr)
{
	if(port >= 0 && port <= 4)
	{
		Ports[port] = (uint8_t*)ptr;
	}
}

void			PcsxrDoSimpleCommand	(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		SysReset();
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		SysReset();
	}
}

static const InputDeviceInputInfoStruct GamepadIDII[] =
{
	{"select",	"SELECT",	15,	IDIT_BUTTON, NULL},
	{"l3",		"L3",		13,	IDIT_BUTTON, NULL},
	{"r3",		"R3",		10,	IDIT_BUTTON, NULL},
	{"start",	"START",	14,	IDIT_BUTTON, NULL},
	{"up",		"UP",		0,	IDIT_BUTTON, "down"},
	{"right",	"RIGHT",	3,	IDIT_BUTTON, "left"},
	{"down",	"DOWN",		1,	IDIT_BUTTON, "up"},
	{"left",	"LEFT",		2,	IDIT_BUTTON, "right"},
	{"l2",		"L2",		12,	IDIT_BUTTON, NULL},
	{"r2",		"R2",		9,	IDIT_BUTTON, NULL},
	{"l1",		"L1",		11, IDIT_BUTTON, NULL},
	{"r1",		"R1",		8,	IDIT_BUTTON, NULL},
	{"triangle","TRIANGLE",	4,	IDIT_BUTTON, 0},
	{"circle",	"CIRCLE",	5,	IDIT_BUTTON, 0},
	{"cross",	"CROSS",	6,	IDIT_BUTTON, 0},
	{"square",	"SQUARE",	7,	IDIT_BUTTON, 0},
};

static InputDeviceInfoStruct InputDeviceInfoPSXPort[] =
{
	{"none",	"none",		NULL,	0,	NULL},
	{"gamepad", "Gamepad",	NULL,	16,	GamepadIDII},
};


static const InputPortInfoStruct PortInfo[] =
{
	{0, "port1", "Port 1", 2, InputDeviceInfoPSXPort, "gamepad"},
};

InputInfoStruct		PcsxrInput =
{
	1,
	PortInfo
};


static FileExtensionSpecStruct	extensions[] = 
{
	{".cue", "PSX Cue File"},
	{0, 0}
};


static MDFNSetting PcsxrSettings[] =
{
	{"pcsxr.bios",		MDFNSF_EMU_STATE,	"Path to optional (but recommended) PSX BIOS ROM image.",				NULL, MDFNST_STRING,	"scph1001.bin"},
	{"pcsxr.recompiler",MDFNSF_NOFLAGS,		"Enable the dynamic recompiler. (Need to restart mednafen to change).",	NULL, MDFNST_BOOL,		"0"},
	{NULL}
};


MDFNGI	PcsxrInfo = 
{
/*	shortname:			*/	"pcsxr",
/*	fullname:			*/	"PCSX-Reloaded",
/*	FileExtensions:		*/	extensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&PcsxrInput,

/*	Load:				*/	0,
/*	TestMagic:			*/	0,
/*	LoadCD:				*/	PcsxrLoad,
/*	TestMagicCD:		*/	PcsxrTestMagic,
/*	CloseGame:			*/	PcsxrCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	PcsxrInstallReadPatch,
/*	RemoveReadPatches:	*/	PcsxrRemoveReadPatches,
/*	MemRead:			*/	PcsxrMemRead,
/*	StateAction:		*/	PcsxrStateAction,
/*	Emulate:			*/	PcsxrEmulate,
/*	SetInput:			*/	PcsxrSetInput,
/*	DoSimpleCommand:	*/	PcsxrDoSimpleCommand,
/*	Settings:			*/	PcsxrSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	true,
/*	lcm_width:			*/	1024,
/*	lcm_height:			*/	512,
/*	dummy_separator:	*/	0,
/*	nominal_width:		*/	1024,
/*	nominal_height:		*/	512,
/*	fb_width:			*/	1024,
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
#define VERSION_FUNC pcsxGetVersion
#define GETEMU_FUNC pcsxGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC()
{
	return &PcsxrInfo;
}

