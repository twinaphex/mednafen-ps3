#include <src/mednafen.h>
#include <src/git.h>

#include "Settings.hxx"
#include "TIA.hxx"
#include "Props.hxx"
#include "PropsSet.hxx"
#include "Cart.hxx"
#include "Console.hxx"
#include "Serializer.hxx"
#include "Event.hxx"
#include "Switches.hxx"
#include "MD5.hxx"

//Function to receive emulator palette
const uInt32* stellaPalette;
void stellaSetPalette(const uInt32* palette)
{
	stellaPalette = palette;
}


Settings		stellaSettings;
Console*		stellaConsole;
uint8_t*		stellaPort[2];
uint32_t		stellaPortType[2];

static struct
{
	const char*			Name;
	uint32_t			Port;
	uint32_t			Count;
	Event::Type			IDs[32];
}	stellaInputDefs[] = 
{
	"none", 0, 0, {},
	"none", 1, 0, {},
	"gamepad", 0, 9, {Event::JoystickZeroUp, Event::JoystickZeroDown, Event::JoystickZeroLeft, Event::JoystickZeroRight, Event::JoystickZeroFire1, Event::JoystickZeroFire2, Event::JoystickZeroFire3, Event::ConsoleSelect, Event::ConsoleReset},
	"gamepad", 1, 9, {Event::JoystickOneUp, Event::JoystickOneDown, Event::JoystickOneLeft, Event::JoystickOneRight, Event::JoystickOneFire1, Event::JoystickOneFire2, Event::JoystickOneFire3, Event::ConsoleSelect, Event::ConsoleReset},
	"paddle", 0, 8, {Event::PaddleZeroDecrease, Event::PaddleZeroIncrease, Event::PaddleZeroFire, Event::PaddleOneDecrease, Event::PaddleOneIncrease, Event::PaddleOneFire, Event::ConsoleSelect, Event::ConsoleReset},
	"paddle", 1, 8, {Event::PaddleTwoDecrease, Event::PaddleTwoIncrease, Event::PaddleTwoFire, Event::PaddleThreeDecrease, Event::PaddleThreeIncrease, Event::PaddleThreeFire, Event::ConsoleSelect, Event::ConsoleReset},
	"keyboard", 0, 14, {Event::KeyboardZero1, Event::KeyboardZero2, Event::KeyboardZero3, Event::KeyboardZero4, Event::KeyboardZero5, Event::KeyboardZero6, Event::KeyboardZero7, Event::KeyboardZero8, Event::KeyboardZero9, Event::KeyboardZeroStar, Event::KeyboardZero0, Event::KeyboardZeroPound, Event::ConsoleSelect, Event::ConsoleReset},
	"keyboard", 1, 14, {Event::KeyboardOne1, Event::KeyboardOne2, Event::KeyboardOne3, Event::KeyboardOne4, Event::KeyboardOne5, Event::KeyboardOne6, Event::KeyboardOne7, Event::KeyboardOne8, Event::KeyboardOne9, Event::KeyboardOneStar, Event::KeyboardOne0, Event::KeyboardOnePound, Event::ConsoleSelect, Event::ConsoleReset},
};


int				StellaLoad				(const char *name, MDFNFILE *fp)
{
	try
	{
		//Get the cart's MD5 sum
		string cartMD5 = MD5(fp->data, fp->size);

		//Get the game properties
		PropertiesSet propslist(0);
		Properties gameProperties;
		propslist.getMD5(cartMD5, gameProperties);

		//Load the cart
		string cartType = gameProperties.get(Cartridge_Type);
		string cartID = "";
		Cartridge* stellaCart = Cartridge::create(fp->data, fp->size, cartMD5, cartType, cartID, stellaSettings);

		if(stellaCart == 0)
		{
			MDFND_PrintError("Stella: Failed to load cartridge.");
			return 0;
		}

		//Create the console
		stellaConsole = new Console(0, stellaCart, gameProperties);
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

	stellaPort[0] = stellaPort[1] = 0;
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
	//Update the input
	for(int i = 0; i != 2; i ++)
	{
		if(stellaPort[i])
		{
			uint32_t inputState = stellaPort[i][0] | (stellaPort[i][1] << 8);
			for(int j = 0; j != stellaInputDefs[stellaPortType[i]].Count - ((i == 1) ? 2 : 0); j ++, inputState >>= 1)
			{
				stellaConsole->event().set(stellaInputDefs[stellaPortType[i]].IDs[j], inputState & 1);
			}
		}
	}

	stellaConsole->switches().update();
	stellaConsole->controller(Controller::Left).update();
	stellaConsole->controller(Controller::Right).update();


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
			espec->surface->pixels[i * 640 + j] = stellaPalette ? stellaPalette[currentFrame[i * width + j]] : 0;
		}
	}

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

void			StellaSetInput			(int port, const char *type, void *ptr)
{
	if(port >= 0 && port < 2)
	{
		stellaPort[port] = (uint8_t*)ptr;

		for(int i = 0; i != sizeof(stellaInputDefs) / sizeof(stellaInputDefs[0]); i ++)
		{
			if(strcmp(stellaInputDefs[i].Name, type) == 0 && stellaInputDefs[i].Port == port)
			{
				stellaPortType[port] = i;
				return;
			}
		}

		stellaPortType[port] = 0;
	}
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
	{"up",		"Up",					0,	IDIT_BUTTON, NULL},
	{"down",	"Down",					1,	IDIT_BUTTON, NULL},
	{"left",	"Left",					2,	IDIT_BUTTON, NULL},
	{"right",	"Right",				3,	IDIT_BUTTON, NULL},
	{"fire1",	"Fire1",				4,	IDIT_BUTTON, NULL},
	{"fire2",	"Fire2",				5,	IDIT_BUTTON, NULL},
	{"fire3",	"Fire3",				6,	IDIT_BUTTON, NULL},
	{"select",	"Select (Port 1 Only)",	7,	IDIT_BUTTON, NULL},
	{"reset",	"Reset (Port 1 Only)",	8,	IDIT_BUTTON, NULL},
};

static InputDeviceInfoStruct InputDeviceInfoPort[] =
{
	{"none",	"none",		NULL,	0,	NULL},
	{"gamepad", "Gamepad",	NULL,	9,	GamepadIDII},
};


static const InputPortInfoStruct PortInfo[] =
{
	{0, "port1", "Port 1", 2, InputDeviceInfoPort, "gamepad"},
	{0, "port2", "Port 2", 2, InputDeviceInfoPort, "gamepad"},
};

InputInfoStruct		StellaInput =
{
	2,
	PortInfo
};


static FileExtensionSpecStruct	extensions[] = 
{
	{".a26", "Atari 2600 ROM"},
	{0, 0}
};


static MDFNSetting StellaSettings[] =
{
	{"stella.ramrandom",	MDFNSF_NOFLAGS,		"Use random values to initialize RAM",					NULL, MDFNST_BOOL,		"1"},
	{"stella.fastscbios",	MDFNSF_NOFLAGS,		"Use quick method to init SuperCharger BIOS",			NULL, MDFNST_BOOL,		"0"},
	{"stella.colorloss",	MDFNSF_NOFLAGS,		"Emulate Color loss",									NULL, MDFNST_BOOL,		"0"},
	{"stella.tiadriven",	MDFNSF_NOFLAGS,		"Randomly toggle unused TIA pins",						NULL, MDFNST_BOOL,		"0"},
	{"stella.palette",		MDFNSF_NOFLAGS,		"Color Palette to use",									NULL, MDFNST_STRING,	"standard"},
	{"stella.framerate",	MDFNSF_NOFLAGS,		"Unused",												NULL, MDFNST_UINT,		"60", "30", "70"},
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

