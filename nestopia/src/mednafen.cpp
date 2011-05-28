#include <src/mednafen.h>
#include <src/mempatcher.h>
#include <src/git.h>
#include <src/general.h>
#include <src/driver.h>

#include <fstream>
#include <sstream>
#include "core/api/NstApiEmulator.hpp"
#include "core/api/NstApiVideo.hpp"
#include "core/api/NstApiUser.hpp"
#include "core/api/NstApiFds.hpp"
#include "core/api/NstApiCartridge.hpp"

#include "settings.h"
#include "mednafen.h"
#include "fileio.h"
#include "input.h"
#include "sound.h"
#include "video.h"

namespace nestMDFN
{
	EmulateSpecStruct*			ESpec;
	bool						GameOpen = false;

	Nes::Api::Emulator			Nestopia;
}

using namespace nestMDFN;

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





//TODO: Masterclock and fps
MDFNGI	NestInfo =
{
/*	shortname:			*/	"nest",
/*	fullname:			*/	"Nintendo Entertainment System (Nestopia)",
/*	FileExtensions:		*/	NestExtensions,
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
/*	MasterClock:		*/	0,
/*	fps:				*/	0,
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

#ifdef MLDLL
#define VERSION_FUNC GetVersion
#define GETEMU_FUNC GetEmulator
#ifdef __WIN32__
#define DLL_PUBLIC __attribute__((dllexport))
#else
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#endif
#else
#define VERSION_FUNC nestGetVersion
#define GETEMU_FUNC nestGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}

extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC()
{
	return &NestInfo;
}


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

	//Setup machine type
	Machine::Mode type = GetSystemType(fp->data, fp->size, 0);
	Machine(Nestopia).SetMode(type);
	NestopiaSettings.FPS = (type == Machine::NTSC ? 60 : 50);
	NestInfo.MasterClock = MDFN_MASTERCLOCK_FIXED(NestopiaSettings.FPS * 100);
	NestInfo.fps = NestopiaSettings.FPS << 24;

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

	if(espec->VideoFormatChanged || NestopiaSettings.NeedRefresh)
	{
		SetupVideo(32, espec->surface->format.Rshift, espec->surface->format.Gshift, espec->surface->format.Bshift);
	}

	if(espec->SoundFormatChanged || NestopiaSettings.NeedRefresh)
	{
		SetupAudio(espec->SoundRate);
	}

	NestopiaSettings.NeedRefresh = false;

	SetBuffer(ESpec->surface->pixels, ESpec->surface->pitch32 * 4);
	UpdateControllers();

	Nestopia.Execute(espec->skip ? 0 : GetVideo(), GetAudio(), GetControllers());

	SetFrame(&espec->DisplayRect);
	CopyAudio(espec->SoundBuf, espec->SoundBufMaxSize, espec->SoundBufSize);

	//TODO: Real timing
	espec->MasterCycles = 1LL * 100;
}

void			NestSetInput			(int port, const char *type, void *ptr)
{
	PluginController(port, type, (uint8_t*)ptr);
}

void			NestDoSimpleCommand		(int cmd)
{
	if(cmd == MDFN_MSC_RESET || cmd == MDFN_MSC_POWER)
	{
		Machine(Nestopia).Reset((cmd == MDFN_MSC_RESET) ? 0 : 1);

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
		else
		{
			MDFND_DispMessage((UTF8*)"Cannot change disk sides");
		}
	}
}



