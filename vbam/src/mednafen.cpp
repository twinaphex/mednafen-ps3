#include <src/mednafen.h>
#include <src/git.h>
#include <src/driver.h>
#include <src/general.h>
#include <src/md5.h>
#include <src/mempatcher.h>

#define MODULENAMESPACE vbam
#include <module_helper.h>
using namespace vbam;

#include "Util.h"
#include "common/Port.h"
#include "common/Patch.h"
#include "gba/Globals.h"
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
namespace vbam
{
	EmulateSpecStruct*					ESpec;
	bool								GBAMode;

	uint32_t							SoundFrame;
	char	 							StateData[1024*512];
}
using namespace vbam;

//SYSTEM
void									InitSystem								();
void									sdlApplyPerImagePreferences				();
extern struct EmulatedSystem			emulator;

//Implement MDFNGI:
static int		VbamLoad				(const char *name, MDFNFILE *fp, bool aGBA)
{
	//Get Game MD5
	md5_context md5;
	md5.starts();
	md5.update(fp->data, fp->size);
	md5.finish(MDFNGameInfo->MD5);

	//Setup sound
	soundInit();

	//This refreshes timer and generates colormaps
	InitSystem();

	//Load and apply game settings
	GBAMode = aGBA;

	//Reset system
	if(aGBA)
	{
		//Load GBA game
		CPULoadRom(fp->data, fp->size);
		emulator = GBASystem;

		//Setup
		cpuSaveType = 0;
		flashSetSize(0x10000);
		rtcEnable(false);
		agbPrintEnable(false);
		mirroringEnable = false;

		sdlApplyPerImagePreferences();
		doMirroring(mirroringEnable);

		//Go
		CPUInit(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("vbamgba.bios").c_str()).c_str(), MDFN_GetSettingB("vbamgba.usebios"));

		//Map the memory for cheats
		MDFNMP_Init(0x8000, (1 << 28) / 0x8000);
		MDFNMP_AddRAM(0x40000, 0x2 << 24, workRAM);
		MDFNMP_AddRAM(0x08000, 0x3 << 24, internalRAM);

	}
	else
	{
		//Load dmg game
		gbLoadRom(fp->data, fp->size);
		emulator = GBSystem;

		//Grab settings
		gbEmulatorType = MDFN_GetSettingI("vbamdmg.emulatortype");
		gbColorOption = MDFN_GetSettingB("vbamdmg.washedcolors");
		gbSoundSetDeclicking(MDFN_GetSettingB("vbamdmg.declicking"));

		//Setup border mode
		int bordertype = MDFN_GetSettingI("vbamdmg.sgbborder");
		gbBorderOn = bordertype == 2;
		gbBorderAutomatic = bordertype == 0;

        gbBorderLineSkip = 256;
        gbBorderColumnSkip = 48;
        gbBorderRowSkip = 40;

		//Go
		gbCPUInit(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("vbamdmg.bios").c_str()).c_str(), MDFN_GetSettingB("vbamdmg.usebios"));
	}

	emulator.emuReset();
	emulating = 1;

	//Load save game
	std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
	emulator.emuReadBattery(filename.c_str());

	return 1;
}

static int		VbamLoadGBA				(const char *name, MDFNFILE *fp)
{
	return VbamLoad(name, fp, true);
}

static int		VbamLoadDMG				(const char *name, MDFNFILE *fp)
{
	return VbamLoad(name, fp, false);
}


static bool		VbamTestMagicGBA		(const char *name, MDFNFILE *fp)
{
	//TODO: Real magic number?
	return !strcasecmp(fp->ext, "gba") || !strcasecmp(fp->ext, "agb");
}

static bool		VbamTestMagicDMG		(const char *name, MDFNFILE *fp)
{
	static const uint8 GBMagic[8] = { 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B };
	return fp->size > 0x10C && !memcmp(fp->data + 0x104, GBMagic, 8);
}


static void		VbamCloseGame			(void)
{
	std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
	emulator.emuWriteBattery(filename.c_str());
	emulator.emuCleanUp();
	emulating = 0;
	MDFNMP_Kill();
}

static void		VbamEmulate				(EmulateSpecStruct *espec)
{
	ESpec = espec;

	//Update sound
	if(ESpec->SoundFormatChanged)
	{
		if(GBAMode)
		{
			soundSetSampleRate((ESpec->SoundRate > 1.0) ? ESpec->SoundRate : 22050);
		}
		else
		{
			gbSoundSetSampleRate((ESpec->SoundRate > 1.0) ? ESpec->SoundRate : 22050);		
		}
	}

	//Cheat
	MDFNMP_ApplyPeriodicCheats();

	//TODO: Support color shift
	systemFrameSkip = ESpec->skip;

	//Set the display size
	static const MDFN_Rect gba = {0, 0, 240, 160};
	static const MDFN_Rect dmgBorder = {0, 0, 256, 224};
	static const MDFN_Rect dmgNoBorder = {48, 40, 160, 144};

	if(GBAMode)
	{
		Video::SetDisplayRect(espec, 0, 0, 240, 160);
	}
	else if(gbBorderOn)
	{
		Video::SetDisplayRect(espec, 0, 0, 256, 224);
	}
	else
	{
		Video::SetDisplayRect(espec, 48, 40, 160, 144);
	}

	//Run the emulator, this will return at the end of a frame
	emulator.emuMain(1000000);

	//Copy the number of samples generated on this frame
	ESpec->SoundBufSize = (SoundFrame < 1000) ? SoundFrame : 0;
	SoundFrame = 0;

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

static int		VbamStateAction			(StateMem *sm, int load, int data_only)
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

static void		VbamSetInput			(int port, const char *type, void *ptr)
{
	Input::SetPort(port, (uint8_t*)ptr);
}

static void		VbamDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET || cmd == MDFN_MSC_POWER)
	{
		emulator.emuReset();
	}
}

//SYSTEM DESCRIPTIONS
//GBA
static const InputDeviceInputInfoStruct gbaIDII[] =
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

static InputDeviceInfoStruct			gbaInputDeviceInfo[] =
{
	{"gamepad",	"Gamepad",	NULL,		10,	gbaIDII}
};

static const InputPortInfoStruct		gbaPortInfo[] =
{
	{0,			"builtin",	"Built-In",	1,	gbaInputDeviceInfo,	"gamepad"}
};

static InputInfoStruct					gbaInput =
{
	1,			gbaPortInfo
};

static const FileExtensionSpecStruct	gbaExtensions[] =
{
	{".gba",	"Game Boy Advance Rom"	},
	{".agb",	"Game Boy Advance Rom"	},
	{0,			0						}
};


static MDFNSetting						gbaSettings[] =
{
	{"vbamgba.usebios",	MDFNSF_NOFLAGS,		"Enable GBA Bios Use",					NULL, MDFNST_BOOL,	"0"},
	{"vbamgba.bios",		MDFNSF_EMU_STATE,	"Path to optional GBA BIOS ROM image.",	NULL, MDFNST_STRING, "gbabios.bin"},
	{"vbamgba.vbaover",	MDFNSF_NOFLAGS,		"Path to vba-over.ini",					NULL, MDFNST_STRING, "vba-over.ini"},
	{0}
};

//TODO: MasterClock and fps
static MDFNGI							gbaInfo =
{
/*	shortname:			*/	"vbamgba",
/*	fullname:			*/	"Game Boy Advance (VBA-M)",
/*	FileExtensions:		*/	gbaExtensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&gbaInput,

/*	Load:				*/	VbamLoadGBA,
/*	TestMagic:			*/	VbamTestMagicGBA,
/*	LoadCD:				*/	0,
/*	TestMagicCD:		*/	0,
/*	CloseGame:			*/	VbamCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
/*	StateAction:		*/	VbamStateAction,
/*	Emulate:			*/	VbamEmulate,
/*	SetInput:			*/	VbamSetInput,
/*	DoSimpleCommand:	*/	VbamDoSimpleCommand,
/*	Settings:			*/	gbaSettings,
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

//DMG
static const InputDeviceInputInfoStruct dmgIDII[] =
{
	{"a",				"A",			7,	IDIT_BUTTON_CAN_RAPID,	NULL},
	{"b",				"B",			6,	IDIT_BUTTON_CAN_RAPID,	NULL},
	{"select",			"SELECT",		4,	IDIT_BUTTON,			NULL},
	{"start",			"START",		5,	IDIT_BUTTON,			NULL},
	{"right",			"RIGHT",		3,	IDIT_BUTTON,			"left"},
	{"left",			"LEFT",			2,	IDIT_BUTTON,			"right"},
	{"up",				"UP",			0,	IDIT_BUTTON,			"down"},
	{"down",			"DOWN",			1,	IDIT_BUTTON,			"up"},
};

static InputDeviceInfoStruct			dmgInputDeviceInfo[] =
{
	{"gamepad",	"Gamepad",	NULL,		8,	dmgIDII}
};

static const InputPortInfoStruct		dmgPortInfo[] =
{
	{0,			"builtin",	"Built-In",	1,	dmgInputDeviceInfo,	"gamepad"}
};

static InputInfoStruct					dmgInput =
{
	1,			dmgPortInfo
};

static FileExtensionSpecStruct			dmgExtensions[] = 
{
	{".gb",		"Game Boy Rom"},
	{".gbc",	"Game Boy Color Rom"},
	{".cgb",	"Game Boy Color Rom"},
	{0,			0}
};

const MDFNSetting_EnumList				dmgEmulatorTypeList[] =
{
	{"auto",		0,		"Autodetect",		""},
	{"gbc",			1,		"GameBoy Color",	""},
	{"sgb",			2,		"Super GameBoy",	""},
	{"dmg",			3,		"GameBoy Mono",		""},
	{"gba",			4,		"GameBoy Advance",	""},
	{"sgb2",		5,		"Super GameBoy 2",	""},
	{0,				0,		0,					0}
};

const MDFNSetting_EnumList				dmgBorderModeList[] =
{
	{"auto",		0,		"Automatic",				""},
	{"never",		1,		"Never display the border",	""},
	{"always",		2,		"Always show the border",	""},
	{0,				0,		0,							0}
};

static MDFNSetting						dmgSettings[] =
{
	{"vbamdmg.usebios",			MDFNSF_NOFLAGS,		"Enable DMG Bios Use",					NULL,	MDFNST_BOOL,	"0"},
	{"vbamdmg.bios",			MDFNSF_EMU_STATE,	"Path to optional DMG BIOS ROM image.",	NULL,	MDFNST_STRING,	"gbbios.bin"},
	{"vbamdmg.emulatortype",	MDFNSF_NOFLAGS,		"Type of gameboy machine to emulate",	NULL,	MDFNST_ENUM,	"auto",	0, 0, 0, 0,	dmgEmulatorTypeList},
	{"vbamdmg.sgbborder",		MDFNSF_NOFLAGS,		"Show border image.",					NULL,	MDFNST_ENUM,	"auto",	0, 0, 0, 0, dmgBorderModeList},
	{"vbamdmg.washedcolors",	MDFNSF_NOFLAGS,		"Enable washed colors.",				NULL,	MDFNST_BOOL,	"1"},
	{"vbamdmg.declicking",		MDFNSF_NOFLAGS,		"Enable Sound Declicking.", 			NULL,	MDFNST_BOOL,	"1"},
	{0}
};

static MDFNGI							dmgInfo =
{
/*	shortname:			*/	"vbamdmg",
/*	fullname:			*/	"Game Boy (Color) (VBA-M)",
/*	FileExtensions:		*/	dmgExtensions,
/*	ModulePriority:		*/	(ModPrio)(MODPRIO_EXTERNAL_HIGH + 1),
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&dmgInput,

/*	Load:				*/	VbamLoadDMG,
/*	TestMagic:			*/	VbamTestMagicDMG,
/*	LoadCD:				*/	0,
/*	TestMagicCD:		*/	0,
/*	CloseGame:			*/	VbamCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
/*	StateAction:		*/	VbamStateAction,
/*	Emulate:			*/	VbamEmulate,
/*	SetInput:			*/	VbamSetInput,
/*	DoSimpleCommand:	*/	VbamDoSimpleCommand,
/*	Settings:			*/	dmgSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	false,
/*	lcm_width:			*/	256,
/*	lcm_height:			*/	224,
/*  dummy_separator:	*/	0,
/*	nominal_width:		*/	256,
/*	nominal_height:		*/	224,
/*	fb_width:			*/	256,
/*	fb_height:			*/	224,
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
#define VERSION_FUNC vbamGetVersion
#define GETEMU_FUNC vbamGetEmulator
#define	DLL_PUBLIC
#endif


extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	if(aIndex == 0) return &gbaInfo;
	if(aIndex == 1) return &dmgInfo;
	return 0;
}

