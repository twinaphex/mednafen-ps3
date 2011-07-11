//Includes from mednafen
#include <src/mednafen.h>
#include <src/cdrom/cdromif.h>
#include <src/mednafen-driver.h>
#include <src/git.h>
#include <include/Fir_Resampler.h>
#include <src/general.h>
#include <stdarg.h>
#include "config.h"

#include "src/mempatcher.h"


namespace
{
	uint8_t*					Ports[8];
	Fir_Resampler<8>			Resampler;				///<The sound plugin only gives 44100hz sound
	int16_t						SampleBuffer[48000];	///<TODO: Support sound freqs > 48000
}

//Pair of functions to export timing to C files.
//TODO: These shouldn't be needed as the emulator module shouldn't be doing timing.
extern "C" void MDFNDC_Sleep(uint32_t aMS){MDFND_Sleep(aMS);}
extern "C" uint32_t MDFNDC_GetTime(){return MDFND_GetTime();}

//Definitions for PCSX
extern "C"
{
	//Pointer to the PSX's memory, for cheats
	extern int8_t*		psxM;


	//Return the filname of a memory card
	const char*			GetMemoryCardName		(int aSlot)
	{
		static char slots[2][MAXPATHLEN];

		snprintf(slots[0], MAXPATHLEN, "%s", MDFN_MakeFName(MDFNMKF_SAV, 0, "sav").c_str());
		snprintf(slots[1], MAXPATHLEN, "%s", MDFN_MakeFName(MDFNMKF_SAV, 0, "sav2").c_str());

		return slots[(aSlot == 0) ? 0 : 1];
	}

	//Return the filename of the BIOS
	const char*			GetBiosName				()
	{
		static char name[MAXPATHLEN];

		snprintf(name, MAXPATHLEN, "%s", MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("pcsxr.bios").c_str()).c_str());

		return name;
	}

	//Return the value of the recompiler setting
	int32_t				GetRecompiler			()
	{
		return MDFN_GetSettingB("pcsxr.recompiler");
	}

	//From main.c
	void				SysLoad					();
	void				SysClose				();
	void				SysReset				();
	void				SysFrame				(uint32_t aSkip, uint32_t* aPixels, uint32_t aPitch, uint32_t aInputPort1, uint32_t aInputPort2, uint32_t* aWidth, uint32_t* aHeight, uint32_t* aSound, uint32_t* aSoundLen);

	//Printing functions needed by libpcsxcore
	void				SysPrintf				(const char *fmt, ...)
	{
		char buffer[2048];
		va_list args;
		va_start (args, fmt);
		vsnprintf(buffer, 2048, fmt, args);
		va_end (args);

		MDFND_Message(buffer);
	}

	void				SysMessage				(const char *fmt, ...)
	{
		char buffer[2048];
		va_list args;
		va_start (args, fmt);
		vsnprintf(buffer, 2048, fmt, args);
		va_end (args);

		MDFND_Message(buffer);
	}
}

//Implement MDFNGI:
int				PcsxrLoad				()
{
	//Initialize the resampler
	Resampler.buffer_size(588 * 2 * 2 + 100);
	Resampler.time_ratio((double)44100 / 48000.0, 0.9965);

	//Call the C function to finish loading
	//TODO: Return error if SysLoad fails.
	SysLoad();

	//TODO: Set the clock if the machine is PAL

	//Just say two 1M pages, for fun not profit
	MDFNMP_Init(1024 * 1024, 2);
	MDFNMP_AddRAM(1024 * 1024 * 2, 0, (uint8_t*)psxM);

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
	//Call the C function in main.c
	SysClose();

	//Close the cheat engine
	MDFNMP_Kill();
}

//FAKE ZLIB HACK
static StateMem* stateMemory;

extern "C"
{
	int SaveState(const char *file);
	int LoadState(const char *file);

	typedef void* smFile;
	smFile smopen (const char *path , const char *mode )
	{
		if(!stateMemory)
		{
			MDFND_PrintError("pcsxr: Using save state functions without a memory object?\n");
		}

		return (void*)stateMemory;
	}

	int smclose (smFile file )
	{
		if(!stateMemory)
		{
			MDFND_PrintError("pcsxr: Using save state functions without a memory object?\n");
		}

		stateMemory = 0;
	}

	off_t smseek(smFile file, off_t offset, int whence)
	{
		StateMem* mem = (StateMem*)file;
		return smem_seek(mem, offset, whence);
	}

	int smwrite (smFile file, void* buf, unsigned int len)
	{
		StateMem* mem = (StateMem*)file;
		smem_write(mem, buf, len);
		return len;
	}

	int smread (smFile file, void* buf, unsigned int len)
	{
		StateMem* mem = (StateMem*)file;
		smem_read(mem, buf, len);
		return len;
	}
};


int				PcsxrStateAction		(StateMem *sm, int load, int data_only)
{
	//TODO:
	if(!load)
	{
		stateMemory = sm;
		int result = SaveState("");
		stateMemory = 0;
		return result == 0;
	}
	else
	{
		stateMemory = sm;
		int result = LoadState("");
		stateMemory = 0;
		return result == 0;
	}
}

void			PcsxrEmulate			(EmulateSpecStruct *espec)
{
    if(espec->SoundFormatChanged)
    {
		//TODO
    }

	//Apply cheats
	MDFNMP_ApplyPeriodicCheats();

	//Fetch the list of pressed buttons
	uint32_t input1 = Ports[0] ? (Ports[0][0] | (Ports[0][1] << 8)) : 0;
	uint32_t input2 = Ports[1] ? (Ports[1][0] | (Ports[1][1] << 8)) : 0;

	//Call the C function to emulate the frame
	//TODO: Support multiplayer
    //TODO: Support color shift
	uint32_t width, height;
	uint32_t sndsize;
	SysFrame(espec->skip, espec->surface->pixels, espec->surface->pitch32, input1, input2, &width, &height, (uint32_t*)SampleBuffer, &sndsize);

	//Resample the audio if needed
	//TODO: Why only on sndsize < 1500, that seems wrong somehow...
	if(sndsize < 1500 && espec->SoundBuf)
	{
		memcpy(Resampler.buffer(), SampleBuffer, sndsize * 4);
		Resampler.write(sndsize * 2);
		espec->SoundBufSize = Resampler.read(espec->SoundBuf, Resampler.avail()) >> 1;
	}

	//Set the output size
    espec->DisplayRect.x = 0;
    espec->DisplayRect.y = 0;
    espec->DisplayRect.w = width;
    espec->DisplayRect.h = height;

	//Update timing
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
	{0, "port2", "Port 2", 2, InputDeviceInfoPSXPort, "gamepad"},
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
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
/*	StateAction:		*/	PcsxrStateAction,
/*	Emulate:			*/	PcsxrEmulate,
/*	SetInput:			*/	PcsxrSetInput,
/*	DoSimpleCommand:	*/	PcsxrDoSimpleCommand,
/*	Settings:			*/	PcsxrSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	true,
/*	lcm_width:			*/	1280,	//?
/*	lcm_height:			*/	960,	//?
/*	dummy_separator:	*/	0,
/*	nominal_width:		*/	640,	//?
/*	nominal_height:		*/	480,	//?
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

