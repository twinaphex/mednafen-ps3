//Includes from mednafen
#include <src/mednafen.h>
#include <src/cdrom/cdromif.h>
#include <src/mednafen-driver.h>
#include <src/git.h>
#include <src/general.h>
#include <src/md5.h>
#include <stdarg.h>

//Desmume includes
#define SFORMAT dsSFORMAT

#include "MMU.h"
#include "NDSSystem.h"
#include "debug.h"
#include "sndsdl.h"
#include "render3D.h"
#include "rasterize.h"
#include "saves.h"
#include "firmware.h"
#include "GPU_osd.h"
#include "addons.h"

#define MODULENAMESPACE desmume
#include <module_helper.h>
using namespace desmume;

//Desume interface
volatile bool								execute = true;

#define SNDCORE_MDFN	10010
extern SoundInterface_struct				SNDMDFN;

SoundInterface_struct*						SNDCoreList[] =
{
	&SNDDummy,
	&SNDMDFN,
	NULL
};

GPU3DInterface*								core3DList[] =
{
	&gpu3DRasterize,
	NULL
};

static void									ReadSettings			(const char* aName)
{
	CommonSettings.num_cores = MDFN_GetSettingUI("desmume.num_cores");
	CommonSettings.GFX3D_HighResolutionInterpolateColor = MDFN_GetSettingB("desmume.highresintp");
	CommonSettings.GFX3D_EdgeMark = MDFN_GetSettingB("desmume.edgemark");
	CommonSettings.GFX3D_Fog = MDFN_GetSettingB("desmume.fog");
	CommonSettings.GFX3D_Texture = MDFN_GetSettingB("desmume.texture");
	CommonSettings.GFX3D_LineHack = MDFN_GetSettingB("desmume.linehack");
	CommonSettings.GFX3D_Zelda_Shadow_Depth_Hack = MDFN_GetSettingB("desmume.zeldashadowhack");
	CommonSettings.rigorous_timing = MDFN_GetSettingB("desmume.rigorous_timing");
	CommonSettings.advanced_timing = MDFN_GetSettingB("desmume.advanced_timing");
}

namespace MODULENAMESPACE
{
	extern MDFNGI							ModuleInfo;

	static const InputDeviceInputInfoStruct	GamepadIDII[] =
	{
		{ "r",		"Right Shoulder",		11,	IDIT_BUTTON,			NULL	},
		{ "l",		"Left Shoulder",		10,	IDIT_BUTTON,			NULL	},
		{ "x",		"X (center, upper)",	8,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "y",		"Y (left)",				6,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "a",		"A (right)",			9,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "b",		"B (center, lower)",	7,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "start",	"START",				5,	IDIT_BUTTON,			NULL	},
		{ "select",	"SELECT",				4,	IDIT_BUTTON,			NULL	},
		{ "up",		"UP ↑",					0,	IDIT_BUTTON,			"down"	},
		{ "down",	"DOWN ↓",				1,	IDIT_BUTTON,			"up"	},
		{ "left",	"LEFT ←",				2,	IDIT_BUTTON,			"right"	},
		{ "right",	"RIGHT →",				3,	IDIT_BUTTON,			"left"	},
		{ "lid",	"Lid",					12,	IDIT_BUTTON,			NULL	}
	};

	static InputDeviceInfoStruct 			InputDeviceInfoPort[] =
	{
		{"gamepad", "Gamepad",	NULL,	13,	GamepadIDII},
	};


	static const InputPortInfoStruct 		PortInfo[] =
	{
		{0, "port1", "Port 1", 1, InputDeviceInfoPort, "gamepad"},
	};

	static InputInfoStruct					ModuleInput =
	{
		1,
		PortInfo
	};


	static FileExtensionSpecStruct			ModuleExtensions[] = 
	{
		{".nds", "NDS ROM"},
		{0, 0}
	};


	static MDFNSetting						ModuleSettings[] =
	{
		{"desmume.num_cores",		MDFNSF_NOFLAGS,	"Number of CPU cores to use. (1, 2 or 4 (3=2))",	NULL,	MDFNST_UINT,	"1",	"1",	"4",	0,	ReadSettings},
		{"desmume.highresintp",		MDFNSF_NOFLAGS,	"Use High resolution color interpolation",			NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.edgemark",		MDFNSF_NOFLAGS,	"Edge Mark?",										NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.fog",				MDFNSF_NOFLAGS,	"Enable Fog",										NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.texture",			MDFNSF_NOFLAGS,	"Enable Texturing",									NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.linehack",		MDFNSF_NOFLAGS,	"Enable Line Hack?",								NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.zeldashadowhack",	MDFNSF_NOFLAGS,	"Enable Zelda Shadow Depth Hack",					NULL,	MDFNST_BOOL,	"0",	"0",	"1",	0,	ReadSettings},
		{"desmume.rigorous_timing",	MDFNSF_NOFLAGS,	"Enable Rigorous Timing",							NULL,	MDFNST_BOOL,	"0",	"0",	"1",	0,	ReadSettings},
		{"desmume.advanced_timing",	MDFNSF_NOFLAGS,	"Enable Advanced Timing",							NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{NULL}
	};

	static int								ModuleLoad				(const char *name, MDFNFILE *fp)
	{
		//Get Game MD5
		md5_context md5;
		md5.starts();
		md5.update(fp->data, fp->size);
		md5.finish(MDFNGameInfo->MD5);

		//Fetch settings
		ReadSettings(0);

		//Setup desmume
		struct NDS_fw_config_data fw_config;
		NDS_FillDefaultFirmwareConfigData(&fw_config);

		addonsChangePak(NDS_ADDON_NONE);
		NDS_Init();
		NDS_CreateDummyFirmware(&fw_config);
		NDS_3D_ChangeCore(0);
		SPU_ChangeSoundCore(SNDCORE_MDFN, 735 * 2);		
		backup_setManualBackupType(MC_TYPE_AUTODETECT);

		//Load ROM
		NDS_LoadROM(fp->data, fp->size);

		return 1;
	}

	static bool								ModuleTestMagic			(const char *name, MDFNFILE *fp)
	{
		return !strcasecmp(fp->ext, "nds");
	}

	static void								ModuleCloseGame			()
	{
		NDS_DeInit();
	}


	static int								ModuleStateAction		(StateMem *sm, int load, int data_only)
	{
		if(!load)
		{
			EMUFILE_MEMORY data;
			if(savestate_save(&data, 0))
			{
				smem_write32le(sm, data.size());
				smem_write(sm, data.buf(), data.size());
				return 1;
			}

			return 0;
		}
		else
		{
			uint32_t len;
			smem_read32le(sm, &len);

			if(len)
			{
				EMUFILE_MEMORY data(len);
				smem_read(sm, data.buf(), len);

				return savestate_load(&data) ? 1 : 0;
			}

			return 0;
		}
	}

	static void								ModuleEmulate			(EmulateSpecStruct *espec)
	{
		//AUDIO PREP
		Resampler::Init(espec, 44100.0);

		//INPUT
		uint32_t portData = Input::GetPort<0, 2>() << 1; //input.array[0] will never be set

		NDS_beginProcessingInput();
		UserButtons& input = NDS_getProcessingUserInput().buttons;

		for(int i = 0; i != 14; i ++, portData >>= 1)
		{
			input.array[i] = portData & 1;
		}
	
		NDS_endProcessingInput();

		//CHEATS
		//EMULATE
		NDS_exec<false>();
		SPU_Emulate_user();

		//VIDEO: TODO: Support other color formats
		Video::BlitRGB15<0, 1, 2, 2, 1, 0>(espec, (const uint16_t*)GPU_screen, 256, 192 * 2, 256);
		Video::SetDisplayRect(espec, 0, 0, 256, 192 * 2);

		//AUDIO
		Resampler::Fetch(espec);

		//TIMING
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
			NDS_Reset();
		}
	}

	MDFNGI									ModuleInfo = 
	{
	/*	shortname:			*/	"desmume",
	/*	fullname:			*/	"Nintendo DS (desmume)",
	/*	FileExtensions:		*/	ModuleExtensions,
	/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
	/*	Debugger:			*/	0,
	/*	InputInfo:			*/	&ModuleInput,

	/*	Load:				*/	ModuleLoad,
	/*	TestMagic:			*/	ModuleTestMagic,
	/*	LoadCD:				*/	0,
	/*	TestMagicCD:		*/	0,
	/*	CloseGame:			*/	ModuleCloseGame,
	/*	ToggleLayer:		*/	0,
	/*	LayerNames:			*/	0,
	/*	InstallReadPatch:	*/	0,
	/*	RemoveReadPatches:	*/	0,
	/*	MemRead:			*/	0,
	/*	StateAction:		*/	ModuleStateAction,
	/*	Emulate:			*/	ModuleEmulate,
	/*	SetInput:			*/	ModuleSetInput,
	/*	DoSimpleCommand:	*/	ModuleDoSimpleCommand,
	/*	Settings:			*/	ModuleSettings,
	/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
	/*	fps:				*/	0,
	/*	multires:			*/	true,
	/*	lcm_width:			*/	256,
	/*	lcm_height:			*/	192*2,
	/*	dummy_separator:	*/	0,
	/*	nominal_width:		*/	256,
	/*	nominal_height:		*/	192*2,
	/*	fb_width:			*/	256,
	/*	fb_height:			*/	192*2,
	/*	soundchan:			*/	2
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
#define VERSION_FUNC desmumeGetVersion
#define GETEMU_FUNC desmumeGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x916;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &MODULENAMESPACE::ModuleInfo : 0;
}


