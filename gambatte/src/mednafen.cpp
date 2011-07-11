#include <src/mednafen.h>
#include <src/git.h>

#include <sstream>

#include "gambatte.h"
#include "resample/resamplerinfo.h"

using namespace Gambatte;

#include "mdfngmbt.h"

namespace mdfngmbt
{
	EmulateSpecStruct*			ESpec;
	bool						GameLoaded = false;
	bool						NeedToClearFrameBuffer = false;
	cothread_t					MainThread;
	GameBoy						*SideA, *SideB;
	uint8_t*					InputPort[2];
	uint8_t*					ROMData;
	uint32_t					ROMSize;

	void							GameBoy::ThreadA	()
	{
		while(1)
		{
			if(SideA && ESpec)
			{
				if(!SideA->Done)
				{
					if(ESpec->SoundFormatChanged)
					{
						SideA->Resample->adjustRate(2097152, (ESpec->SoundRate > 1.0) ? ESpec->SoundRate : 22050);
					}

					if(InputPort[0])
					{
						SideA->Input.inputs.startButton		= (*InputPort[0] & 8) ? 1 : 0;
						SideA->Input.inputs.selectButton	= (*InputPort[0] & 4) ? 1 : 0;
						SideA->Input.inputs.bButton			= (*InputPort[0] & 2) ? 1 : 0;
						SideA->Input.inputs.aButton			= (*InputPort[0] & 1) ? 1 : 0;
						SideA->Input.inputs.dpadUp			= (*InputPort[0] & 0x40) ? 1 : 0;
						SideA->Input.inputs.dpadDown		= (*InputPort[0] & 0x80) ? 1 : 0;
						SideA->Input.inputs.dpadLeft		= (*InputPort[0] & 0x20) ? 1 : 0;
						SideA->Input.inputs.dpadRight		= (*InputPort[0] & 0x10) ? 1 : 0;
					}

					uint32_t samps = SideA->Gambatte->runFor((Gambatte::uint_least32_t*)SideA->Samples, 35112 - SideA->SampleOverflow);
					SideA->SampleOverflow += samps;
					SideA->SampleOverflow -= 35112;

					//Grab sound
					uint32_t count = SideA->Resample->resample((short*)SideA->Resamples, (short*)SideA->Samples, samps);

					if(ESpec->SoundBuf && (ESpec->SoundBufMaxSize >= count))
					{
						ESpec->SoundBufSize = count;
						memcpy(ESpec->SoundBuf, SideA->Resamples, ESpec->SoundBufSize * 4);
					}

					SideA->Done = true;
				}
			}

			co_switch(MainThread);
		}
	}

	void							GameBoy::ThreadB	()
	{
		while(1)
		{
			if(SideB && ESpec)
			{
				if(!SideB->Done)
				{
					if(ESpec->SoundFormatChanged)
					{
						SideB->Resample->adjustRate(2097152, (ESpec->SoundRate > 1.0) ? ESpec->SoundRate : 22050);
					}

					if(InputPort[1])
					{
						SideB->Input.inputs.startButton		= (*InputPort[1] & 8) ? 1 : 0;
						SideB->Input.inputs.selectButton	= (*InputPort[1] & 4) ? 1 : 0;
						SideB->Input.inputs.bButton			= (*InputPort[1] & 2) ? 1 : 0;
						SideB->Input.inputs.aButton			= (*InputPort[1] & 1) ? 1 : 0;
						SideB->Input.inputs.dpadUp			= (*InputPort[1] & 0x40) ? 1 : 0;
						SideB->Input.inputs.dpadDown		= (*InputPort[1] & 0x80) ? 1 : 0;
						SideB->Input.inputs.dpadLeft		= (*InputPort[1] & 0x20) ? 1 : 0;
						SideB->Input.inputs.dpadRight		= (*InputPort[1] & 0x10) ? 1 : 0;
					}

					uint32_t samps = SideB->Gambatte->runFor((Gambatte::uint_least32_t*)SideB->Samples, 35112 - SideB->SampleOverflow);
					SideB->SampleOverflow += samps;
					SideB->SampleOverflow -= 35112;
				}

				SideB->Done = true;
			}
			co_switch(MainThread);
		}
	}
}
using namespace	mdfngmbt;

int				GmbtLoad				(const char *name, MDFNFILE *fp);
bool			GmbtTestMagic			(const char *name, MDFNFILE *fp);
void			GmbtCloseGame			(void);
bool			GmbtToggleLayer			(int which);
int				GmbtStateAction			(StateMem *sm, int load, int data_only);
void			GmbtEmulate				(EmulateSpecStruct *espec);
void			GmbtSetInput			(int port, const char *type, void *ptr);
void			GmbtDoSimpleCommand		(int cmd);

int				GmbtLoad				(const char *name, MDFNFILE *fp)
{
	if(GameLoaded)
	{
		GmbtCloseGame();
	}

	//Copy ROM data for multi-instance
	ROMData = new uint8_t[fp->size];
	ROMSize = fp->size;
	memcpy(ROMData, fp->data, fp->size);

	//Get main thread handle
	MainThread = co_active();

	//Load game
	SideA = new GameBoy(fp->data, fp->size, 0);

	GameLoaded = true;
	return 1;
}

bool			GmbtTestMagic			(const char *name, MDFNFILE *fp)
{
	static const uint8 GBMagic[8] = { 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B };
	return fp->size > 0x10C && !memcmp(fp->data + 0x104, GBMagic, 8);
}

void			GmbtCloseGame			(void)
{
	delete SideA;
	delete SideB;
	delete[] ROMData;

	SideA = 0;
	SideB = 0;
	ROMData = 0;

	GameLoaded = false;
}

int				GmbtStateAction			(StateMem *sm, int load, int data_only)
{
	//TODO:
	if(!load)
	{
		std::ostringstream os(std::ios_base::out | std::ios_base::binary);
		SideA->Gambatte->saveState(os);

		void* buffer = malloc(os.str().size());
		memcpy(buffer, os.str().data(), os.str().size());

		smem_write32le(sm, os.str().size());
		smem_write(sm, buffer, os.str().size());

		free(buffer);

		return 1;
	}
	else
	{
		uint32_t size;
		smem_read32le(sm, &size);

		char* buffer = (char*)malloc(size);
		smem_read(sm, buffer, size);

		std::istringstream iss(std::string((const char*)buffer, (size_t)size), std::ios_base::in | std::ios_base::binary);
		SideA->Gambatte->loadState(iss);

		free(buffer);

		SideA->SampleOverflow = 0;

		return 1;
	}
	return 0;
}

void			GmbtEmulate				(EmulateSpecStruct *espec)
{
	ESpec = espec;

	//Clear frame after starting new instance
	if(NeedToClearFrameBuffer)
	{
		NeedToClearFrameBuffer = false;
		memset(ESpec->surface->pixels, 0, ESpec->surface->h * ESpec->surface->pitch32);
	}

	//Run frame
	SideA->Done = false;
	if(SideB) SideB->Done = false;

	co_switch(SideA->Thread);

	while(!SideA->Done) co_switch(SideA->Thread);

	if(SideB)
	{
		while(!SideB->Done) co_switch(SideB->Thread);
	}

	//Set frame size
	espec->DisplayRect.x = 0;
	espec->DisplayRect.y = 0;
	espec->DisplayRect.w = SideB ? 320 : 160;
	espec->DisplayRect.h = SideB ? 288 : 144;

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

void			GmbtSetInput			(int port, const char *type, void *ptr)
{
	if(port >= 0 && port < 2)
	{
		InputPort[port] = (uint8_t*)ptr;
	}
}

void			GmbtDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET || cmd == MDFN_MSC_POWER)
	{
		SideA->Gambatte->reset();
		SideA->CycleCounter = 0;

		if(SideB)
		{
			SideB->Gambatte->reset();
			SideB->CycleCounter = 0;
		}
	}
	else if(!SideB && cmd == MDFN_MSC_SELECT_DISK)
	{
		SideB = new GameBoy(ROMData, ROMSize, 1);
		NeedToClearFrameBuffer = true;

		GmbtDoSimpleCommand(MDFN_MSC_RESET);
	}
}

static const InputDeviceInputInfoStruct IDII[] =
{
	{"a",		"A",		7, IDIT_BUTTON_CAN_RAPID,	NULL},
	{"b",		"B",		6, IDIT_BUTTON_CAN_RAPID,	NULL},
	{"select",	"SELECT",	4, IDIT_BUTTON,				NULL},
	{"start",	"START",	5, IDIT_BUTTON,				NULL},
	{"right",	"RIGHT",	3, IDIT_BUTTON,				"left"},
	{"left",	"LEFT",		2, IDIT_BUTTON,				"right"},
	{"up",		"UP",		0, IDIT_BUTTON,				"down"},
	{"down",	"DOWN",		1, IDIT_BUTTON,				"up"},
};

static InputDeviceInfoStruct InputDeviceInfo[] =	{{"gamepad", "Gamepad", NULL, 8, IDII,}};
static const InputPortInfoStruct PortInfo[] =		{{0, "builtin", "Built-In", 1, InputDeviceInfo, "gamepad"},{0, "builtin", "Built-In", 1, InputDeviceInfo, "gamepad"}};
static InputInfoStruct GmbtInput =					{2, PortInfo};

static FileExtensionSpecStruct	extensions[] = 
{
	{".gb",		"Game Boy Rom"},
	{".gbc",	"Game Boy Color Rom"},
	{".cgb",	"Game Boy Color Rom"},
	{0,			0}
};


static MDFNSetting GmbtSettings[] =
{
	{"gmbt.forcedmg",	MDFNSF_NOFLAGS,	"Force GB Mono Mode.",	NULL, MDFNST_BOOL,	"0"},
	{0}
};


static MDFNGI	GmbtInfo = 
{
/*	shortname:*/		"gmbt",
/*	fullname:*/			"Game Boy Color (Gambatte)",
/*	FileExtensions:*/	extensions,
/*	ModulePriority:*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:*/			0,
/*	InputInfo:*/		&GmbtInput,

/*	Load:*/				GmbtLoad,
/*	TestMagic:*/		GmbtTestMagic,
/*	LoadCD:*/			0,
/*	TestMagicCD:*/		0,
/*	CloseGame:*/		GmbtCloseGame,
/*	ToggleLayer:*/		0,
/*	LayerNames:*/		0,
/*	InstallReadPatch:*/	0,
/*	RemoveReadPatches:*/0,
/*	MemRead:*/			0,
/*	StateAction:*/		GmbtStateAction,
/*	Emulate:*/			GmbtEmulate,
/*	SetInput:*/			GmbtSetInput,
/*	DoSimpleCommand:*/	GmbtDoSimpleCommand,
/*	Settings:*/			GmbtSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:*/				0,
/*	multires:*/			false,
/*	lcm_width:*/		320,
/*	lcm_height:*/		288,
/*	dummy_separator:*/	0,
/*	nominal_width:*/	320,
/*	nominal_height:*/	288,
/*	fb_width:*/			320,
/*	fb_height:*/		288,
/*	soundchan:*/		2
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
#define VERSION_FUNC gmbtGetVersion
#define GETEMU_FUNC gmbtGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC()
{
	return &GmbtInfo;
}

