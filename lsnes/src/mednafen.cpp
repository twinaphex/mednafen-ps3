#include <src/mednafen.h>
#include <src/git.h>
#include <src/driver.h>
#include <src/general.h>
#include <src/mempatcher.h>
#include <src/md5.h>
#include <include/Fir_Resampler.h>

#include <stdio.h>

#include "libsnes.hpp"

//SYSTEM
namespace lsnes
{
	struct						MDFNlsnes
	{
								MDFNlsnes() : ESpec(0), SampleBufferCount(0), Resampler(0) {}
								~MDFNlsnes() {delete Resampler;}

		EmulateSpecStruct*		ESpec;
		uint8_t*				Ports[4];

		//Audio values
		static const uint32_t	SampleBufferSize = 3200;
		uint32_t				SampleBufferCount;
		Fir_Resampler<8>*		Resampler;
	};

	MDFNlsnes*				mdfnLsnes;

	int						DetermineGameType		(MDFNFILE* fp)
	{
		if(MDFN_GetSettingB("lsnes.usesupergb"))
		{
			static const uint8 GBMagic[8] = { 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B };
			if(fp->size > 0x10C && !memcmp(fp->data + 0x104, GBMagic, 8))
			{
				return 1;
			}
		}

		//Straight from mednafen
		return (!(strcasecmp(fp->ext, "smc") && strcasecmp(fp->ext, "swc") && strcasecmp(fp->ext, "sfc") && strcasecmp(fp->ext, "fig") && strcasecmp(fp->ext, "bs") && strcasecmp(fp->ext, "st"))) ? 0 : -1;
	}

	void					VideoRefreshCallback	(const uint16_t *data, unsigned width, unsigned height)
	{
		assert(data && width && height);
		assert(mdfnLsnes && mdfnLsnes->ESpec && mdfnLsnes->ESpec->surface && mdfnLsnes->ESpec->surface->pixels);

		//Get a pointer to the target buffer
		uint32_t* destimage = (uint32_t*)mdfnLsnes->ESpec->surface->pixels;

		//Copy entire image
		for(int i = 0; i != height && i != mdfnLsnes->ESpec->surface->h; i ++)
		{
			for(int j = 0; j != width && j != mdfnLsnes->ESpec->surface->w; j ++)
			{
				//Convert 16-bit->32-bit color
				uint16_t source = data[i * 1024 + j];	//TODO: Is it always 1024?
				uint8_t a = (source & 0x3F) << 3;
				uint8_t b = ((source >> 5) & 0x3F) << 3;
				uint8_t c = ((source >> 10) & 0x3F) << 3;
				destimage[i * mdfnLsnes->ESpec->surface->pitchinpix + j] = a | b << 8 | c << 16;
			}
		}

		//Set the output rectangle
		mdfnLsnes->ESpec->DisplayRect.x = 0;
		mdfnLsnes->ESpec->DisplayRect.y = 0;
		mdfnLsnes->ESpec->DisplayRect.w = width;
		mdfnLsnes->ESpec->DisplayRect.h = height;
	}

	void					AudioSampleCallback		(uint16_t left, uint16_t right)
	{
		assert(mdfnLsnes);

		if(mdfnLsnes->SampleBufferCount ++ < mdfnLsnes->SampleBufferSize)
		{
			if(mdfnLsnes->Resampler && mdfnLsnes->Resampler->max_write() >= 2)
			{
				mdfnLsnes->Resampler->buffer()[0] = left;
				mdfnLsnes->Resampler->buffer()[1] = right;
				mdfnLsnes->Resampler->write(2);
			}
		}
		else
		{
			MDFN_PrintError("libsnes: SampleBuffer overflow!\n");
		}
	}

	int16_t					InputStateCallback		(bool port, unsigned device, unsigned index, unsigned id)
	{
		assert(mdfnLsnes);

		uint8_t* thisPort = mdfnLsnes->Ports[port ? 1 : 0];
		uint16_t portData = thisPort ? (thisPort[0] | (thisPort[1] << 8)) : 0;
		return (portData & (1 << id)) ? 1 : 0;
	}
}
using namespace lsnes;

//SYSTEM DESCRIPTIONS
static const InputDeviceInputInfoStruct		GamepadIDII[] =
{
	{ "b",		"B (center, lower)",	7,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "y",		"Y (left)",				6,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "select",	"SELECT",				4,	IDIT_BUTTON,			NULL	},
	{ "start",	"START",				5,	IDIT_BUTTON,			NULL	},
	{ "up",		"UP ↑",					0,	IDIT_BUTTON,			"down"	},
	{ "down",	"DOWN ↓",				1,	IDIT_BUTTON,			"up"	},
	{ "left",	"LEFT ←",				2,	IDIT_BUTTON,			"right"	},
	{ "right",	"RIGHT →",				3,	IDIT_BUTTON,			"left"	},
	{ "a",		"A (right)",			9,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "x",		"X (center, upper)",	8,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	{ "l",		"Left Shoulder",		10,	IDIT_BUTTON,			NULL	},
	{ "r",		"Right Shoulder",		11,	IDIT_BUTTON,			NULL	},
};

static InputDeviceInfoStruct 				InputDeviceInfo[] =
{
	{"none",	"none",		NULL,	0,															NULL			},
	{"gamepad", "Gamepad",	NULL,	sizeof(GamepadIDII) / sizeof(InputDeviceInputInfoStruct),	GamepadIDII,	},
};

static const InputPortInfoStruct PortInfo[] =
{
	{0, "port1", "Port 1", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" },
	{0, "port2", "Port 2", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" },
};

static InputInfoStruct 						lsnesInput =
{
	sizeof(PortInfo) / sizeof(InputPortInfoStruct), PortInfo
};

static MDFNSetting							lsnesSettings[] =
{
	{"lsnes.sgbbios",		MDFNSF_EMU_STATE,	"Path to Super Game Boy ROM image.",	NULL,	MDFNST_STRING,	"supergb.bin"},
	{"lsnes.usesupergb",	MDFNSF_EMU_STATE,	"Enable use of Super Game Boy.",		NULL,	MDFNST_BOOL,	"0"},
	{NULL}
};

static FileExtensionSpecStruct				lsnesExtensions[] =
{
	{".smc",	"Super Magicom ROM Image"			},
	{".swc",	"Super Wildcard ROM Image"			},
	{".sfc",	"Cartridge ROM Image"				},
	{".fig",	"Cartridge ROM Image"				},

	{".bs",		"BS-X EEPROM Image"					},
	{".st",		"Sufami Turbo Cartridge ROM Image"	},

	{NULL, NULL}
};

//Implement MDFNGI:
static int			lsnesLoad				(const char *name, MDFNFILE *fp)
{
	mdfnLsnes = new MDFNlsnes();

	//Get Game MD5
	md5_context md5;
	md5.starts();
	md5.update(fp->data, fp->size);
	md5.finish(MDFNGameInfo->MD5);

	//Setup libsnes
	snes_init();
	snes_set_video_refresh(VideoRefreshCallback);
	snes_set_audio_sample(AudioSampleCallback);
	snes_set_input_state(InputStateCallback);

	int type = DetermineGameType(fp);

	if(type < 0 || type > 1)
	{
		MDFN_printf("libsnes: Unrecognized game type\n");
		return 0;
	}

	//Load game
	if(type == 0 && !snes_load_cartridge_normal(0, fp->data, fp->size))
	{
		MDFN_printf("libsnes: Failed to load game\n");
		return 0;
	}
	else if(type == 1)
	{
		MDFNFILE sgbrom;
		if(sgbrom.Open(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("lsnes.sgbbios").c_str()).c_str(), 0))
		{
			if(!snes_load_cartridge_super_game_boy(0, sgbrom.data, sgbrom.size, 0, fp->data, fp->size))
			{
				MDFN_printf("libsnes: Could not load Super Game Boy Game\n");
				return 0;
			}
		}
		else
		{
			MDFN_printf("libsnes: Could not load Super Game Boy BIOS\n");
			return 0;
		}
	}

	//Setup region
	bool PAL = snes_get_region() != SNES_REGION_NTSC;
	MDFNGameInfo->fps = PAL ? 838977920 : 1008307711;				//These lines taken from mednafen, plus that comment down there too
	MDFNGameInfo->MasterClock = MDFN_MASTERCLOCK_FIXED(32040.40);	//MDFN_MASTERCLOCK_FIXED(21477272);	//PAL ? PAL_CPU : NTSC_CPU);
	MDFNGameInfo->nominal_height = PAL ? 239 : 224;
	MDFNGameInfo->lcm_height = MDFNGameInfo->nominal_height * 2;
	
	//Load save
	unsigned save_size = snes_get_memory_size(SNES_MEMORY_CARTRIDGE_RAM);
	uint8_t* save_data = snes_get_memory_data(SNES_MEMORY_CARTRIDGE_RAM);
	if(save_size && save_data)
	{
		std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
		FILE* save_file = fopen(filename.c_str(), "rb");
		if(save_file)
		{
			fseek(save_file, 0, SEEK_END);
			if(ftell(save_file) == save_size)
			{
				fseek(save_file, 0, SEEK_SET);
				if(save_size != fread(save_data, 1, save_size, save_file))
				{
					MDFN_PrintError("libsnes: Failed to read entire save game?\n");
				}
			}
			else
			{
				MDFN_PrintError("libsnes: Save file incorrect size, expected %u got %ld\n", save_size, ftell(save_file));
			}

			fclose(save_file);
		}
	}

	//Start emulator
	snes_power();

	return 1;
}

static bool			lsnesTestMagic			(const char *name, MDFNFILE *fp)
{
	DetermineGameType(fp) >= 0;
}

static void			lsnesCloseGame			(void)
{
	//Write save
	unsigned save_size = snes_get_memory_size(SNES_MEMORY_CARTRIDGE_RAM);
	uint8_t* save_data = snes_get_memory_data(SNES_MEMORY_CARTRIDGE_RAM);
	if(save_size && save_data)
	{
		std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
		FILE* save_file = fopen(filename.c_str(), "wb");
		if(save_file)
		{
			fwrite(save_data, 1, save_size, save_file);
			fclose(save_file);
		}
		else
		{
			MDFN_PrintError("libsnes: Failed to open save file for writing. Game will not be saved.\n");
		}
	}

	//Close libsnes
	snes_unload_cartridge();
	snes_term();

	//Clean up
	delete mdfnLsnes;
	mdfnLsnes = 0;
}

static int			lsnesStateAction		(StateMem *sm, int load, int data_only)
{
	uint32_t size = snes_serialize_size();
	uint8_t* buffer = (uint8_t*)malloc(size);
	bool result;
	
	if(load)
	{
		smem_read(sm, buffer, size);
		result = snes_unserialize(buffer, size);
	}
	else
	{
		result = snes_serialize(buffer, size);

		if(result)
		{
			smem_write(sm, buffer, size);
		}
	}

	free(buffer);
	return result;
}

static void			lsnesEmulate			(EmulateSpecStruct *espec)
{
	assert(mdfnLsnes);
	mdfnLsnes->ESpec = espec;

	//AUDIO PREP
	if(espec->SoundFormatChanged)
	{
		delete mdfnLsnes->Resampler;

		if(espec->SoundRate > 1.0)
		{
			mdfnLsnes->Resampler = new Fir_Resampler<8>();
			mdfnLsnes->Resampler->buffer_size(MDFNlsnes::SampleBufferSize * 2);
			mdfnLsnes->Resampler->time_ratio(32000.0 / espec->SoundRate, 0.9965);	//TODO: Is 32000 the right number?
		}
	}

	//EMULATE
	snes_run();

	//AUDIO
	if(mdfnLsnes->Resampler && espec->SoundBuf && espec->SoundBufMaxSize)
	{
		espec->SoundBufSize = mdfnLsnes->Resampler->read(espec->SoundBuf, mdfnLsnes->Resampler->avail()) >> 1;

		//Reset the sample buffer for the next frame
		mdfnLsnes->SampleBufferCount = 0;
	}

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

static void			lsnesSetInput			(int port, const char *type, void *ptr)
{
	assert(mdfnLsnes);

	if(port >= 0 && port <= 4)
	{
		//Search for the device
		unsigned pluggeddevice = 0;
		for(int i = 0; i != sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct); i ++)
		{
			if(strcmp(type, InputDeviceInfo[i].ShortName) == 0)
			{
				pluggeddevice = i;
			}
		}

		//Plug it in
		snes_set_controller_port_device(0, pluggeddevice);
		mdfnLsnes->Ports[port] = (uint8_t*)ptr;
	}
}

static void			lsnesDoSimpleCommand	(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		snes_reset();
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		snes_power();
	}
}

//GAME INFO
static MDFNGI				lsnesInfo =
{
/*	shortname:			*/	"lsnes",
/*	fullname:			*/	"libsnes Wrapper",
/*	FileExtensions:		*/	lsnesExtensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&lsnesInput,

/*	Load:				*/	lsnesLoad,
/*	TestMagic:			*/	lsnesTestMagic,
/*	LoadCD:				*/	0,
/*	TestMagicCD:		*/	0,
/*	CloseGame:			*/	lsnesCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
/*	StateAction:		*/	lsnesStateAction,
/*	Emulate:			*/	lsnesEmulate,
/*	SetInput:			*/	lsnesSetInput,
/*	DoSimpleCommand:	*/	lsnesDoSimpleCommand,
/*	Settings:			*/	lsnesSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	0,
/*	multires:			*/	false,
/*	lcm_width:			*/	512,
/*	lcm_height:			*/	480,
/*  dummy_separator:	*/	0,
/*	nominal_width:		*/	256,
/*	nominal_height:		*/	240,
/*	fb_width:			*/	512,
/*	fb_height:			*/	512,
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
#define VERSION_FUNC lsnesGetVersion
#define GETEMU_FUNC lsnesGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &lsnesInfo : 0;
}

