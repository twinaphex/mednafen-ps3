#include <src/mednafen.h>
#include <src/git.h>

#include <sstream>

#include "gambatte.h"
#include "resample/resamplerinfo.h"

using namespace Gambatte;

#include "ps3_system.h"
extern PathBuild Paths;

namespace
{
	EmulateSpecStruct*			ESpec;

	class gbblitter : public VideoBlitter
	{
		public:
								gbblitter				()
			{
				buffer.pixels = 0;
			}
			
								~gbblitter				()
			{
				if(buffer.pixels)
				{
					free(buffer.pixels);
				}
			}
		
			void				setBufferDimensions		(unsigned aWidth, unsigned aHeight)
			{
				if(buffer.pixels)
				{
					free(buffer.pixels);
				}
				
				width = aWidth;
				height = aHeight;
			
				buffer.pixels = malloc(aWidth * aHeight * 4);
				buffer.pitch = aWidth;
			}
			
			const PixelBuffer	inBuffer				()
			{
				return buffer;
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
	gbblitter* 					blitter;
	gbinput*					input;
	
	uint32_t					samples[48000];
	uint32_t					resamples[48000];
	int32_t						sampleoverflow;

	uint8_t*					Ports[4] = {0, 0, 0, 0};
}

int				GmbtLoad				(const char *name, MDFNFILE *fp)
{
	gambatte = new GB();
	blitter = new gbblitter();
	input = new gbinput();
	resampler = ResamplerInfo::get(0).create(2097152, 48000, 35112);
	
	//TODO: Move it
	gambatte->set_savedir(Paths.Build("").c_str());
	
	sampleoverflow = 0;
	memset(samples, 0, sizeof(samples));
	memset(resamples, 0, sizeof(resamples));

	std::istringstream file(std::string((const char*)fp->data, (size_t)fp->size), std::ios_base::in | std::ios_base::binary);	
	gambatte->load(file, false);
	gambatte->setVideoBlitter(blitter);
	gambatte->setInputStateGetter(input);
	
	return 1;
}

bool			GmbtTestMagic			(const char *name, MDFNFILE *fp)
{
	static const uint8 GBMagic[8] = { 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B };

	if(fp->size < 0x10C || memcmp(fp->data + 0x104, GBMagic, 8))
		return(FALSE);

	return(TRUE);
}

void			GmbtCloseGame			(void)
{
	delete resampler;
	delete input;
	delete blitter;
	delete gambatte;
}

bool			GmbtToggleLayer			(int which)
{
	//TODO:
	return false;
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
	return 0;
}

void			GmbtEmulate				(EmulateSpecStruct *espec)
{
	ESpec = espec;

	if(espec->SoundFormatChanged)
	{
		resampler->adjustRate(2097152, espec->SoundRate);
	}
	
	//TODO: Support color shift

	if(Ports[0])
	{
		input->inputs.startButton = (Ports[0][0] & 8) ? 1 : 0;
		input->inputs.selectButton = (Ports[0][0] & 4) ? 1 : 0;
		input->inputs.bButton = (Ports[0][0] & 2) ? 1 : 0;
		input->inputs.aButton = (Ports[0][0] & 1) ? 1 : 0;
		input->inputs.dpadUp = (Ports[0][0] & 0x40) ? 1 : 0;
		input->inputs.dpadDown = (Ports[0][0] & 0x80) ? 1 : 0;
		input->inputs.dpadLeft = (Ports[0][0] & 0x20) ? 1 : 0;
		input->inputs.dpadRight = (Ports[0][0] & 0x10) ? 1 : 0;
	}

	uint32_t samps = gambatte->runFor((Gambatte::uint_least32_t*)samples, 35112 - sampleoverflow);
	sampleoverflow += samps;
	sampleoverflow -= 35112;
	uint32_t count = resampler->resample((short*)resamples, (short*)samples, samps);

	if(espec->SoundBuf && (espec->SoundBufMaxSize > espec->SoundRate / 60))
	{
		espec->SoundBufSize = count;
		memcpy(espec->SoundBuf, resamples, espec->SoundBufSize * 4);
	}
	
	espec->DisplayRect.x = 0;
	espec->DisplayRect.y = 0;
	espec->DisplayRect.w = 160;
	espec->DisplayRect.h = 144;
}

void			GmbtSetInput			(int port, const char *type, void *ptr)
{
	if(port == 0)
	{
		Ports[port] = (uint8_t*)ptr;
	}
}

void			GmbtDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		gambatte->reset();
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		gambatte->reset();
	}
}

static const InputDeviceInputInfoStruct IDII[] =
{
	{ "a", "A", 		/*VIRTB_1,*/ 7, IDIT_BUTTON_CAN_RAPID, NULL },
	{ "b", "B", 		/*VIRTB_0,*/ 6, IDIT_BUTTON_CAN_RAPID, NULL },
	{ "select", "SELECT",	/*VIRTB_SELECT,*/ 4, IDIT_BUTTON, NULL },
	{ "start", "START",	/*VIRTB_START,*/ 5, IDIT_BUTTON, NULL },
	{ "right", "RIGHT",	/*VIRTB_DP0_R,*/ 3, IDIT_BUTTON, "left" },
	{ "left", "LEFT",	/*VIRTB_DP0_L,*/ 2, IDIT_BUTTON, "right" },
	{ "up", "UP", 	/*VIRTB_DP0_U,*/ 0, IDIT_BUTTON, "down" },
	{ "down", "DOWN",	/*VIRTB_DP0_D,*/ 1, IDIT_BUTTON, "up" },
};

static InputDeviceInfoStruct InputDeviceInfo[] =
{
	{
		"gamepad",
		"Gamepad",
		NULL,
		sizeof(IDII) / sizeof(InputDeviceInputInfoStruct),
		IDII,
	}
};

static const InputPortInfoStruct PortInfo[] =
{
	{ 0, "builtin", "Built-In", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" }
};

static InputInfoStruct GmbtInput =
{
	sizeof(PortInfo) / sizeof(InputPortInfoStruct),
	PortInfo
};


static FileExtensionSpecStruct	extensions[] = 
{
	{".gb", "Game Boy Rom"},
	{".gbc", "Game Boy Color Rom"},	
	{".cgb", "Game Boy Color Rom"},		
	{0, 0}
};


static MDFNSetting GmbtSettings[] =
{
	{0}
};


static MDFNGI	GmbtInfo = 
{
	shortname:			"gmbt",
	fullname:			"Gambatte Game Boy Color",
	FileExtensions:		extensions,
	ModulePriority:		MODPRIO_EXTERNAL_HIGH,
	Debugger:			0,
	InputInfo:			&GmbtInput,

	Load:				GmbtLoad,
	TestMagic:			GmbtTestMagic,
	LoadCD:				0,
	TestMagicCD:		0,
	CloseGame:			GmbtCloseGame,
	ToggleLayer:		GmbtToggleLayer,
	LayerNames:			"Background\0Sprites\0Window\0",
	InstallReadPatch:	GmbtInstallReadPatch,
	RemoveReadPatches:	GmbtRemoveReadPatches,
	MemRead:			GmbtMemRead,
	StateAction:		GmbtStateAction,
	Emulate:			GmbtEmulate,
	SetInput:			GmbtSetInput,
	DoSimpleCommand:	GmbtDoSimpleCommand,
	Settings:			GmbtSettings,
	MasterClock:		0,
	fps:				0,
	multires:			false,
	nominal_width:		160,
	nominal_height:		144,
	fb_width:			256,
	fb_height:			144,
	soundchan:			2
};
	
MDFNGI* GetGambatte()
{
	return &GmbtInfo;
}
