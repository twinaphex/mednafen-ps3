#include <src/mednafen.h>
#include <src/git.h>

#include <sstream>

#include "gambatte.h"
#include "resample/resamplerinfo.h"

#include <src/snes/src/lib/libco/libco.h>

using namespace Gambatte;

namespace
{
	EmulateSpecStruct*			ESpec;
	bool						GameLoaded = false;
	bool						NeedToClearFrameBuffer = false;

	cothread_t					MainThread;

	class							GameBoy;
	GameBoy							*SideA, *SideB;

	class gbblitter : public VideoBlitter
	{
		public:
								gbblitter				()			{buffer.pixels = 0;}
								~gbblitter				()			{delete[] (uint32_t*)buffer.pixels;}
			const PixelBuffer	inBuffer				()			{return buffer;}
			void				setSide					(uint32_t a){side = a;}


			void				setBufferDimensions		(unsigned aWidth, unsigned aHeight)
			{
				delete[] (uint32_t*)buffer.pixels;
				buffer.pixels = new uint32_t[aWidth * aHeight];

				width = aWidth;
				height = aHeight;
				buffer.pitch = aWidth;
			}


			void				blit					()
			{
				uint32_t* sourceptr = (uint32_t*)buffer.pixels;

				for(int i = 0; i != height; i ++)
				{
					for(int j = 0; j != width ; j ++)
					{
						if(!SideB)
						{
							ESpec->surface->pixels[i * ESpec->surface->pitch32 + j] = sourceptr[i * width + j];
						}
						else
						{
							ESpec->surface->pixels[(i + (144 / 2)) * ESpec->surface->pitch32 + j + (side ? 160 : 0)] = sourceptr[i * width + j];
						}
					}
				}
			}

		protected:
			uint32_t			width, height;
			uint32_t			side;
			PixelBuffer			buffer;
	};


	class gbinput : public InputStateGetter
	{
		public:
			const InputState& 		operator()			()	{return inputs;};

			InputState				inputs;
	};

	class							GameBoy
	{
		public:
									GameBoy				(const void* aROM, uint32_t aSize, uint32_t aSide)
			{
				Side = aSide;

				Gambatte = new GB();
				Resample = ResamplerInfo::get(0).create(2097152, 48000, 35112);

				SampleOverflow = 0;
				memset(Samples, 0, sizeof(Samples));
				memset(Resamples, 0, sizeof(Resamples));

				std::istringstream file(std::string((const char*)aROM, (size_t)aSize), std::ios_base::in | std::ios_base::binary);	
				if(Gambatte->load(file, MDFN_GetSettingB("gmbt.forcedmg")))
				{
					delete Resample;
					delete Gambatte;

					MDFND_PrintError("gambatte: Failed to load ROM");
					throw 0;
				}

				Blitter.setSide(aSide);

				Gambatte->setVideoBlitter(&Blitter);
				Gambatte->setInputStateGetter(&Input);

				Thread = co_create(65536 * sizeof(void*), aSide ? ThreadB : ThreadA);
			}

									~GameBoy			()
			{
				delete Resample;
				delete Gambatte;

				co_delete(Thread);
			}

			static void				ThreadA				();
			static void				ThreadB				();

			GB*						Gambatte;
			Resampler*				Resample;
			gbblitter				Blitter;
			gbinput					Input;
			uint32_t				Side;

			cothread_t				Thread;

			uint32_t				Samples[48000];
			uint32_t				Resamples[48000];
			int32_t					SampleOverflow;
	};

	uint8_t*						InputPort;
	uint8_t*						ROMData;
	uint32_t						ROMSize;

	void							GameBoy::ThreadA	()
	{
		while(1)
		{
			if(ESpec->SoundFormatChanged)
			{
				SideA->Resample->adjustRate(2097152, ESpec->SoundRate);
			}

			if(InputPort)
			{
				SideA->Input.inputs.startButton		= (*InputPort & 8) ? 1 : 0;
				SideA->Input.inputs.selectButton	= (*InputPort & 4) ? 1 : 0;
				SideA->Input.inputs.bButton			= (*InputPort & 2) ? 1 : 0;
				SideA->Input.inputs.aButton			= (*InputPort & 1) ? 1 : 0;
				SideA->Input.inputs.dpadUp			= (*InputPort & 0x40) ? 1 : 0;
				SideA->Input.inputs.dpadDown		= (*InputPort & 0x80) ? 1 : 0;
				SideA->Input.inputs.dpadLeft		= (*InputPort & 0x20) ? 1 : 0;
				SideA->Input.inputs.dpadRight		= (*InputPort & 0x10) ? 1 : 0;
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

			co_switch(MainThread);
		}
	}

	void							GameBoy::ThreadB	()
	{
		while(1)
		{
			if(ESpec->SoundFormatChanged)
			{
				SideB->Resample->adjustRate(2097152, ESpec->SoundRate);
			}

			if(InputPort)
			{
				SideB->Input.inputs.startButton		= (*InputPort & 8) ? 1 : 0;
				SideB->Input.inputs.selectButton	= (*InputPort & 4) ? 1 : 0;
				SideB->Input.inputs.bButton			= (*InputPort & 2) ? 1 : 0;
				SideB->Input.inputs.aButton			= (*InputPort & 1) ? 1 : 0;
				SideB->Input.inputs.dpadUp			= (*InputPort & 0x40) ? 1 : 0;
				SideB->Input.inputs.dpadDown		= (*InputPort & 0x80) ? 1 : 0;
				SideB->Input.inputs.dpadLeft		= (*InputPort & 0x20) ? 1 : 0;
				SideB->Input.inputs.dpadRight		= (*InputPort & 0x10) ? 1 : 0;
			}

			uint32_t samps = SideB->Gambatte->runFor((Gambatte::uint_least32_t*)SideB->Samples, 35112 - SideB->SampleOverflow);
			SideB->SampleOverflow += samps;
			SideB->SampleOverflow -= 35112;

			co_switch(MainThread);
		}
	}
}

int				GmbtLoad				(const char *name, MDFNFILE *fp);
bool			GmbtTestMagic			(const char *name, MDFNFILE *fp);
void			GmbtCloseGame			(void);
bool			GmbtToggleLayer			(int which);
void			GmbtInstallReadPatch	(uint32 address);
void			GmbtRemoveReadPatches	(void);
uint8			GmbtMemRead				(uint32 addr);
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

void			GmbtInstallReadPatch	(uint32 address)
{
	//TODO:
}

void			GmbtRemoveReadPatches	(void)
{
	//TODO:
}

uint8			GmbtMemRead				(uint32 addr)
{
	//TODO:
	return 0;
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
	co_switch(SideA->Thread);

	//Run frame of second instance
	if(SideB)
	{
		co_switch(SideB->Thread);
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
	if(port == 0)
	{
		InputPort = (uint8_t*)ptr;
	}
}

void			GmbtDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET || cmd == MDFN_MSC_POWER)
	{
		SideA->Gambatte->reset();

		if(SideB)
		{
			SideB->Gambatte->reset();
		}
	}
	else if(!SideB && cmd == MDFN_MSC_SELECT_DISK)
	{
		SideB = new GameBoy(ROMData, ROMSize, 1);
		NeedToClearFrameBuffer = true;
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
static const InputPortInfoStruct PortInfo[] =		{{0, "builtin", "Built-In", 1, InputDeviceInfo, "gamepad"}};
static InputInfoStruct GmbtInput =					{1, PortInfo};

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
/*	InstallReadPatch:*/	GmbtInstallReadPatch,
/*	RemoveReadPatches:*/GmbtRemoveReadPatches,
/*	MemRead:*/			GmbtMemRead,
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
	return &GmbtInfo;
}
