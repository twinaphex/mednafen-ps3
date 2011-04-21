#include <src/mednafen.h>
#include <src/git.h>
#include <src/driver.h>
#include <src/general.h>

#include "Util.h"
#include "common/Port.h"
#include "common/Patch.h"
#include "gba/Flash.h"
#include "gba/RTC.h"
#include "gba/Sound.h"
#include "gba/Cheats.h"
#include "gba/GBA.h"
#include "gba/agbprint.h"
#include "gb/gb.h"
#include "gb/gbGlobals.h"
#include "gb/gbCheats.h"
#include "gb/gbSound.h"
#include "common/SoundDriver.h"

//MEDNAFEN
namespace mdfn
{
	EmulateSpecStruct*					ESpec;

	bool								GameLoaded = false;

	uint8_t*							Ports[4];
	uint32_t							SoundFrame;
}
using namespace mdfn;

//SYSTEM
void								InitSystem								();
void								sdlApplyPerImagePreferences				();
extern struct EmulatedSystem		emulator;


int				VbamLoad				(const char *name, MDFNFILE *fp);
bool			VbamTestMagic			(const char *name, MDFNFILE *fp);
void			VbamCloseGame			(void);
void			VbamEmulate				(EmulateSpecStruct *espec);
void			VbamSetInput			(int port, const char *type, void *ptr);
void			VbamDoSimpleCommand		(int cmd);
void			VbamInstallReadPatch	(uint32 address);
void			VbamRemoveReadPatches	(void);
uint8			VbamMemRead				(uint32 addr);
int				VbamStateAction			(StateMem *sm, int load, int data_only);

int				VbamLoad				(const char *name, MDFNFILE *fp)
{
	if(GameLoaded)
	{
		VbamCloseGame();
	}

	//Setup sound
	soundInit();
	soundSetSampleRate(48000);

	//Load and apply game settings
	//TODO: Support GB and GBC
	CPULoadRom(fp->data, fp->size);
	emulator = GBASystem;

	cpuSaveType = 0;
	flashSetSize(0x10000);
	rtcEnable(false);
	agbPrintEnable(false);
	mirroringEnable = false;

	sdlApplyPerImagePreferences();
	doMirroring(mirroringEnable);

	//Reset system
	soundReset();
	CPUInit(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("vbam.bios").c_str()).c_str(), MDFN_GetSettingB("vbam.usebios"));
	CPUReset();
	emulating = 1;

	//Load save game
	std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
	emulator.emuReadBattery(filename.c_str());

	//This refreshes timer and generates colormaps
	InitSystem();

	GameLoaded = true;

	return 1;
}

bool			VbamTestMagic			(const char *name, MDFNFILE *fp)
{
	//TODO: Real magic number?
	return !strcasecmp(fp->ext, "gba") || !strcasecmp(fp->ext, "agb");
}

void			VbamCloseGame			(void)
{
	if(GameLoaded)
	{
		std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
		emulator.emuWriteBattery(filename.c_str());

		emulator.emuCleanUp();
	}

	emulating = 0;
	GameLoaded = false;
}


void			VbamEmulate				(EmulateSpecStruct *espec)
{
	ESpec = espec;

	//Update sound
	if(ESpec->SoundFormatChanged)
	{
		soundSetSampleRate(ESpec->SoundRate);
	}

	//TODO: Support color shift
	systemFrameSkip = ESpec->skip;

	//Set the display size
	espec->DisplayRect.x = 0;
	espec->DisplayRect.y = 0;
	espec->DisplayRect.w = 240;
	espec->DisplayRect.h = 160;

	//Run the emulator, this will return at the end of a frame
	emulator.emuMain(1000000);

	//Copy the number of samples generated on this frame
	ESpec->SoundBufSize = SoundFrame;
	SoundFrame = 0;

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

void			VbamSetInput			(int port, const char *type, void *ptr)
{
	if(port == 0)
	{
		Ports[port] = (uint8_t*)ptr;
	}
}

void			VbamDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		CPUReset();
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		CPUReset();
	}
}


//STUBS
void			VbamInstallReadPatch	(uint32 address)							{}
void			VbamRemoveReadPatches	(void)										{}
uint8			VbamMemRead				(uint32 addr)								{return 0;}

char	 		StateData[1024*512];
int				VbamStateAction			(StateMem *sm, int load, int data_only)
{
	if(!load)
	{
		emulator.emuWriteMemState(StateData, 512 * 1024);

		smem_write32le(sm, 512 * 1024);
		smem_write(sm, StateData, 512 * 1024);

		return 1;
	}
	else
	{
		uint32_t size;
		smem_read32le(sm, &size);
		smem_read(sm, StateData, size);

		emulator.emuReadMemState(StateData, 512 * 1024);

		return 1;
	}

	return 0;
}



//SYSTEM DESCRIPTIONS
static const InputDeviceInputInfoStruct IDII[] =
{
	{"a",				"A",			7,	IDIT_BUTTON_CAN_RAPID,	NULL},
	{"b",				"B",			6,	IDIT_BUTTON_CAN_RAPID,	NULL},
	{"select",			"SELECT",		4,	IDIT_BUTTON,			NULL},
	{"start",			"START",		5,	IDIT_BUTTON,			NULL},
	{"right",			"RIGHT",		3,	IDIT_BUTTON,			"left"},
	{"left",			"LEFT",			2,	IDIT_BUTTON,			"right"},
	{"up",				"UP",			0,	IDIT_BUTTON,			"down"},
	{"down",			"DOWN",			1,	IDIT_BUTTON,			"up"},
	{"shoulder_r",		"SHOULDER R",	9,	IDIT_BUTTON,			NULL},
	{"shoulder_l",		"SHOULDER L",	8,	IDIT_BUTTON,			NULL},
};

static InputDeviceInfoStruct InputDeviceInfo[] =
{
	{"gamepad",	"Gamepad",	NULL,	sizeof(IDII) / sizeof(InputDeviceInputInfoStruct),	IDII}
};

static const InputPortInfoStruct PortInfo[] =
{
	{0, "builtin", "Built-In", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad"}
};

static InputInfoStruct VbamInput =
{
	sizeof(PortInfo) / sizeof(InputPortInfoStruct), PortInfo
};

static FileExtensionSpecStruct	extensions[] =
{
	{".gba", "Game Boy Advance Rom"},
	{".agb", "Game Boy Advance Rom"},
	{0, 0}
};


static MDFNSetting VbamSettings[] =
{
	{"vbam.usebios",	MDFNSF_NOFLAGS,		"Enable GBA Bios Use",					NULL, MDFNST_BOOL,	"0"},
	{"vbam.bios",		MDFNSF_EMU_STATE,	"Path to optional GBA BIOS ROM image.",	NULL, MDFNST_STRING, "gbabios.bin"},
	{"vbam.vbaover",	MDFNSF_NOFLAGS,		"Path to vba-over.ini",					NULL, MDFNST_STRING, "vba-over.ini"},
	{0}
};


//TODO: MasterClock and fps
static MDFNGI	VbamInfo =
{
/*	shortname:			*/	"vbam",
/*	fullname:			*/	"Game Boy Advance (VBA-M)",
/*	FileExtensions:		*/	extensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&VbamInput,

/*	Load:				*/	VbamLoad,
/*	TestMagic:			*/	VbamTestMagic,
/*	LoadCD:				*/	0,
/*	TestMagicCD:		*/	0,
/*	CloseGame:			*/	VbamCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	VbamInstallReadPatch,
/*	RemoveReadPatches:	*/	VbamRemoveReadPatches,
/*	MemRead:			*/	VbamMemRead,
/*	StateAction:		*/	VbamStateAction,
/*	Emulate:			*/	VbamEmulate,
/*	SetInput:			*/	VbamSetInput,
/*	DoSimpleCommand:	*/	VbamDoSimpleCommand,
/*	Settings:			*/	VbamSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	false,
/*	lcm_width:			*/	240,
/*	lcm_height:			*/	160,
/*  dummy_separator:	*/	0,
/*	nominal_width:		*/	240,
/*	nominal_height:		*/	160,
/*	fb_width:			*/	240,
/*	fb_height:			*/	160,
/*	soundchan:			*/	2
};


#ifdef MLDLL
#ifdef __WIN32__
#define DLL_PUBLIC __attribute__((dllexport))
#else
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#endif
#else
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		GetVersion()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GetEmulator()
{
	return &VbamInfo;
}

