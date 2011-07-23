#include <src/mednafen.h>
#include <src/mempatcher.h>
#include <src/git.h>

#define MODULENAMESPACE	Stella
#include <module_helper.h>
using namespace Stella;

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
#include "SoundSDL.hxx"

extern MDFNGI	StellaInfo;

//Structure holding all of the objects needed for a stella instance
struct					MDFNStella
{
	Console*			GameConsole;
	Settings			GameSettings;

	const uInt32*		Palette;

	SoundSDL			Sound;

	MDFNStella() 	{GameConsole = 0; Palette = 0;}
	~MDFNStella()	{delete GameConsole;}
};
MDFNStella*				mdfnStella;

//Set the palette for the current setlla instance
void					stellaMDFNSetPalette		(const uInt32* palette)
{
	if(mdfnStella)
	{
		mdfnStella->Palette = palette;
	}
}

//Get the settings from the current stella instance
Settings&				stellaMDFNSettings			()
{
	if(mdfnStella)
	{
		return mdfnStella->GameSettings;
	}

	//HACK
	abort();
}

//Implement MDFNGI:
int				StellaLoad				(const char *name, MDFNFILE *fp)
{
	try
	{
		//Setup cheats
		MDFNMP_Init(128, 65536 / 128);

		//Create the stella objects
		mdfnStella = new MDFNStella();

		//Get the game properties
		string cartMD5 = MD5(fp->data, fp->size);
		PropertiesSet propslist(0);
		Properties gameProperties;
		propslist.getMD5(cartMD5, gameProperties);

		//Load the cart
		string cartType = gameProperties.get(Cartridge_Type);
		string cartID = "";
		Cartridge* stellaCart = Cartridge::create(fp->data, fp->size, cartMD5, cartType, cartID, mdfnStella->GameSettings);

		if(stellaCart == 0)
		{
			MDFND_PrintError("Stella: Failed to load cartridge.");
			return 0;
		}

		//Create the console
		mdfnStella->GameConsole = new Console(mdfnStella->Sound, stellaCart, gameProperties);

		//Init sound
	    mdfnStella->Sound.open();

		//Update MDFNGI's timing
		StellaInfo.MasterClock = MDFN_MASTERCLOCK_FIXED(((uint32_t)mdfnStella->GameConsole->getFramerate()) * 100);

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
	//There is no reliable way to detect a 2600 game, only support games listed in stella's properties file
	string cartMD5 = MD5(fp->data, fp->size);
	PropertiesSet propslist(0);
	Properties gameProperties;
	propslist.getMD5(cartMD5, gameProperties);

	return gameProperties.get(Cartridge_MD5).length() != 0;
}

void			StellaCloseGame			(void)
{
	Resampler::Kill();

	//TODO: Is anything else needed here?
	delete mdfnStella;
	mdfnStella = 0;

	//Stop cheats
	MDFNMP_Kill();
}

int				StellaStateAction		(StateMem *sm, int load, int data_only)
{
	try
	{
		//Can't load a state if there is no console.
		if(mdfnStella && mdfnStella->GameConsole)
		{
			//Get the state stream
			Serializer state(sm);

			//Do the action
			if(load)		mdfnStella->GameConsole->load(state);
			else			mdfnStella->GameConsole->save(state);

			//Done and done
			return 1;
		}
	}
	catch(...){}

	return 0;
}

void			StellaEmulate			(EmulateSpecStruct *espec)
{
	//AUDIO PREP
	Resampler::Init(espec, 34100.0);

	//INPUT
	//Update stella's event structure
	for(int i = 0; i != 2; i ++)
	{
		//Get the base event id for this port
		Event::Type baseEvent = (i == 0) ? Event::JoystickZeroUp : Event::JoystickOneUp;

		//Get the input data for this port and stuff it in the event structure
		uint32_t inputState = Input::GetPort<3>(i);
		for(int j = 0; j != 19; j ++, inputState >>= 1)
		{
			mdfnStella->GameConsole->event().set((Event::Type)(baseEvent + j), inputState & 1);
		}
	}

	//Update the reset and select events
	uint32_t inputState = Input::GetPort<0, 3>() >> 19;
	mdfnStella->GameConsole->event().set(Event::ConsoleSelect, inputState & 1);
	mdfnStella->GameConsole->event().set(Event::ConsoleReset, inputState & 2);

	//Tell all input devices to read their state from the event structure
	mdfnStella->GameConsole->switches().update();
	mdfnStella->GameConsole->controller(Controller::Left).update();
	mdfnStella->GameConsole->controller(Controller::Right).update();

	//CHEATS
	MDFNMP_ApplyPeriodicCheats();

	//EMULATE
	mdfnStella->GameConsole->tia().update();

	//	if(myOSystem->eventHandler().frying())
	//		myOSystem->GameConsole().fry();

	//VIDEO: TODO: Support other color formats
	//Get the frame info from stella
	Int32 frameWidth = mdfnStella->GameConsole->tia().width();
	Int32 frameHeight = mdfnStella->GameConsole->tia().height();

	//Setup the output area for mednafen, never allow stella's size to excede mednafen's
	Video::SetDisplayRect(espec, 0, 0, frameWidth, frameHeight);

	//Copy the frame from stella to mednafen
	Video::BlitPalette<0xFF>(espec, mdfnStella->Palette, mdfnStella->GameConsole->tia().currentFrameBuffer(), frameWidth, frameHeight, frameWidth);

	//AUDIO
	//Get the number of samples in a frame
	uint32_t soundFrameSize = 34100.0f / mdfnStella->GameConsole->getFramerate();

	//Process one frame of audio from stella
	uint8_t samplebuffer[2048];
	mdfnStella->Sound.processFragment(samplebuffer, soundFrameSize);

	//Convert and stash it in the resampler...
	for(int i = 0; i != soundFrameSize; i ++)
	{
		int16_t sample = (samplebuffer[i] << 8) - 32768;
		int16_t frame[2] = {sample, sample};
		Resampler::Fill(frame, 2);
	}

	//Get the output
	Resampler::Fetch(espec);

	//TIMING
	espec->MasterCycles = 1LL * 100;
}

void			StellaSetInput			(int port, const char *type, void *ptr)
{
	Input::SetPort(port, (uint8_t*)ptr);
}

void			StellaDoSimpleCommand	(int cmd)
{
	//Reset the game
	if(cmd == MDFN_MSC_RESET || cmd == MDFN_MSC_POWER)
	{
		mdfnStella->GameConsole->system().reset();
	}
}

static const InputDeviceInputInfoStruct GamepadIDII[] =
{
	{"up",		"Up",					0,	IDIT_BUTTON, NULL},
	{"down",	"Down",					1,	IDIT_BUTTON, NULL},
	{"left",	"Left",					2,	IDIT_BUTTON, NULL},
	{"right",	"Right",				3,	IDIT_BUTTON, NULL},
	{"fire1",	"Fire 1",				4,	IDIT_BUTTON, NULL},
	{"fire2",	"Fire 2",				5,	IDIT_BUTTON, NULL},
	{"fire3",	"Fire 3",				6,	IDIT_BUTTON, NULL},

	{"one",		"Keyboard One",			7,	IDIT_BUTTON, NULL},
	{"two",		"Keyboard Two",			8,	IDIT_BUTTON, NULL},
	{"three",	"Keyboard Three",		9,	IDIT_BUTTON, NULL},
	{"four",	"Keyboard Four",		10,	IDIT_BUTTON, NULL},
	{"five",	"Keyboard Five",		11,	IDIT_BUTTON, NULL},
	{"six",		"Keyboard Six",			12,	IDIT_BUTTON, NULL},
	{"seven",	"Keyboard Seven",		13,	IDIT_BUTTON, NULL},
	{"eight",	"Keyboard Eight",		14,	IDIT_BUTTON, NULL},
	{"nine",	"Keyboard Nine",		15,	IDIT_BUTTON, NULL},
	{"star",	"Keyboard Star",		16,	IDIT_BUTTON, NULL},
	{"zero",	"Keyboard Zero",		17,	IDIT_BUTTON, NULL},
	{"pound",	"Keyboard Pound",		18,	IDIT_BUTTON, NULL},

	{"select",	"Select (Port 1 Only)",	19,	IDIT_BUTTON, NULL},
	{"reset",	"Reset (Port 1 Only)",	20,	IDIT_BUTTON, NULL},
};

static InputDeviceInfoStruct InputDeviceInfoPort[] =
{
	{"none",		"none",			NULL,	0,	NULL},
	{"gamepad",		"All Types",	NULL,	21,	GamepadIDII}
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
	{".bin", "Atari 2600 ROM"},
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
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
/*	StateAction:		*/	StellaStateAction,
/*	Emulate:			*/	StellaEmulate,
/*	SetInput:			*/	StellaSetInput,
/*	DoSimpleCommand:	*/	StellaDoSimpleCommand,
/*	Settings:			*/	StellaSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	true,
/*	lcm_width:			*/	160,	//?
/*	lcm_height:			*/	5250,	//?
/*	dummy_separator:	*/	0,
/*	nominal_width:		*/	320,	//?
/*	nominal_height:		*/	250,	//?
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
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &StellaInfo : 0;
}

