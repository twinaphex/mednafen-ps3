#include <mednafen/mednafen.h>
#include <mednafen/git.h>
#include <mednafen/md5.h>

#include <sstream>

#include "resample/resamplerinfo.h"
#include "resample/resampler.h"

#define MODULENAMESPACE gmbt
#include <module_helper.h>

#include "gambatte.h"

using namespace gambatte;

namespace gmbt
{
	GB*						GambatteEmu;

	::Resampler*			resampler;	

	uint32_t				Samples[48000];
	uint32_t				Resamples[48000];
	int32_t					SampleOverflow;

	class gbinput : public InputGetter
	{
		unsigned operator()()
		{
			return Input::GetPort<0, 1>();
		}
	};
	gbinput InputState;
}
using namespace	gmbt;

namespace MODULENAMESPACE
{
	extern MDFNGI							ModuleInfo;

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

	static InputDeviceInfoStruct			InputDeviceInfo[] =
	{
		{"gamepad", "Gamepad", NULL, 8, IDII}
	};

	static const InputPortInfoStruct		PortInfo[] =
	{
		{0, "builtin", "Built-In", 1, InputDeviceInfo, "gamepad"}
	};

	static InputInfoStruct					ModuleInput =
	{
		1, PortInfo
	};

	static FileExtensionSpecStruct			ModuleExtensions[] = 
	{
		{".gb",		"Game Boy Rom"},
		{".gbc",	"Game Boy Color Rom"},
		{".cgb",	"Game Boy Color Rom"},
		{0,			0}
	};


	static MDFNSetting						ModuleSettings[] =
	{
		{"gmbt.forcedmg",	MDFNSF_NOFLAGS,	"Force GB Mono Mode.",	NULL, MDFNST_BOOL,	"0"},
		{0}
	};

	static int								ModuleLoad				(const char *name, MDFNFILE *fp)
	{
		//Get Game MD5
		md5_context md5;
		md5.starts();
		md5.update(fp->data, fp->size);
		md5.finish(MDFNGameInfo->MD5);

		//Create gambatte objects
		GambatteEmu = new GB();

		//Init sound values
		SampleOverflow = 0;
		memset(Samples, 0, sizeof(Samples));
		memset(Resamples, 0, sizeof(Resamples));

		//Load the file into gambatte
		std::istringstream file(std::string((const char*)fp->data, (size_t)fp->size), std::ios_base::in | std::ios_base::binary);	
		if(GambatteEmu->load(file, MDFN_GetSettingB("gmbt.forcedmg")))
		{
			delete GambatteEmu;

			MDFND_PrintError("gambatte: Failed to load ROM");
			throw 0;
		}

		//Give gambatte it's objects
		GambatteEmu->setInputGetter(&InputState);

		//Done
		return 1;
	}

	static bool								ModuleTestMagic			(const char *name, MDFNFILE *fp)
	{
		static const uint8 GBMagic[8] = { 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B };
		return fp->size > 0x10C && !memcmp(fp->data + 0x104, GBMagic, 8);
	}

	static void								ModuleCloseGame			()
	{
//		delete Resample;
		delete GambatteEmu;
		GambatteEmu = 0;
	}

	static int								ModuleStateAction		(StateMem *sm, int load, int data_only)
	{
		if(!load)
		{
			//Get the state data from gambatte
			std::ostringstream os(std::ios_base::out | std::ios_base::binary);
			GambatteEmu->saveState(0, 0, os);

			//Feed it to medanfen and leave
			smem_write32le(sm, os.str().size());
			smem_write(sm, (char*)os.str().data(), os.str().size());
			return 1;
		}
		else
		{
			//Get the size of the state
			uint32_t size;
			smem_read32le(sm, &size);

			//Read it from mednafen
			char* buffer = (char*)malloc(size);
			smem_read(sm, buffer, size);

			//Feed it to gambatte
			std::istringstream iss(std::string((const char*)buffer, (size_t)size), std::ios_base::in | std::ios_base::binary);
			GambatteEmu->loadState(iss);

			//Clear up and leave
			free(buffer);
			SampleOverflow = 0;
			return 1;
		}
		return 0;
	}

	static void								ModuleEmulate		(EmulateSpecStruct *espec)
	{
		//AUDIO PREP
		if(espec->SoundFormatChanged)
		{
			//Fir_Resampler can't handle the rate I guess
			resampler = ResamplerInfo::get(0).create(2097152, espec->SoundRate, 48000);
		}

		//EMULATE
		uint32_t samps = 35112 + SampleOverflow;
		GambatteEmu->runFor((gambatte::uint_least32_t*)espec->surface->pixels, espec->surface->pitchinpix, (gambatte::uint_least32_t*)Samples, samps);
		SampleOverflow += 35112 - samps;

		//VIDEO
		gmbt::Video::SetDisplayRect(espec, 0, 0, 160, 144);

		//AUDIO
		int32_t count = resampler->resample((short*)Resamples, (short*)Samples, samps);
		espec->SoundBufSize = std::min(espec->SoundBufMaxSize, count);
		memcpy(espec->SoundBuf, Resamples, espec->SoundBufSize * 4);

		//TODO: Real timing
		espec->MasterCycles = 1LL * 100;
	}

	static void								ModuleSetInput			(int port, const char *type, void *ptr)
	{
		Input::SetPort(port, (uint8_t*)ptr);
	}

	static void								ModuleDoSimpleCommand	(int cmd)
	{
		if(cmd == MDFN_MSC_RESET || cmd == MDFN_MSC_POWER)
		{
			GambatteEmu->reset();
		}
	}

	MDFNGI									ModuleInfo = 
	{
	/*	shortname:*/		"gmbt",
	/*	fullname:*/			"Game Boy Color (Gambatte)",
	/*	FileExtensions:*/	ModuleExtensions,
	/*	ModulePriority:*/	MODPRIO_EXTERNAL_HIGH,
	/*	Debugger:*/			0,
	/*	InputInfo:*/		&ModuleInput,

	/*	Load:*/				ModuleLoad,
	/*	TestMagic:*/		ModuleTestMagic,
	/*	LoadCD:*/			0,
	/*	TestMagicCD:*/		0,
	/*	CloseGame:*/		ModuleCloseGame,
	/*	ToggleLayer:*/		0,
	/*	LayerNames:*/		0,
	/*	InstallReadPatch:*/	0,
	/*	RemoveReadPatches:*/0,
	/*	MemRead:*/			0,
	/*	StateAction:*/		ModuleStateAction,
	/*	Emulate:*/			ModuleEmulate,
	/*	SetInput:*/			ModuleSetInput,
	/*	DoSimpleCommand:*/	ModuleDoSimpleCommand,
	/*	Settings:*/			ModuleSettings,
	/*	MasterClock:*/		MDFN_MASTERCLOCK_FIXED(6000),
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
}

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
	return 0x918;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &MODULENAMESPACE::ModuleInfo : 0;
}

