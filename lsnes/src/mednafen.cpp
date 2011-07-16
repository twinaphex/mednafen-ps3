#include <src/mednafen.h>
#include <src/git.h>
#include <src/driver.h>
#include <src/general.h>
#include <src/mempatcher.h>

#include "libsnes.hpp"


//MEDNAFEN
namespace mdfn
{
	EmulateSpecStruct*					ESpec;

	uint8_t*							Ports[4];
	uint32_t							SoundFrame;
}
using namespace mdfn;

//SYSTEM
namespace lsnes
{
	void			VideoRefreshCallback	(const uint16_t *data, unsigned width, unsigned height)
	{
		assert(data && width && height);
		assert(ESpec && ESpec->surface && ESpec->surface->pixels);

		uint32_t* destimage = (uint32_t*)ESpec->surface->pixels;

		for(int i = 0; i != height && i != ESpec->surface->h; i ++)
		{
			for(int j = 0; j != width && j != ESpec->surface->w; j ++)
			{
				uint16_t source = data[i * width * 4 + j];
				uint8_t a = (source & 0x3F) << 3;
				uint8_t b = ((source >> 5) & 0x3F) << 3;
				uint8_t c = ((source >> 10) & 0x3F) << 3;
				destimage[i * ESpec->surface->pitchinpix + j] = a | b << 8 | c << 16;
			}
		}	

		ESpec->DisplayRect.x = 0;
		ESpec->DisplayRect.y = 0;
		ESpec->DisplayRect.w = width;
		ESpec->DisplayRect.h = height;
	}

	void			AudioSampleCallback		(uint16_t left, uint16_t right)
	{

	}

	void			InputPollCallback		()
	{

	}

	int16_t			InputStateCallback		(bool port, unsigned device, unsigned index, unsigned id)
	{
		return 0;
	}
}
using namespace lsnes;

//Implement MDFNGI:
int				lsnesLoad				(const char *name, MDFNFILE *fp)
{
	snes_init();
	snes_set_video_refresh(VideoRefreshCallback);
	snes_set_audio_sample(AudioSampleCallback);
	snes_set_input_poll(InputPollCallback);
	snes_set_input_state(InputStateCallback);

	snes_load_cartridge_normal(0, fp->data, fp->size);
	snes_power();

	return 1;
}

bool			lsnesTestMagic			(const char *name, MDFNFILE *fp)
{
	//Straight from mednafen
	return !(strcasecmp(fp->ext, "smc") && strcasecmp(fp->ext, "swc") && strcasecmp(fp->ext, "sfc") && strcasecmp(fp->ext, "fig") && strcasecmp(fp->ext, "bs") && strcasecmp(fp->ext, "st"));
}

void			lsnesCloseGame			(void)
{
	snes_unload_cartridge();
	snes_term();
}


void			lsnesEmulate			(EmulateSpecStruct *espec)
{
	ESpec = espec;

	//Update sound
//	if(ESpec->SoundFormatChanged)
//	{
//	}

	//Cheat
//	MDFNMP_ApplyPeriodicCheats();

	snes_run();

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

void			lsnesSetInput			(int port, const char *type, void *ptr)
{
	if(port >= 0 && port <= 4)
	{
		Ports[port] = (uint8_t*)ptr;
	}
}

void			lsnesDoSimpleCommand	(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		snes_reset();
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		snes_power();
	}
}


//STUBS
int				lsnesStateAction		(StateMem *sm, int load, int data_only)
{
	return 0;
}



//SYSTEM DESCRIPTIONS
static const InputDeviceInputInfoStruct IDII[] =
{
	{ "b",		"B (center, lower)",	7,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "y",		"Y (left)",				6,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "select",	"SELECT",				4,	IDIT_BUTTON,			NULL	},
	{ "start",	"START",				5,	IDIT_BUTTON,			NULL	},
	{ "up",		"UP ↑",					0,	IDIT_BUTTON,			"down"	},
	{ "down",	"DOWN ↓",				1,	IDIT_BUTTON,			"up"	},
	{ "left",	"LEFT ←",				2,	IDIT_BUTTON,			"right"	},
	{ "right",	"RIGHT →",				3,	IDIT_BUTTON,			"left"	},
	{ "a",		"A (right)",			9,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "x",		"X (center, upper)",	8,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "l",		"Left Shoulder",		10,	IDIT_BUTTON,			NULL	},
	{ "r",		"Right Shoulder",		11,	IDIT_BUTTON,			NULL	},
};

static InputDeviceInfoStruct InputDeviceInfo[] =
{
	{"none", "none", NULL, 0, NULL},
	{"gamepad", "Gamepad", NULL, sizeof(IDII) / sizeof(InputDeviceInputInfoStruct), IDII,},
};

static const InputPortInfoStruct PortInfo[] =
{
	{0, "port1", "Port 1", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" },
};

static InputInfoStruct lsnesInput =
{
	sizeof(PortInfo) / sizeof(InputPortInfoStruct), PortInfo
};

static MDFNSetting lsnesSettings[] =
{
	{NULL}
};


static FileExtensionSpecStruct extensions[] =
{
	{".smc", "Super Magicom ROM Image"},
	{".swc", "Super Wildcard ROM Image"},
	{".sfc", "Cartridge ROM Image"},
	{".fig", "Cartridge ROM Image"},

	{".bs", "BS-X EEPROM Image"},
	{".st", "Sufami Turbo Cartridge ROM Image"},

	{NULL, NULL}
};

//TODO: MasterClock and fps
static MDFNGI	lsnesInfo =
{
/*	shortname:			*/	"lsnes",
/*	fullname:			*/	"libsnes Wrapper",
/*	FileExtensions:		*/	extensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&lsnesInput,

/*	Load:				*/	lsnesLoad,
/*	TestMagic:			*/	lsnesTestMagic,
/*	LoadCD:				*/	0,
/*	TestMagicCD:		*/	0,
/*	CloseGame:			*/	lsnesCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
/*	StateAction:		*/	lsnesStateAction,
/*	Emulate:			*/	lsnesEmulate,
/*	SetInput:			*/	lsnesSetInput,
/*	DoSimpleCommand:	*/	lsnesDoSimpleCommand,
/*	Settings:			*/	lsnesSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	false,
/*	lcm_width:			*/	512,
/*	lcm_height:			*/	480,
/*  dummy_separator:	*/	0,
/*	nominal_width:		*/	256,
/*	nominal_height:		*/	240,
/*	fb_width:			*/	512,
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
#define VERSION_FUNC lsnesGetVersion
#define GETEMU_FUNC lsnesGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC()
{
	return &lsnesInfo;
}

