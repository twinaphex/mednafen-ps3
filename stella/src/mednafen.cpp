#include <src/mednafen.h>
#include <src/git.h>

#include "Settings.hxx"
#include "TIA.hxx"
#include "Props.hxx"
#include "Cart.hxx"
#include "Console.hxx"
#include "Serializer.hxx"

Settings	stellaSettings(0);	//TODO:<
Properties	stellaProperties;
Console*	stellaConsole;

int				StellaLoad				(const char *name, MDFNFILE *fp)
{
	try
	{
		//Load the cart
		string cartType = "AUTO-DETECT";
		string cartID = "";			//?
		string cartMD5 = "";		//?
		Cartridge* stellaCart = Cartridge::create(fp->data, fp->size, cartMD5, cartType, cartID, stellaSettings);

		if(stellaCart == 0)
		{
			MDFND_PrintError("Stella: Failed to load cartridge.");
			return 0;
		}

		//Create the console
		stellaConsole = new Console(0, stellaCart, stellaProperties);
		return 1;
	}
	catch(...)
	{
		MDFND_PrintError("Stella: Failed to load due to exception.");
		return 0;
	}
}

bool			StellaTestMagic			(const char *name, MDFNFILE *fp)
{
	//There is no reliable way to detect a 2600 game, so just go by extension
	return true;
}

void			StellaCloseGame			(void)
{
	//TODO: Is anything else needed here?

	delete stellaConsole;
	stellaConsole = 0;
}

void			StellaInstallReadPatch	(uint32 address)
{
	//TODO:
}

void			StellaRemoveReadPatches	(void)
{
	//TODO:
}

uint8			StellaMemRead			(uint32 addr)
{
	//TODO:
	return 0;
}

int				StellaStateAction		(StateMem *sm, int load, int data_only)
{
	try
	{
		if(load)
		{
			Serializer state(sm);
			stellaConsole->load(state);
		}
		else
		{
			Serializer state(sm);
			stellaConsole->save(state);
		}

		return 1;
	}
	catch(...)
	{
		return 0;
	}
}

void			StellaEmulate			(EmulateSpecStruct *espec)
{
	// Run the console for one frame
	stellaConsole->tia().update();

//	if(myOSystem->eventHandler().frying())
//		myOSystem->console().fry();

	// And update the screen
	uInt8* currentFrame = stellaConsole->tia().currentFrameBuffer();
	uInt32 width  = stellaConsole->tia().width();
	uInt32 height = stellaConsole->tia().height();

	espec->DisplayRect.x = 0;
	espec->DisplayRect.y = 0;
	espec->DisplayRect.w = width;
	espec->DisplayRect.h = height;

	for(int i = 0; i != height; i ++)
	{
		for(int j = 0; j != width; j ++)
		{
			espec->surface->pixels[i * 640 + j] = currentFrame[i * width + j] << 8;
		}
	}

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

void			StellaSetInput			(int port, const char *type, void *ptr)
{
	//TODO:
}

void			StellaDoSimpleCommand	(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		stellaConsole->system().reset();
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		stellaConsole->system().reset();
	}
}

static const InputDeviceInputInfoStruct GamepadIDII[] =
{
	{"button",	"BUTTON",	0,	IDIT_BUTTON, NULL},
};

static InputDeviceInfoStruct InputDeviceInfoPort[] =
{
	{"none",	"none",		NULL,	0,	NULL},
	{"gamepad", "Gamepad",	NULL,	1,	GamepadIDII},
};


static const InputPortInfoStruct PortInfo[] =
{
	{0, "port1", "Port 1", 2, InputDeviceInfoPort, "gamepad"},
	{0, "port2", "Port 2", 2, InputDeviceInfoPort, "gamepad"},
};

InputInfoStruct		StellaInput =
{
	1,
	PortInfo
};


static FileExtensionSpecStruct	extensions[] = 
{
	{".a26", "Atari 2600 ROM"},
	{0, 0}
};


static MDFNSetting StellaSettings[] =
{
	{NULL}
};


MDFNGI	StellaInfo = 
{
/*	shortname:			*/	"stella",
/*	fullname:			*/	"Stella (Atari 2600)",
/*	FileExtensions:		*/	extensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&StellaInput,

/*	Load:				*/	StellaLoad,
/*	TestMagic:			*/	StellaTestMagic,
/*	LoadCD:				*/	0,
/*	TestMagicCD:		*/	0,
/*	CloseGame:			*/	StellaCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	StellaInstallReadPatch,
/*	RemoveReadPatches:	*/	StellaRemoveReadPatches,
/*	MemRead:			*/	StellaMemRead,
/*	StateAction:		*/	StellaStateAction,
/*	Emulate:			*/	StellaEmulate,
/*	SetInput:			*/	StellaSetInput,
/*	DoSimpleCommand:	*/	StellaDoSimpleCommand,
/*	Settings:			*/	StellaSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	true,
/*	lcm_width:			*/	640,	//?
/*	lcm_height:			*/	480,	//?
/*	dummy_separator:	*/	0,
/*	nominal_width:		*/	640,	//?
/*	nominal_height:		*/	480,	//?
/*	fb_width:			*/	640,
/*	fb_height:			*/	480,
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
#define VERSION_FUNC stellaGetVersion
#define GETEMU_FUNC stellaGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC()
{
	return &StellaInfo;
}

