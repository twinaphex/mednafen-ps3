#include <src/mednafen.h>
#include <src/git.h>

#include <sstream>

#include "gambatte.h"
#include "resample/resamplerinfo.h"

using namespace Gambatte;

#include "ps3_system.h"

namespace
{
	EmulateSpecStruct*			ESpec;
	bool						GameLoaded = false;

	class gbblitter : public VideoBlitter
	{
		public:
								gbblitter				()			{buffer.pixels = 0;}
								~gbblitter				()			{delete[] (uint32_t*)buffer.pixels;}
			const PixelBuffer	inBuffer				()			{return buffer;}


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
						ESpec->surface->pixels[i * ESpec->surface->pitch32 + j] = sourceptr[i * width + j];
					}
				}
			}

		protected:
			uint32_t			width, height;
			PixelBuffer			buffer;
	};


	class gbinput : public InputStateGetter
	{
		public:
			const InputState& 		operator()			()	{return inputs;};

			InputState				inputs;
	};

	GB*							gambatte;
	Resampler*					resampler;
	gbblitter 					blitter;
	gbinput						input;

	uint32_t					samples[48000];
	uint32_t					resamples[48000];
	int32_t						sampleoverflow;

	uint8_t*					InputPort;
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

	//Create emulator
	gambatte = new GB();
	resampler = ResamplerInfo::get(0).create(2097152, 48000, 35112);

	//Init Sound
	sampleoverflow = 0;
	memset(samples, 0, sizeof(samples));
	memset(resamples, 0, sizeof(resamples));

	//Load game
	std::istringstream file(std::string((const char*)fp->data, (size_t)fp->size), std::ios_base::in | std::ios_base::binary);	
	if(gambatte->load(file, MDFN_GetSettingB("gmbt.forcedmg")))
	{
		delete resampler;
		delete gambatte;

		MDFND_PrintError("gambatte: Failed to load ROM");
		return 0;
	}

	//Set Input and video
	gambatte->setVideoBlitter(&blitter);
	gambatte->setInputStateGetter(&input);

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
	delete resampler;
	delete gambatte;

	resampler = 0;
	gambatte = 0;

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
		gambatte->saveState(os);

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
		gambatte->loadState(iss);

		free(buffer);

		sampleoverflow = 0;

		return 1;
	}
	return 0;
}

void			GmbtEmulate				(EmulateSpecStruct *espec)
{
	ESpec = espec;

	//Setup sound
	if(espec->SoundFormatChanged)
	{
		resampler->adjustRate(2097152, espec->SoundRate);
	}

	//TODO: Support color shift, 16-bit, yuv

	//Setup input
	if(InputPort)
	{
		input.inputs.startButton	= (*InputPort & 8) ? 1 : 0;
		input.inputs.selectButton	= (*InputPort & 4) ? 1 : 0;
		input.inputs.bButton		= (*InputPort & 2) ? 1 : 0;
		input.inputs.aButton		= (*InputPort & 1) ? 1 : 0;
		input.inputs.dpadUp			= (*InputPort & 0x40) ? 1 : 0;
		input.inputs.dpadDown		= (*InputPort & 0x80) ? 1 : 0;
		input.inputs.dpadLeft		= (*InputPort & 0x20) ? 1 : 0;
		input.inputs.dpadRight		= (*InputPort & 0x10) ? 1 : 0;
	}

	//Run frame
	uint32_t samps = gambatte->runFor((Gambatte::uint_least32_t*)samples, 35112 - sampleoverflow);
	sampleoverflow += samps;
	sampleoverflow -= 35112;

	//Grab sound
	uint32_t count = resampler->resample((short*)resamples, (short*)samples, samps);

	if(espec->SoundBuf && (espec->SoundBufMaxSize >= count))
	{
		espec->SoundBufSize = count;
		memcpy(espec->SoundBuf, resamples, espec->SoundBufSize * 4);
	}

	//Set frame size
	espec->DisplayRect.x = 0;
	espec->DisplayRect.y = 0;
	espec->DisplayRect.w = 160;
	espec->DisplayRect.h = 144;

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
		gambatte->reset();
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
/*	fullname:*/			"Gambatte Game Boy Color",
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
/*	lcm_width:*/		160,
/*	lcm_height:*/		144,
/*	dummy_separator:*/	0,
/*	nominal_width:*/	160,
/*	nominal_height:*/	144,
/*	fb_width:*/			160,
/*	fb_height:*/		144,
/*	soundchan:*/		2
};

MDFNGI* GetGambatte()
{
	return &GmbtInfo;
}
