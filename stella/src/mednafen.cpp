#include <src/mednafen.h>
#include <src/mempatcher.h>
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

extern MDFNGI	StellaInfo;

//Structure holding all of the objects needed for a stella instance
struct					MDFNStella
{
	Console*			GameConsole;
	Settings			GameSettings;

	const uInt32*		Palette;

	SoundSDL			Sound;
	Fir_Resampler<8>*	Resampler;		//The TIA gives shitty sound if the output rate doesn't equal the input rate, so resample here!

	uint8_t*			Port[2];
	uint32_t			PortType[2];

	MDFNStella() 	{GameConsole = 0; Palette = 0; Resampler = 0; Port[0] = 0; Port[1] = 0; PortType[0] = 0; PortType[1] = 0; }
	~MDFNStella()	{delete Resampler; delete GameConsole;}
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
	//Only if we are open.
	if(mdfnStella)
	{
		//CHEATS
		MDFNMP_ApplyPeriodicCheats();

		//INPUT
		//Update stella's event structure
		for(int i = 0; i != 2; i ++)
		{
			//Get the base event id for this port
			Event::Type baseEvent = (i == 0) ? Event::JoystickZeroUp : Event::JoystickOneUp;

			//Get the input data for this port and stuff it in the event structure
			uint32_t inputState = mdfnStella->Port[i] ? (mdfnStella->Port[i][0] | (mdfnStella->Port[i][1] << 8) | (mdfnStella->Port[i][2] << 16)) : 0;
			for(int j = 0; j != 19; j ++, inputState >>= 1)
			{
				mdfnStella->GameConsole->event().set((Event::Type)(baseEvent + j), inputState & 1);
			}
		}

		//Update the reset and select events
		uint32_t inputState = mdfnStella->Port[0] ? (mdfnStella->Port[0][2] << 16) >> 19 : 0;
		mdfnStella->GameConsole->event().set(Event::ConsoleSelect, inputState & 1);
		mdfnStella->GameConsole->event().set(Event::ConsoleReset, inputState & 2);

		//Tell all input devices to read their state from the event structure
		mdfnStella->GameConsole->switches().update();
		mdfnStella->GameConsole->controller(Controller::Left).update();
		mdfnStella->GameConsole->controller(Controller::Right).update();

		//EXECUTE
		mdfnStella->GameConsole->tia().update();

	//	if(myOSystem->eventHandler().frying())
	//		myOSystem->GameConsole().fry();

		//VIDEO: TODO: Support other color formats
		//Get the frame info from stella
		uInt8* currentFrame = mdfnStella->GameConsole->tia().currentFrameBuffer();
		Int32 frameWidth = mdfnStella->GameConsole->tia().width();
		Int32 frameHeight = mdfnStella->GameConsole->tia().height();

		//Setup the output area for mednafen, never allow stella's size to excede mednafen's
		espec->DisplayRect.x = 0;
		espec->DisplayRect.y = 0;
		espec->DisplayRect.w = std::min(frameWidth, espec->surface->w);
		espec->DisplayRect.h = std::min(frameHeight, espec->surface->h);

		//Copy the frame from stella to mednafen
		for(int i = 0; i != espec->DisplayRect.h; i ++)
		{
			for(int j = 0; j != espec->DisplayRect.w; j ++)
			{
				espec->surface->pixels[i * espec->surface->pitchinpix + j] = mdfnStella->Palette ? mdfnStella->Palette[currentFrame[i * frameWidth + j]] : 0;
			}
		}

		//AUDIO
		//Update audio format and create the resampler
		if(espec->SoundFormatChanged)
		{
			delete mdfnStella->Resampler;
			mdfnStella->Resampler = 0;

			//Only make a resampler if the sound rate is valid
			if(espec->SoundRate > 1.0)
			{
				mdfnStella->Resampler = new Fir_Resampler<8>();
				mdfnStella->Resampler->buffer_size(1600*2);
				mdfnStella->Resampler->time_ratio(34100.0 / espec->SoundRate, 0.9965);
			}
		}

		//Copy audio data
		if(mdfnStella->Resampler && espec->SoundBuf && espec->SoundBufMaxSize)
		{
			//Get the number of samples in a frame
			uint32_t soundFrameSize = 34100.0f / mdfnStella->GameConsole->getFramerate();

			//Process one frame of audio from stella
			uint8_t samplebuffer[2048];
			mdfnStella->Sound.processFragment(samplebuffer, soundFrameSize);

			//Convert and stash it in the resampler...
			for(int i = 0; i != soundFrameSize; i ++)
			{
				int16_t sample = (samplebuffer[i] << 8);
				sample -= 32768;
				mdfnStella->Resampler->buffer()[i * 2 + 0] = sample;
				mdfnStella->Resampler->buffer()[i * 2 + 1] = sample;
			}

			mdfnStella->Resampler->write(soundFrameSize * 2);

			//Get the output
			//TODO: Never overrun the output buffer...
			espec->SoundBufSize = mdfnStella->Resampler->read(espec->SoundBuf, mdfnStella->Resampler->avail()) >> 1;
		}

		//TIMING
		espec->MasterCycles = 1LL * 100;
	}
}

void			StellaSetInput			(int port, const char *type, void *ptr)
{
	//Don't do anything if state isn't valid
	if(mdfnStella && port >= 0 && port < 2)
	{
		mdfnStella->Port[port] = (uint8_t*)ptr;
		mdfnStella->PortType[port] = strcmp(type, "none") != 0;
	}
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
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC()
{
	return &StellaInfo;
}

