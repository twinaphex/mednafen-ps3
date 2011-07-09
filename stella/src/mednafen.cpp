#include <src/mednafen.h>
#include <src/git.h>
#include <include/Fir_Resampler.h>

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

struct		MDFNStella
{
	Console*			GameConsole;

	Fir_Resampler<8>*	Resampler;		//The TIA gives shitty sound if the output rate doesn't equal the input rate, so resample here!
	Settings			GameSettings;
	SoundSDL			Sound;
	uint8_t*			Port[2];
	uint32_t			PortType[2];
	const uInt32*		Palette;

	MDFNStella() 	{Resampler = 0; GameConsole = 0; Port[0] = 0; Port[1] = 0; PortType[0] = 0; PortType[1] = 0; Palette = 0;}
	~MDFNStella()	{delete Resampler; delete GameConsole;}
};
MDFNStella*				mdfnStella;

//Function to receive emulator palette
void		stellaMDFNSetPalette		(const uInt32* palette)
{
	if(mdfnStella)
	{
		mdfnStella->Palette = palette;
	}
}

Settings&	stellaMDFNSettings			()
{
	if(mdfnStella)
	{
		return mdfnStella->GameSettings;
	}

	//HACK
	abort();
}


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
		//Create the stella objects
		mdfnStella = new MDFNStella();

		//Get the cart's MD5 sum
		string cartMD5 = MD5(fp->data, fp->size);

		//Get the game properties
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
	delete mdfnStella;
	mdfnStella = 0;
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
		//Can't load a state if there is no console.
		if(!mdfnStella || !mdfnStella->GameConsole)
		{
			return 0;
		}

		//Get the state stream
		Serializer state(sm);

		//Do the action
		if(load)		mdfnStella->GameConsole->load(state);
		else			mdfnStella->GameConsole->save(state);

		//Done and done
		return 1;
	}
	catch(...)
	{
		return 0;
	}
}

void			StellaEmulate			(EmulateSpecStruct *espec)
{
	//Only if we are open.
	if(mdfnStella)
	{
		//Update the input
		for(int i = 0; i != 2; i ++)
		{
			if(mdfnStella->Port[i])
			{
				uint32_t inputState = mdfnStella->Port[i][0] | (mdfnStella->Port[i][1] << 8);
				for(int j = 0; j != stellaInputDefs[mdfnStella->PortType[i]].Count - ((i == 1) ? 2 : 0); j ++, inputState >>= 1)
				{
					mdfnStella->GameConsole->event().set(stellaInputDefs[mdfnStella->PortType[i]].IDs[j], inputState & 1);
				}
			}
		}

		mdfnStella->GameConsole->switches().update();
		mdfnStella->GameConsole->controller(Controller::Left).update();
		mdfnStella->GameConsole->controller(Controller::Right).update();


		// Run the console for one frame
		mdfnStella->GameConsole->tia().update();

	//	if(myOSystem->eventHandler().frying())
	//		myOSystem->GameConsole().fry();

		// And update the screen
		uInt8* currentFrame = mdfnStella->GameConsole->tia().currentFrameBuffer();
		uInt32 width  = mdfnStella->GameConsole->tia().width();
		uInt32 height = mdfnStella->GameConsole->tia().height();

		espec->DisplayRect.x = 0;
		espec->DisplayRect.y = 0;
		espec->DisplayRect.w = width;
		espec->DisplayRect.h = height;

		for(int i = 0; i != height; i ++)
		{
			for(int j = 0; j != width; j ++)
			{
				espec->surface->pixels[i * 640 + j] = mdfnStella->Palette ? mdfnStella->Palette[currentFrame[i * width + j]] : 0;
			}
		}

		//AUDIO
		//Update audio format and create the resampler
		if(espec->SoundFormatChanged)
		{
			delete mdfnStella->Resampler;

			if(espec->SoundRate > 1.0)
			{
				mdfnStella->Resampler = new Fir_Resampler<8>();
				mdfnStella->Resampler->buffer_size(1600*2);
				mdfnStella->Resampler->time_ratio(34100.0 / espec->SoundRate, 0.9965);
			}
			else
			{
				mdfnStella->Resampler = 0;
			}
		}

		//Copy audio data
		if(mdfnStella->Resampler && espec->SoundBuf && espec->SoundBufMaxSize)
		{
			//HACK: 568 = 34100 / 60, update for framerate later

			//Process one frame of audio from stella
			uint8_t samplebuffer[2048];
			mdfnStella->Sound.processFragment(samplebuffer, 568);

			//Convert and stash it in the resampler...
			for(int i = 0; i != 568; i ++)
			{
				int16_t sample = (samplebuffer[i] << 8);
				sample -= 32768;
				mdfnStella->Resampler->buffer()[i * 2 + 0] = sample;
				mdfnStella->Resampler->buffer()[i * 2 + 1] = sample;
			}

			mdfnStella->Resampler->write(568 * 2);

			//Get the output
			//TODO: Never overrun the output buffer...
			espec->SoundBufSize = mdfnStella->Resampler->read(espec->SoundBuf, mdfnStella->Resampler->avail()) >> 1;
		}

		//TODO: Real timing
		espec->MasterCycles = 1LL * 100;
	}
}

void			StellaSetInput			(int port, const char *type, void *ptr)
{
	if(mdfnStella && port >= 0 && port < 2)
	{
		mdfnStella->Port[port] = (uint8_t*)ptr;

		for(int i = 0; i != sizeof(stellaInputDefs) / sizeof(stellaInputDefs[0]); i ++)
		{
			if(strcmp(stellaInputDefs[i].Name, type) == 0 && stellaInputDefs[i].Port == port)
			{
				mdfnStella->PortType[port] = i;
				return;
			}
		}

		mdfnStella->PortType[port] = 0;
	}
}

void			StellaDoSimpleCommand	(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		mdfnStella->GameConsole->system().reset();
	}
	else if(cmd == MDFN_MSC_POWER)
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
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC()
{
	return &StellaInfo;
}

