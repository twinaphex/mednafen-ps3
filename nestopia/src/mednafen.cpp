#include <src/mednafen.h>
#include <src/git.h>
#include <src/general.h>

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
#include "core/NstCrc32.hpp"
#include "core/NstChecksum.hpp"
#include "core/NstXml.hpp"

using namespace Nes::Api;
using namespace Nes;


namespace
{
	EmulateSpecStruct*			ESpec;

	Nes::Api::Emulator			Nestopia;
	Video::Output* 				EmuVideo;
	Sound::Output* 				EmuSound;
	Input::Controllers*			EmuPads;
	
	std::string					SaveFileName;
	std::string					StateFileName;
	uint32_t					Samples[48000];
	uint8_t*					Ports[4] = {0, 0, 0, 0};
}

bool NST_CALLBACK			VideoLock						(void* userData, Video::Output& video)
{
	video.pixels = ESpec->surface->pixels;
	video.pitch = ESpec->surface->pitch32 * 4;
	return true;
}

void NST_CALLBACK 			VideoUnlock						(void* userData, Video::Output& video)
{
}

void NST_CALLBACK 			DoLog							(void *userData, const char *string, Nes::ulong length)
{
}

void NST_CALLBACK 			DoFileIO						(void *userData, User::File& file)
{
	unsigned char *compbuffer;
	int compsize, compoffset;
	char mbstr[512];

	switch (file.GetAction())
	{
		case User::File::LOAD_BATTERY: // load in battery data from a file
		case User::File::LOAD_EEPROM: // used by some Bandai games, can be treated the same as battery files
		case User::File::LOAD_TAPE: // for loading Famicom cassette tapes
		case User::File::LOAD_TURBOFILE: // for loading turbofile data
		{
			std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
		
			std::ifstream savefile(filename.c_str(), std::ifstream::in | std::ifstream::binary );

			if (savefile.is_open())
			{
				file.SetContent(savefile);
			}
		
			return;
		}

		case User::File::SAVE_BATTERY: // save battery data to a file
		case User::File::SAVE_EEPROM: // can be treated the same as battery files
		case User::File::SAVE_TAPE: // for saving Famicom cassette tapes
		case User::File::SAVE_TURBOFILE: // for saving turbofile data
		{
			std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
		
			std::ofstream batteryFile(filename.c_str(), std::ifstream::out | std::ifstream::binary );
			const void* savedata;
			unsigned long savedatasize;

			file.GetContent(savedata, savedatasize);

			if (batteryFile.is_open())
				batteryFile.write((const char*) savedata, savedatasize);
			return;		
		}
	}
}

int				NestLoad				(const char *name, MDFNFILE *fp)
{
	Video::Output::lockCallback.Set(VideoLock, 0);
	Video::Output::unlockCallback.Set(VideoUnlock, 0);
	User::fileIoCallback.Set(DoFileIO, 0);
	User::logCallback.Set(DoLog, 0);

	EmuVideo = new Video::Output;
	EmuSound = new Sound::Output;
	EmuPads  = new Input::Controllers;

	Video::RenderState renderState;
	renderState.bits.count = 32;
	renderState.bits.mask.r = 0x00ff0000;
	renderState.bits.mask.g = 0x0000ff00;
	renderState.bits.mask.b = 0x000000ff;
	renderState.filter = Video::RenderState::FILTER_NONE;
	renderState.width = 256;
	renderState.height = 240;	
	Video(Nestopia).SetRenderState(renderState);

	EmuSound->samples[0] = (void*)Samples;
	EmuSound->length[0] = 48000 / 60;
	EmuSound->samples[1] = NULL;
	EmuSound->length[1] = 0;

	std::istringstream file(std::string((const char*)fp->data, (size_t)fp->size), std::ios_base::in | std::ios_base::binary);
	Machine(Nestopia).LoadCartridge(file, Machine::FAVORED_NES_NTSC, Machine::DONT_ASK_PROFILE);

	Input(Nestopia).ConnectController(0, Input::PAD1);
	Input(Nestopia).ConnectController(1, Input::PAD2);

	Machine(Nestopia).Power(true);
	
	return 1;
}

bool			NestTestMagic			(const char *name, MDFNFILE *fp)
{
	if(fp->data[0] == 'N' && fp->data[1] == 'E' && fp->data[2] == 'S' && fp->data[3] == 0x1A)
		return true;
		
	return false;
}

void			NestCloseGame			(void)
{
	Machine(Nestopia).Power(false);
	Machine(Nestopia).Unload();
	
	if(EmuVideo)	delete EmuVideo;
	if(EmuSound)	delete EmuSound;
	if(EmuPads)		delete EmuPads;
	
	EmuVideo = 0;
	EmuSound = 0;
	EmuPads = 0;
}

bool			NestToggleLayer			(int which)
{
	//TODO:
	return false;
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
	//TODO:
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

	if(espec->SoundFormatChanged)
	{
		Sound(Nestopia).SetSampleBits(16);
		Sound(Nestopia).SetSampleRate(espec->SoundRate);
		Sound(Nestopia).SetVolume(Sound::ALL_CHANNELS, 100);
		Sound(Nestopia).SetSpeaker(Sound::SPEAKER_MONO);
	}
	
	//TODO: Support color shift
	//TODO: Support multiplayer
	EmuPads->pad[0].buttons = 0;
	if(Ports[0]) EmuPads->pad[0].buttons = Ports[0][0];

	Nestopia.Execute(espec->skip ? 0 : EmuVideo, EmuSound, EmuPads);
	
	if(espec->SoundBuf && (espec->SoundBufMaxSize > espec->SoundRate / 60))
	{
		espec->SoundBufSize = espec->SoundRate / 60;
		memcpy(espec->SoundBuf, Samples, espec->SoundBufSize * 2);
	}
	
	uint32_t clipsides = MDFN_GetSettingB("nest.clipsides");
	uint32_t slstart = MDFN_GetSettingUI("nest.slstart");
	uint32_t slend = MDFN_GetSettingUI("nest.slend");
	
	espec->DisplayRect.x = clipsides ? 8 : 0;
	espec->DisplayRect.y = slstart;
	espec->DisplayRect.w = clipsides ? 232 : 256;
	espec->DisplayRect.h = slend - slstart;
}

void			NestSetInput			(int port, const char *type, void *ptr)
{
	if(port >= 0 && port <= 4)
	{
		Ports[port] = (uint8_t*)ptr;
	}
}

void			NestDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET)
	{
		Machine(Nestopia).Reset(0);
	}
	else if(cmd == MDFN_MSC_POWER)
	{
		Machine(Nestopia).Reset(1);
	}
}

static const InputDeviceInputInfoStruct GamepadIDII[] =
{
 { "a", "A", 7, IDIT_BUTTON_CAN_RAPID, NULL },
 { "b", "B", 6, IDIT_BUTTON_CAN_RAPID, NULL },
 { "select", "SELECT", 4, IDIT_BUTTON, NULL },
 { "start", "START", 5, IDIT_BUTTON, NULL },
 { "up", "UP", 0, IDIT_BUTTON, "down" },
 { "down", "DOWN", 1, IDIT_BUTTON, "up" },
 { "left", "LEFT", 2, IDIT_BUTTON, "right" },
 { "right", "RIGHT", 3, IDIT_BUTTON, "left" },
};

static InputDeviceInfoStruct InputDeviceInfoNESPort[] =
{
 // None
 {
  "none",
  "none",
  NULL,
  0,
  NULL
 },

 // Gamepad
 {
  "gamepad",
  "Gamepad",
  NULL,
  sizeof(GamepadIDII) / sizeof(InputDeviceInputInfoStruct),
  GamepadIDII,
 },
};


static const InputPortInfoStruct PortInfo[] =
{
 { 0, "port1", "Port 1", sizeof(InputDeviceInfoNESPort) / sizeof(InputDeviceInfoStruct), InputDeviceInfoNESPort, "gamepad" },
};

InputInfoStruct		NestInput = 
{
	sizeof(PortInfo) / sizeof(InputPortInfoStruct),
	PortInfo
};


FileExtensionSpecStruct	extensions[] = 
{
	{".nes", "iNES Format ROM Image"},
	{0, 0}
};


static MDFNSetting NestSettings[] =
{
  { "nest.clipsides", MDFNSF_NOFLAGS, "Clip left+right 8 pixel columns.", NULL, MDFNST_BOOL, "0" },
  { "nest.slstart", MDFNSF_NOFLAGS, "First displayed scanline in NTSC mode.", NULL, MDFNST_UINT, "8", "0", "239" },
  { "nest.slend", MDFNSF_NOFLAGS, "Last displayed scanlines in NTSC mode.", NULL, MDFNST_UINT, "231", "0", "239" },
  { NULL }
};


MDFNGI	NestInfo = 
{
	shortname:			"nest",
	fullname:			"Nestopia NES",
	FileExtensions:		extensions,
	ModulePriority:		MODPRIO_EXTERNAL_HIGH,
	Debugger:			0,
	InputInfo:			&NestInput,

	Load:				NestLoad,
	TestMagic:			NestTestMagic,
	LoadCD:				0,
	TestMagicCD:		0,
	CloseGame:			NestCloseGame,
	ToggleLayer:		NestToggleLayer,
	LayerNames:			"Background\0Sprites\0",
	InstallReadPatch:	NestInstallReadPatch,
	RemoveReadPatches:	NestRemoveReadPatches,
	MemRead:			NestMemRead,
	StateAction:		NestStateAction,
	Emulate:			NestEmulate,
	SetInput:			NestSetInput,
	DoSimpleCommand:	NestDoSimpleCommand,
	Settings:			NestSettings,
	MasterClock:		0,
	fps:				0,
	multires:			false,
	nominal_width:		256,
	nominal_height:		240,
	fb_width:			256,
	fb_height:			256,
	soundchan:			1
};
	
MDFNGI* GetNestopia()
{
	return &NestInfo;
}
