#include <src/mednafen.h>
#include <src/git.h>

#include "Settings.hxx"
#include "TIA.hxx"
#include "Props.hxx"
#include "Cart.hxx"
#include "Console.hxx"
#include "Serializer.hxx"
#include "Event.hxx"
#include "Switches.hxx"

uint32_t stellaNTSCPalette[256] = {
  0x000000, 0, 0x4a4a4a, 0, 0x6f6f6f, 0, 0x8e8e8e, 0,
  0xaaaaaa, 0, 0xc0c0c0, 0, 0xd6d6d6, 0, 0xececec, 0,
  0x484800, 0, 0x69690f, 0, 0x86861d, 0, 0xa2a22a, 0,
  0xbbbb35, 0, 0xd2d240, 0, 0xe8e84a, 0, 0xfcfc54, 0,
  0x7c2c00, 0, 0x904811, 0, 0xa26221, 0, 0xb47a30, 0,
  0xc3903d, 0, 0xd2a44a, 0, 0xdfb755, 0, 0xecc860, 0,
  0x901c00, 0, 0xa33915, 0, 0xb55328, 0, 0xc66c3a, 0,
  0xd5824a, 0, 0xe39759, 0, 0xf0aa67, 0, 0xfcbc74, 0,
  0x940000, 0, 0xa71a1a, 0, 0xb83232, 0, 0xc84848, 0,
  0xd65c5c, 0, 0xe46f6f, 0, 0xf08080, 0, 0xfc9090, 0,
  0x840064, 0, 0x97197a, 0, 0xa8308f, 0, 0xb846a2, 0,
  0xc659b3, 0, 0xd46cc3, 0, 0xe07cd2, 0, 0xec8ce0, 0,
  0x500084, 0, 0x68199a, 0, 0x7d30ad, 0, 0x9246c0, 0,
  0xa459d0, 0, 0xb56ce0, 0, 0xc57cee, 0, 0xd48cfc, 0,
  0x140090, 0, 0x331aa3, 0, 0x4e32b5, 0, 0x6848c6, 0,
  0x7f5cd5, 0, 0x956fe3, 0, 0xa980f0, 0, 0xbc90fc, 0,
  0x000094, 0, 0x181aa7, 0, 0x2d32b8, 0, 0x4248c8, 0,
  0x545cd6, 0, 0x656fe4, 0, 0x7580f0, 0, 0x8490fc, 0,
  0x001c88, 0, 0x183b9d, 0, 0x2d57b0, 0, 0x4272c2, 0,
  0x548ad2, 0, 0x65a0e1, 0, 0x75b5ef, 0, 0x84c8fc, 0,
  0x003064, 0, 0x185080, 0, 0x2d6d98, 0, 0x4288b0, 0,
  0x54a0c5, 0, 0x65b7d9, 0, 0x75cceb, 0, 0x84e0fc, 0,
  0x004030, 0, 0x18624e, 0, 0x2d8169, 0, 0x429e82, 0,
  0x54b899, 0, 0x65d1ae, 0, 0x75e7c2, 0, 0x84fcd4, 0,
  0x004400, 0, 0x1a661a, 0, 0x328432, 0, 0x48a048, 0,
  0x5cba5c, 0, 0x6fd26f, 0, 0x80e880, 0, 0x90fc90, 0,
  0x143c00, 0, 0x355f18, 0, 0x527e2d, 0, 0x6e9c42, 0,
  0x87b754, 0, 0x9ed065, 0, 0xb4e775, 0, 0xc8fc84, 0,
  0x303800, 0, 0x505916, 0, 0x6d762b, 0, 0x88923e, 0,
  0xa0ab4f, 0, 0xb7c25f, 0, 0xccd86e, 0, 0xe0ec7c, 0,
  0x482c00, 0, 0x694d14, 0, 0x866a26, 0, 0xa28638, 0,
  0xbb9f47, 0, 0xd2b656, 0, 0xe8cc63, 0, 0xfce070, 0
};


Settings	stellaSettings(0);	//TODO:<
Properties	stellaProperties;
Console*	stellaConsole;
uint8_t*	stellaPort[2];

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
	//Update the input
	Event::Type inputIDs[] = {	Event::JoystickZeroUp, Event::JoystickZeroDown, Event::JoystickZeroLeft, Event::JoystickZeroRight,
								Event::JoystickZeroFire1, Event::JoystickZeroFire2, Event::JoystickZeroFire3, Event::ConsoleSelect, Event::ConsoleReset};
	uint32_t inputState = stellaPort[0][0] | (stellaPort[0][1] << 8);
	for(int i = 0; i != 9; i ++, inputState >>= 1)
	{
		stellaConsole->event().set(inputIDs[i], inputState & 1);
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
			espec->surface->pixels[i * 640 + j] = stellaNTSCPalette[currentFrame[i * width + j]];
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
	{"select",	"Select (On Console)",	7,	IDIT_BUTTON, NULL},
	{"reset",	"Reset (On Console)",	8,	IDIT_BUTTON, NULL},
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

