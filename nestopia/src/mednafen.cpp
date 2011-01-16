#include <src/mednafen.h>
#include <src/mempatcher.h>
#include <src/git.h>
#include <src/general.h>
#include <src/driver.h>

#include <fstream>
#include <sstream>
#include "core/api/NstApiEmulator.hpp"
#include "core/api/NstApiVideo.hpp"
#include "core/api/NstApiSound.hpp"
#include "core/api/NstApiInput.hpp"
#include "core/api/NstApiMachine.hpp"
#include "core/api/NstApiUser.hpp"
#include "core/api/NstApiNsf.hpp"
#include "core/api/NstApiMovie.hpp"
#include "core/api/NstApiFds.hpp"
#include "core/api/NstApiRewinder.hpp"
#include "core/api/NstApiCartridge.hpp"
#include "core/api/NstApiCheats.hpp"
#include "core/NstMachine.hpp"
#include "core/NstCrc32.hpp"
#include "core/NstChecksum.hpp"
#include "core/NstXml.hpp"

using namespace Nes::Api;
using namespace Nes;

namespace
{
	EmulateSpecStruct*			ESpec;

	bool						GameOpen = false;

	Nes::Api::Emulator			Nestopia;
	Video::Output 				EmuVideo;
	Sound::Output 				EmuSound;
	Input::Controllers			EmuPads;

	uint32_t					Samples[48000];
	uint8_t*					Ports[Input::NUM_PADS] = {0};

	struct						_tagSettings
	{
		bool					NeedRefresh;
		bool					ClipSides;
		uint32_t				ScanLineStart;
		uint32_t				ScanLineEnd;
		std::string				FDSBios;
		bool					EnableNTSC;
		uint32_t				NTSCMode;
		bool					DisableSpriteLimit;
	}	NestopiaSettings;

	void						GetSettings						(const char* aName = 0)
	{
		NestopiaSettings.NeedRefresh = true;

		NestopiaSettings.ClipSides = MDFN_GetSettingB("nest.clipsides");
		NestopiaSettings.ScanLineStart = MDFN_GetSettingUI("nest.slstart");
		NestopiaSettings.ScanLineEnd = MDFN_GetSettingUI("nest.slend");
		NestopiaSettings.FDSBios = MDFN_GetSettingS("nest.fdsbios");
		NestopiaSettings.EnableNTSC = MDFN_GetSettingB("nest.ntsc");
		NestopiaSettings.NTSCMode = MDFN_GetSettingUI("nest.ntscmode");
		NestopiaSettings.DisableSpriteLimit = MDFN_GetSettingB("nest.nospritelmt");
	}
}

static void						LoadCartDatabase				()
{
	if(!Cartridge::Database(Nestopia).IsLoaded())
	{
		std::ifstream database(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, "NstDatabase.xml").c_str(), std::ifstream::binary);

		if(database.is_open())
		{
			if(NES_FAILED(Cartridge::Database(Nestopia).Load(database)))
			{
				MDFND_Message("nest: Couldn't load cartridge database");
			}
			else if(NES_FAILED(Cartridge::Database(Nestopia).Enable(true)))
			{
				MDFND_Message("nest: Couldn't enable cartridge database");
			}
			else
			{
				MDFND_Message("nest: Nestopia Cartridge Database opened");
			}
		}
		else
		{
			MDFND_Message("nest: Nestopia Cartridge Database not found");
		}
	}
}

static void						LoadFDSBios						()
{
	if(!Fds(Nestopia).HasBIOS())
	{
		std::ifstream bios(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, NestopiaSettings.FDSBios.c_str()).c_str(), std::ifstream::binary);

		if(bios.is_open())
		{
			if(NES_FAILED(Fds(Nestopia).SetBIOS(&bios)))
			{
				MDFND_Message("nest: Couldn't set FDS bios");
			}
			else
			{
				MDFND_Message("nest: FDS BIOS opened");
			}
		}
		else
		{
			MDFND_Message("nest: FDS BIOS not found");
		}
	}
}

static void NST_CALLBACK		DoLog							(void *userData, const char *string, Nes::ulong length)
{
	char buffer[1024];
	snprintf(buffer, 1024, "nest: %s", string);
	MDFND_Message(buffer);
}

//TODO: Fill this in properly, support compression on save games
static void NST_CALLBACK		DoFileIO						(void *userData, User::File& file)
{
	if(file.GetAction() == User::File::LOAD_BATTERY || file.GetAction() == User::File::LOAD_EEPROM || file.GetAction() == User::File::LOAD_TAPE || file.GetAction() == User::File::LOAD_TURBOFILE)
	{
		std::ifstream savefile(MDFN_MakeFName(MDFNMKF_SAV, 0, "sav").c_str(), std::ifstream::in | std::ifstream::binary );

		if (savefile.is_open())
		{
			file.SetContent(savefile);
		}

		return;
	}	

	if(file.GetAction() == User::File::SAVE_BATTERY || file.GetAction() == User::File::SAVE_EEPROM || file.GetAction() == User::File::SAVE_TAPE || file.GetAction() == User::File::LOAD_TURBOFILE)
	{
		std::ofstream batteryFile(MDFN_MakeFName(MDFNMKF_SAV, 0, "sav").c_str(), std::ifstream::out | std::ifstream::binary );

		const void* savedata;
		unsigned long savedatasize;

		file.GetContent(savedata, savedatasize);

		if (batteryFile.is_open())
		{
			batteryFile.write((const char*) savedata, savedatasize);
		}
		return;
	}	

	if(file.GetAction() == User::File::LOAD_FDS)
	{
		std::ifstream batteryFile(MDFN_MakeFName(MDFNMKF_SAV, 0, "ups").c_str(), std::ifstream::in | std::ifstream::binary);

		if(batteryFile.is_open())
		{
			file.SetPatchContent(batteryFile);
		}

		return;
	}

	if(file.GetAction() == User::File::SAVE_FDS)
	{
		std::ofstream fdsFile(MDFN_MakeFName(MDFNMKF_SAV, 0, "ups").c_str(), std::ifstream::out | std::ifstream::binary);

		if (fdsFile.is_open())
		{
			file.GetPatchContent(User::File::PATCH_UPS, fdsFile);
		}

		return;
	}
}

int				NestLoad				(const char *name, MDFNFILE *fp);
bool			NestTestMagic			(const char *name, MDFNFILE *fp);
void			NestCloseGame			(void);
bool			NestToggleLayer			(int which);
void			NestInstallReadPatch	(uint32 address);
void			NestRemoveReadPatches	(void);
uint8			NestMemRead				(uint32 addr);
int				NestStateAction			(StateMem *sm, int load, int data_only);
void			NestEmulate				(EmulateSpecStruct *espec);
void			NestSetInput			(int port, const char *type, void *ptr);
void			NestDoSimpleCommand		(int cmd);

int				NestLoad				(const char *name, MDFNFILE *fp)
{
	if(GameOpen)
	{
		NestCloseGame();
	}

	GameOpen = true;

	//Get the settings
	GetSettings();

	//Load external files
	LoadCartDatabase();
	LoadFDSBios();

	//Setup some nestopia callbacks
	User::fileIoCallback.Set(DoFileIO, 0);
	User::logCallback.Set(DoLog, 0);

	//Make stream from file in memory, and load it
	std::istringstream file(std::string((const char*)fp->data, (size_t)fp->size), std::ios_base::in | std::ios_base::binary);
	if(NES_FAILED(Machine(Nestopia).Load(file, Machine::FAVORED_NES_NTSC, Machine::DONT_ASK_PROFILE)))
	{
		MDFND_PrintError("nest: Failed to load game");
		return 0;
	}

	if(Machine(Nestopia).Is(Machine::DISK))
	{
		if(NES_FAILED(Fds(Nestopia).InsertDisk(0, 0)))
		{
			MDFND_PrintError("nest: Failed to insert FDS disk");
			return 0;
		}
	}

	//TODO: Support more controllers
	Input(Nestopia).ConnectController(0, Input::PAD1);
	Input(Nestopia).ConnectController(1, Input::PAD2);

	//Here we go
	if(NES_FAILED(Machine(Nestopia).Power(true)))
	{
		MDFND_PrintError("nest: Failed to power on NES");
		return 0;
	}

	return 1;
}

bool			NestTestMagic			(const char *name, MDFNFILE *fp)
{
	return(fp->size > 16 && (!memcmp(fp->data, "NES\x1a", 4) || !memcmp(fp->data, "FDS\x1a", 4) || !memcmp(fp->data + 1, "*NINTENDO-HVC*", 14) || !memcmp(fp->data, "UNIF", 4)));
}

void			NestCloseGame			(void)
{
	if(GameOpen)
	{
		Machine(Nestopia).Power(false);
		Machine(Nestopia).Unload();

		GameOpen = false;
	}
}

void			NestInstallReadPatch	(uint32 address)
{
	//TODO:
}

void			NestRemoveReadPatches	(void)
{
	//TODO:
}

uint8			NestMemRead				(uint32 addr)
{
	//TODO:
	return 0;
}

int				NestStateAction			(StateMem *sm, int load, int data_only)
{
	if(!load)
	{
		std::ostringstream os(std::ios_base::out | std::ios_base::binary);
		Nes::Result res = Machine(Nestopia).SaveState(os, Nes::Api::Machine::USE_COMPRESSION);

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
		Nes::Result res = Machine(Nestopia).LoadState(iss);

		free(buffer);

		return 1;
	}
	return 0;
}

void			NestEmulate				(EmulateSpecStruct *espec)
{
	ESpec = espec;

	//Update sound
	if(espec->SoundFormatChanged)
	{
		EmuSound.samples[0] = (void*)Samples;
		EmuSound.length[0] = espec->SoundRate / 60;
		EmuSound.samples[1] = NULL;
		EmuSound.length[1] = 0;

		Sound(Nestopia).SetSampleBits(16);
		Sound(Nestopia).SetSampleRate(espec->SoundRate);
		Sound(Nestopia).SetVolume(Sound::ALL_CHANNELS, 100);
		Sound(Nestopia).SetSpeaker(Sound::SPEAKER_MONO);
	}

	//Update sprite limit
	Video(Nestopia).EnableUnlimSprites(NestopiaSettings.DisableSpriteLimit);

	//Update colors
	//TODO: Support 16-bit and YUV
	if(espec->VideoFormatChanged || NestopiaSettings.NeedRefresh)
	{
		NestopiaSettings.NeedRefresh = false;

		if(NestopiaSettings.NTSCMode == 0)
		{
			Video(Nestopia).SetSharpness(Video::DEFAULT_SHARPNESS_COMP);
			Video(Nestopia).SetColorResolution(Video::DEFAULT_COLOR_RESOLUTION_COMP);
			Video(Nestopia).SetColorBleed(Video::DEFAULT_COLOR_BLEED_COMP);
			Video(Nestopia).SetColorArtifacts(Video::DEFAULT_COLOR_ARTIFACTS_COMP);
			Video(Nestopia).SetColorFringing(Video::DEFAULT_COLOR_FRINGING_COMP);
		}
		else if(NestopiaSettings.NTSCMode == 1)
		{
			Video(Nestopia).SetSharpness(Video::DEFAULT_SHARPNESS_SVIDEO);
			Video(Nestopia).SetColorResolution(Video::DEFAULT_COLOR_RESOLUTION_SVIDEO);
			Video(Nestopia).SetColorBleed(Video::DEFAULT_COLOR_BLEED_SVIDEO);
			Video(Nestopia).SetColorArtifacts(Video::DEFAULT_COLOR_ARTIFACTS_SVIDEO);
			Video(Nestopia).SetColorFringing(Video::DEFAULT_COLOR_FRINGING_SVIDEO);
		}
		else
		{
			Video(Nestopia).SetSharpness(Video::DEFAULT_SHARPNESS_RGB);
			Video(Nestopia).SetColorResolution(Video::DEFAULT_COLOR_RESOLUTION_RGB);
			Video(Nestopia).SetColorBleed(Video::DEFAULT_COLOR_BLEED_RGB);
			Video(Nestopia).SetColorArtifacts(Video::DEFAULT_COLOR_ARTIFACTS_RGB);
			Video(Nestopia).SetColorFringing(Video::DEFAULT_COLOR_FRINGING_RGB);
		}

	    Video::RenderState renderState;
    	renderState.bits.count = 32;
	    renderState.bits.mask.r = 0xFF << espec->surface->format.Rshift;
    	renderState.bits.mask.g = 0xFF << espec->surface->format.Gshift;
	    renderState.bits.mask.b = 0xFF << espec->surface->format.Bshift;
		renderState.filter = NestopiaSettings.EnableNTSC ? Video::RenderState::FILTER_NTSC : Video::RenderState::FILTER_NONE;
		renderState.width = NestopiaSettings.EnableNTSC ? Video::Output::NTSC_WIDTH : Video::Output::WIDTH;
		renderState.height = Video::Output::HEIGHT;

    	if(NES_FAILED(Video(Nestopia).SetRenderState(renderState)))
		{
			MDFND_PrintError("nest: Failed to set render state");
			//TODO: Abort, throw, or ignore?
		}
	}

	//Update video
	EmuVideo.pixels = ESpec->surface->pixels;
	EmuVideo.pitch = ESpec->surface->pitch32 * 4;

	//Update input
	//TODO: Support more controllers
	for(int i = 0; i != Input::NUM_PADS; i ++)
	{
		EmuPads.pad[i].buttons = Ports[i] ? Ports[i][0] : 0;
	}

	//Do frame
	Nestopia.Execute(espec->skip ? 0 : &EmuVideo, &EmuSound, &EmuPads);

	//Copy sound
	//TODO: This can't possibly be the correct way to calculate the number of samples, can it?
	if(espec->SoundBuf && (espec->SoundBufMaxSize > espec->SoundRate / 60))
	{
		espec->SoundBufSize = espec->SoundRate / 60;
		memcpy(espec->SoundBuf, Samples, espec->SoundBufSize * 2);
	}

	//Set video. No Clipping on NTSC
	uint32_t widthhelp = NestopiaSettings.EnableNTSC ? 0 : 8;
	espec->DisplayRect.x = NestopiaSettings.ClipSides ? widthhelp : 0;
	espec->DisplayRect.y = NestopiaSettings.ScanLineStart;
	espec->DisplayRect.w = (NestopiaSettings.EnableNTSC ? Video::Output::NTSC_WIDTH : Video::Output::WIDTH) - (NestopiaSettings.ClipSides ? widthhelp : 0);
	espec->DisplayRect.h = NestopiaSettings.ScanLineEnd - NestopiaSettings.ScanLineStart;

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

void			NestSetInput			(int port, const char *type, void *ptr)
{
	if(port >= 0 && port < Input::NUM_PADS)
	{
		Ports[port] = (uint8_t*)ptr;
	}
}

void			NestDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		Machine(Nestopia).Reset(0);

		if(Machine(Nestopia).Is(Machine::DISK))
		{
			Fds(Nestopia).EjectDisk();
			Fds(Nestopia).InsertDisk(0, 0);
		}
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		Machine(Nestopia).Reset(1);

		if(Machine(Nestopia).Is(Machine::DISK))
		{
			Fds(Nestopia).EjectDisk();
			Fds(Nestopia).InsertDisk(0, 0);
		}
	}
	else if(cmd == MDFN_MSC_SELECT_DISK)
	{
		if(Machine(Nestopia).Is(Machine::DISK) && Fds(Nestopia).CanChangeDiskSide())
		{
			if(!NES_FAILED(Fds(Nestopia).ChangeSide()))
			{
				MDFND_DispMessage((UTF8*)"Disk Side Changed");
			}
			else
			{
				MDFND_DispMessage((UTF8*)"Error changing disk side");
			}
		}
	}
}


//TODO: Define more control types
static const InputDeviceInputInfoStruct GamepadIDII[] =
{
	{"a",		"A",		7, IDIT_BUTTON_CAN_RAPID,	NULL},
	{"b",		"B",		6, IDIT_BUTTON_CAN_RAPID,	NULL},
	{"select",	"SELECT",	4, IDIT_BUTTON,				NULL},
	{"start",	"START",	5, IDIT_BUTTON,				NULL},
	{"up",		"UP",		0, IDIT_BUTTON,				"down"},
	{"down",	"DOWN",		1, IDIT_BUTTON,				"up"},
	{"left",	"LEFT",		2, IDIT_BUTTON,				"right"},
	{"right",	"RIGHT",	3, IDIT_BUTTON,				"left"},
};

static InputDeviceInfoStruct InputDeviceInfoNESPort[] =
{
	{"none",	"none",		NULL, 0,														NULL},
	{"gamepad",	"Gamepad",	NULL, sizeof(GamepadIDII) / sizeof(InputDeviceInputInfoStruct), GamepadIDII},
};


static const InputPortInfoStruct PortInfo[] =
{
	{0, "port1", "Port 1", sizeof(InputDeviceInfoNESPort) / sizeof(InputDeviceInfoStruct), InputDeviceInfoNESPort, "gamepad"},
};

InputInfoStruct		NestInput =
{
	sizeof(PortInfo) / sizeof(InputPortInfoStruct),
	PortInfo
};


FileExtensionSpecStruct	extensions[] =
{
	{".nes",	"iNES Format ROM Image"},
	{".nez",	"iNES Format ROM Image"},
	{".fds",	"Famicom Disk System Disk Image"},
	{".unf",	"UNIF Format ROM Image"},
	{".unif",	"UNIF Format ROM Image"},
	{0, 0}
};

const MDFNSetting_EnumList	NTSCTypes[] =
{
	{"Composite", 0, "Composite", ""},
	{"SVIDEO", 1, "SVIDEO", ""},
	{"RGB", 2, "RGB", ""},
	{0, 0, 0, 0}
};

static MDFNSetting NestSettings[] =
{
	{"nest.clipsides",	MDFNSF_NOFLAGS,	"Clip left+right 8 pixel columns.",			NULL, MDFNST_BOOL,	"0",			0,		0,		0, GetSettings},
	{"nest.slstart",	MDFNSF_NOFLAGS,	"First displayed scanline in NTSC mode.",	NULL, MDFNST_UINT,	"8",			"0",	"239",	0, GetSettings},
	{"nest.slend",		MDFNSF_NOFLAGS,	"Last displayed scanlines in NTSC mode.",	NULL, MDFNST_UINT,	"231",			"0",	"239",	0, GetSettings},
	{"nest.fdsbios",	MDFNSF_NOFLAGS,	"Path to FDS BIOS.",						NULL, MDFNST_STRING,"disksys.rom",	0,		0,		0, GetSettings},
	{"nest.ntsc",		MDFNSF_NOFLAGS, "Enable the NTSC filter",					NULL, MDFNST_BOOL,	"0",			0,		0,		0, GetSettings},
	{"nest.ntscmode",	MDFNSF_NOFLAGS, "Type of NTSC filter",						NULL, MDFNST_ENUM,	"Composite",	0,		0,		0, GetSettings,	NTSCTypes},
	{"nest.nospritelmt",MDFNSF_NOFLAGS, "Disable NES Sprite limit",					NULL, MDFNST_BOOL,	"0",			0,		0,		0, GetSettings},
	{NULL}
};


//TODO: Masterclock and fps
MDFNGI	NestInfo =
{
/*	shortname:			*/	"nest",
/*	fullname:			*/	"Nintendo Entartainment System (Nestopia)",
/*	FileExtensions:		*/	extensions,
/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
/*	Debugger:			*/	0,
/*	InputInfo:			*/	&NestInput,

/*	Load:				*/	NestLoad,
/*	TestMagic:			*/	NestTestMagic,
/*	LoadCD:				*/	0,
/*	TestMagicCD:		*/	0,
/*	CloseGame:			*/	NestCloseGame,
/*	ToggleLayer:		*/	0,
/*	LayerNames:			*/	0,
/*	InstallReadPatch:	*/	NestInstallReadPatch,
/*	RemoveReadPatches:	*/	NestRemoveReadPatches,
/*	MemRead:			*/	NestMemRead,
/*	StateAction:		*/	NestStateAction,
/*	Emulate:			*/	NestEmulate,
/*	SetInput:			*/	NestSetInput,
/*	DoSimpleCommand:	*/	NestDoSimpleCommand,
/*	Settings:			*/	NestSettings,
/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
/*	fps:				*/	60,
/*	multires:			*/	false,
/*	lcm_width:			*/	256,
/*	lcm_height:			*/	240,
/*  dummy_separator:	*/	0,
/*	nominal_width:		*/	256,
/*	nominal_height:		*/	240,
/*	fb_width:			*/	1024,
/*	fb_height:			*/	512,
/*	soundchan:			*/	1
};

MDFNGI* GetNestopia()
{
	return &NestInfo;
}

