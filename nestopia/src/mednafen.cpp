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
#include "core/api/NstApiSound.hpp"


#include "settings.h"
#include "mednafen.h"
#include "fileio.h"
#include "input.h"

namespace nestMDFN
{
	bool						UseNTSC;						//Is the NTSC filter enabled?
	const int					NTSCSettingsArray[][3] =	 	//Various contants for the NTSC engine
	{
		{Video::DEFAULT_SHARPNESS_COMP,			Video::DEFAULT_SHARPNESS_SVIDEO,		Video::DEFAULT_SHARPNESS_RGB},
		{Video::DEFAULT_COLOR_RESOLUTION_COMP,	Video::DEFAULT_COLOR_RESOLUTION_SVIDEO,	Video::DEFAULT_COLOR_RESOLUTION_RGB},
		{Video::DEFAULT_COLOR_BLEED_COMP,		Video::DEFAULT_COLOR_BLEED_SVIDEO,		Video::DEFAULT_COLOR_BLEED_RGB},
		{Video::DEFAULT_COLOR_ARTIFACTS_COMP,	Video::DEFAULT_COLOR_ARTIFACTS_SVIDEO,	Video::DEFAULT_COLOR_ARTIFACTS_RGB},
		{Video::DEFAULT_COLOR_FRINGING_COMP,	Video::DEFAULT_COLOR_FRINGING_SVIDEO,	Video::DEFAULT_COLOR_FRINGING_RGB}
	};

	bool						GameOpen = false;
	Nes::Api::Emulator			Nestopia;
	Sound::Output*				NESAudio;

	Video::Output 				NESVideo;

	uint32_t					NESSamples[48000];
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
/*	InstallReadPatch:	*/	0,
/*	RemoveReadPatches:	*/	0,
/*	MemRead:			*/	0,
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

//Implement MDFNGI:
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

		delete NESAudio;
		NESAudio = 0;

		GameOpen = false;
	}
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
	//PREP VIDEO
	if(espec->VideoFormatChanged || NestopiaSettings.NeedRefresh)
	{
		//Set ntsc mode settings, note that NTSC mode can only be used with a specific color format
		int ntscmode = NestopiaSettings.NTSCMode > 2 ? 0 : NestopiaSettings.NTSCMode;
		Video(Nestopia).SetSharpness(NTSCSettingsArray[0][ntscmode]);
		Video(Nestopia).SetColorResolution(NTSCSettingsArray[1][ntscmode]);
		Video(Nestopia).SetColorBleed(NTSCSettingsArray[2][ntscmode]);
		Video(Nestopia).SetColorArtifacts(NTSCSettingsArray[3][ntscmode]);
		Video(Nestopia).SetColorFringing(NTSCSettingsArray[4][ntscmode]);
		UseNTSC = NestopiaSettings.EnableNTSC && espec->surface->format.Rshift == 16 && espec->surface->format.Gshift == 8 && espec->surface->format.Bshift == 0;

		//Setup color
		//TODO: Support 16-bit, and YUV, limit output size based on mednafen frame buffer to avoid issues
	    Video::RenderState renderState;
    	renderState.bits.count = 32;
	    renderState.bits.mask.r = 0xFF << espec->surface->format.Rshift;
    	renderState.bits.mask.g = 0xFF << espec->surface->format.Gshift;
	    renderState.bits.mask.b = 0xFF << espec->surface->format.Bshift;
		renderState.filter = UseNTSC ? Video::RenderState::FILTER_NTSC : Video::RenderState::FILTER_NONE;
		renderState.width = UseNTSC ? Video::Output::NTSC_WIDTH : Video::Output::WIDTH;
		renderState.height = Video::Output::HEIGHT;

		//Send the render state to nestopia
    	if(NES_FAILED(Video(Nestopia).SetRenderState(renderState)))
		{
			MDFND_PrintError("nest: Failed to set render state");
		}
	}

	//Set video buffers
	NESVideo.pixels = espec->surface->pixels;
	NESVideo.pitch = espec->surface->pitchinpix * 4;
	Video(Nestopia).EnableUnlimSprites(NestopiaSettings.DisableSpriteLimit);

	//PREP AUDIO
	if(espec->SoundFormatChanged || NestopiaSettings.NeedRefresh)
	{
		//Delete any old audio interface
		delete NESAudio;
		NESAudio = 0;

		//If sound is enabled
		if(espec->SoundRate > 1.0)
		{
			//Create a new audio interface
			NESAudio = new Sound::Output();
			NESAudio->samples[0] = (void*)NESSamples;
			NESAudio->length[0] = (uint32_t)espec->SoundRate / NestopiaSettings.FPS;
			NESAudio->samples[1] = NULL;
			NESAudio->length[1] = 0;

			//Set nestopia sound properties, give a default sample rate if sound is unavailable
			Sound(Nestopia).SetSampleBits(16);
			Sound(Nestopia).SetSampleRate((uint32)espec->SoundRate);
			Sound(Nestopia).SetVolume(Sound::ALL_CHANNELS, 100);
			Sound(Nestopia).SetSpeaker(Sound::SPEAKER_MONO);
		}
	}

	NestopiaSettings.NeedRefresh = false;

	//INPUT
	UpdateControllers();

	//EXECUTE
	Nestopia.Execute(espec->skip ? 0 : &NESVideo, NESAudio, GetControllers());

	//FINISH VIDEO: Set the display area rectangle
	uint32_t widthhelp = UseNTSC ? 0 : 8;
	espec->DisplayRect.x = NestopiaSettings.ClipSides ? widthhelp : 0;
	espec->DisplayRect.y = NestopiaSettings.ScanLineStart;
	espec->DisplayRect.w = (UseNTSC ? Video::Output::NTSC_WIDTH : Video::Output::WIDTH) - (NestopiaSettings.ClipSides ? widthhelp : 0);
	espec->DisplayRect.h = NestopiaSettings.ScanLineEnd - NestopiaSettings.ScanLineStart;

	//FINISH AUDIO: Copy the samples into mednafen's sound buffer
	espec->SoundBufSize = 0;
	if(NESAudio && espec->SoundBuf && espec->SoundBufMaxSize >= ((uint32_t)espec->SoundRate) / NestopiaSettings.FPS)
	{
		espec->SoundBufSize = ((uint32_t)espec->SoundRate) / NestopiaSettings.FPS;
		memcpy(espec->SoundBuf, NESSamples, espec->SoundBufSize * 2);
	}

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



