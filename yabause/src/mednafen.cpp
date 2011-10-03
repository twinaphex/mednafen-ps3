#include <mednafen/mednafen.h>
#include <mednafen/git.h>
#include <mednafen/driver.h>
#include <mednafen/general.h>
#include <mednafen/mempatcher.h>
#include <mednafen/md5.h>
#include <mednafen/cdrom/cdromif.h>

#define MODULENAMESPACE Yabause
#include <module_helper.h>
using namespace Yabause;

#include <stdio.h>

#ifdef PSGL //PSGL.h cannot be included in an 'extern "C" block'
#include <PSGL/psgl.h>
#define glOrtho glOrthof
#else
#include <GL/gl.h>
#endif


//SYSTEM
extern "C"
{
	#include "yabause.h"
	#include "peripheral.h"
	#include "sh2core.h"
	#include "sh2int.h"
	#include "vidogl.h"
	#include "vidsoft.h"
	#include "cs0.h"
	#include "cs2.h"
	#include "cdbase.h"
	#include "scsp.h"
	#include "m68kcore.h"
	#include "sndmdfn.h"
	#include "permdfnjoy.h"

	#define StateMemTag void

	void ywrite(IOCheck_struct * check, void * ptr, size_t size, size_t nmemb, StateMemTag* stream) {
	   IOCheck_struct checker;
       IOCheck_struct* checkP = check ? check : &checker;

	   (unsigned int)smem_write((StateMem*)stream, ptr, size * nmemb);
	   checkP->done += (unsigned int)nmemb;
	   checkP->size += (unsigned int)nmemb;
	}

	void yread(IOCheck_struct * check, void * ptr, size_t size, size_t nmemb, StateMemTag* stream) {
	   IOCheck_struct checker;
       IOCheck_struct* checkP = check ? check : &checker;

	   (unsigned int)smem_read((StateMem*)stream, ptr, size * nmemb);
	   checkP->done += (unsigned int)nmemb;
	   checkP->size += (unsigned int)nmemb;
	}

	size_t yseek(StateMemTag* fp, size_t offset, int whence) {
		return smem_seek((StateMem*)fp, offset, whence);
	}

	size_t ytell(StateMemTag* fp) {
		return smem_tell((StateMem*)fp);
	}

	int StateWriteHeader(StateMemTag* fp, const char *name, int version) {
	   IOCheck_struct check;
       smem_write((StateMem*)fp, (void*)name, strlen(name));
	   check.done = 0;
	   check.size = 0;
	   ywrite(&check, (void *)&version, sizeof(version), 1, fp);
	   ywrite(&check, (void *)&version, sizeof(version), 1, fp); // place holder for size
	   return (check.done == check.size) ? smem_tell((StateMem*)fp) : -1;
	}

	int StateFinishHeader(StateMemTag* fp, int offset) {
	   IOCheck_struct check;
	   int size = 0;
	   size = smem_tell((StateMem*)fp) - offset;
	   smem_seek((StateMem*)fp, offset - 4, SEEK_SET);
	   check.done = 0;
	   check.size = 0;
	   ywrite(&check, (void *)&size, sizeof(size), 1, fp); // write true size
	   smem_seek((StateMem*)fp, 0, SEEK_END);
	   return (check.done == check.size) ? (size + 12) : -1;
	}

	int StateCheckRetrieveHeader(StateMemTag* fp, const char *name, int *version, int *size) {
	   char id[4];
	   size_t ret;

	   if ((ret = smem_read((StateMem*)fp, (void *)id, 4)) != 4)
		  return -1;

	   if (strncmp(name, id, 4) != 0)
		  return -2;

	   if ((ret = smem_read((StateMem*)fp, (void *)version, 4)) != 4)
		  return -1;

	   if (smem_read((StateMem*)fp, (void *)size, 4) != 4)
		  return -1;

	   return 0;
	}
	
	int									YabaSkipFrame;				//Patched to core to skip frame is this is true
	char								BatteryFile[1024];			//2K of BSS down the drain
	char								CartFile[1024];

	void								CheatDoPatches				() {}	//HACK: I really wanted to dummy this out, but a call is made from sh2_dynarac in an s file, so CPP can't do it
	void								YuiSwapBuffers				() {}
	void								YuiErrorMsg					(const char * string) {MDFN_printf("yabause: %s\n", string);}

	void								YuiSetVideoAttribute		(int type, int val) {}
	int									YuiSetVideoMode				(int width, int height, int bpp, int fullscreen) {return 0;}

	static int32_t						ResolutionWidth = 320;
	static int32_t						ResolutionHeight = 244;
	void								YuiChangeResolution			(int width, int height)
	{
		ResolutionWidth = width;
		ResolutionHeight = height;
	}


	M68K_struct * M68KCoreList[] =
	{
		&M68KQ68,
		0
	};

	SH2Interface_struct *SH2CoreList[] =
	{
		&SH2Interpreter,
#ifdef SH2_DYNAREC
#define SH2CORE_DYNAREC 2
		&SH2Dynarec,
#endif
		0,
	};

	PerInterface_struct *PERCoreList[] =
	{
		&PERMDFNJoy,
		0
	};

	CDInterface *CDCoreList[] =
	{
		&ArchCD,
		0
	};

	SoundInterface_struct *SNDCoreList[] =
	{
		&SNDMDFN,
		NULL
	};

	VideoInterface_struct *VIDCoreList[] =
	{
		&VIDSoft,
		&VIDOGL,
		NULL
	};
}

namespace MODULENAMESPACE
{
	extern MDFNGI							ModuleInfo;

	static const InputDeviceInputInfoStruct	GamepadIDII[] =
	{
		{ "up",		"UP ↑",				0,	IDIT_BUTTON,			"down"	},
		{ "right",	"RIGHT →",			3,	IDIT_BUTTON,			"left"	},
		{ "down",	"DOWN ↓",			1,	IDIT_BUTTON,			"up"	},
		{ "left",	"LEFT ←",			2,	IDIT_BUTTON,			"right"	},
		{ "rt",		"RIGHT TRIGGER",	10,	IDIT_BUTTON,			NULL	},
		{ "lt",		"LEFT TRIGGER",		11,	IDIT_BUTTON,			NULL	},
		{ "start",	"START",			12,	IDIT_BUTTON,			NULL	},
		{ "a",		"A",				4,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "b",		"B",				5,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "c",		"C",				6,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "x",		"X",				7,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "y",		"Y",				8,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "z",		"Z",				9,	IDIT_BUTTON_CAN_RAPID,	NULL	},
	};

	static InputDeviceInfoStruct 			InputDeviceInfo[] =
	{
		{"none",	"none",		NULL,	0,		NULL			},
		{"gamepad", "Gamepad",	NULL,	13,		GamepadIDII,	},
	};

	static const InputPortInfoStruct		PortInfo[] =
	{
		{0,	"port1",	"Port 1",	2,	InputDeviceInfo, "gamepad" },
		{0,	"port2",	"Port 2",	2,	InputDeviceInfo, "gamepad" },
	};

	static InputInfoStruct 					ModuleInput =
	{
		2,	PortInfo
	};

	static FileExtensionSpecStruct			ModuleExtensions[] =
	{
		{".cue", "Sega Saturn Disc Image"},
		{NULL, NULL}
	};

	const static MDFNSetting_EnumList		ModuleCartTypeList[] =
	{
		{"save4mbit",		CART_BACKUPRAM4MBIT,	"4 MBIT SAVE"		""},
		{"save8mbit",		CART_BACKUPRAM8MBIT,	"8 MBIT SAVE"		""},
		{"save16mbit",		CART_BACKUPRAM16MBIT,	"16 MBIT SAVE"		""},
		{"save32mbit",		CART_BACKUPRAM32MBIT,	"32 MBIT SAVE"		""},
		{"ram8mbit",		CART_DRAM8MBIT,			"8 MBIT RAM",		""},
		{"ram32mbit",		CART_DRAM32MBIT,		"32 MBIT RAM",		""},
		{"none",			CART_NONE,				"No Cart.",			""},
		{0,					0,	0,										0}
	};

	static MDFNSetting						ModuleSettings[] =
	{
		{"yabause.bios",		MDFNSF_EMU_STATE,	"Path to Sega Satrun BIOS Image.",							NULL,	MDFNST_STRING,	"satbios.bin"},
		{"yabause.use_opengl",	MDFNSF_EMU_STATE,	"Use the OpenGL renderer.",									NULL,	MDFNST_BOOL,	"0"},
		{"yabause.cart_type",	MDFNSF_EMU_STATE,	"Type of addon cart to emulate.",							NULL,	MDFNST_ENUM,	"save4mbit", NULL, NULL, NULL, NULL, ModuleCartTypeList},
#ifdef SH2_DYNAREC
		{"yabause.sh2dynarec",	MDFNSF_EMU_STATE,	"Use the SH2 dynamic recompiler.",							NULL,	MDFNST_BOOL,	"0"},
#endif
		{NULL}
	};


	static int								ModuleLoad				()
	{
		//Get Settings
		static char biospath[1024];
		strncpy(biospath, MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("yabause.bios").c_str()).c_str(), 1024);
		ModuleInfo.OpenGL = MDFN_GetSettingB("yabause.use_opengl");

		//Get Save name
		std::string filePath = MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, "saturn_system_save.bin");
		strncpy(BatteryFile, filePath.c_str(), sizeof(BatteryFile));

		filePath = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
		strncpy(CartFile, filePath.c_str(), sizeof(CartFile));

		//Init yabause
		yabauseinit_struct yinit;
		memset(&yinit, 0, sizeof(yabauseinit_struct));
		yinit.percoretype = PERCORE_MDFNJOY;
#ifdef SH2_DYNAREC
		yinit.sh2coretype = MDFN_GetSettingB("yabause.sh2dynarec") ? SH2CORE_DYNAREC : SH2CORE_INTERPRETER;
#else
		yinit.sh2coretype = SH2CORE_INTERPRETER;
#endif
		yinit.vidcoretype = ModuleInfo.OpenGL ? VIDCORE_OGL : VIDCORE_SOFT;
		yinit.sndcoretype = SNDCORE_MDFN;
		yinit.cdcoretype = CDCORE_ARCH;
		yinit.m68kcoretype = M68KCORE_Q68;
		yinit.carttype = MDFN_GetSettingUI("yabause.cart_type");
		yinit.regionid = REGION_AUTODETECT;
		yinit.biospath = biospath;
		yinit.cdpath = "";
		yinit.buppath = BatteryFile;
		yinit.mpegpath = 0;
		yinit.cartpath = CartFile;
		yinit.netlinksetting = 0;
	//	yinit.flags = VIDEOFORMATTYPE_NTSC;

		if(YabauseInit(&yinit) < 0)
		{
			MDFN_printf("yabause: Failed to init");
			return 0;
		}

		ScspSetVolume(100);
		ScspUnMuteAudio();

		return 1;
	}

	static bool								ModuleTestMagic			()
	{
		uint8_t Buffer[4000];
		CDIF_ReadRawSector(Buffer, 0);

		//Works for shining force 3 anyway (only game I have on hand to test)
		return strncmp("SEGA SEGASATURN", (const char*)&Buffer[16], 15) == 0;
	}

	static void								ModuleCloseGame			()
	{
		//TODO: Anything else?
		YabauseDeInit();

		Resampler::Kill();
	}

	static int								ModuleStateAction		(StateMem *sm, int load, int data_only)
	{
		if(load)
		{
			return !YabLoadState(sm);
		}
		else
		{
			return !YabSaveState(sm);
		}

		return 0;
	}

	static void								ModuleEmulate			(EmulateSpecStruct *espec)
	{
		//AUDIO PREP
		Resampler::Init(espec, 44100.0);

		//VIDEO PREP
		YabaSkipFrame = espec->skip ? 1 : 0;

		if(ModuleInfo.OpenGL)
		{
			glViewport(0, 0, 704, 512);
			glDisableClientState(GL_COLOR_ARRAY);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, ResolutionWidth, ResolutionHeight, 0, 1, 0);

			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(-1024, 1024, -1024, 1024, 1, 0);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		//EMULATE
		PERCore->HandleEvents();

		//VIDEO
		if(ModuleInfo.OpenGL)
		{
			glMatrixMode(GL_TEXTURE);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			int width, height;
			Video::SetDisplayRect(espec, 0, 0, ModuleInfo.fb_width, ModuleInfo.fb_height);
		}
		else
		{
			int width, height;
			VIDSoftGetScreenSize(&width, &height);
			Video::SetDisplayRect(espec, 0, 0, width, height);
			Video::BlitRGB32<0, 1, 2, 2, 1, 0, 0>(espec, dispbuffer, width, height, width);
		}

		//AUDIO
		Resampler::Fetch(espec);
	}

	static void								ModuleSetInput			(int port, const char *type, void *ptr)
	{
		Input::SetPort(port, (uint8_t*)ptr);
	}

	static void								ModuleDoSimpleCommand	(int cmd)
	{
		if(cmd == MDFN_MSC_RESET)
		{
			YabauseResetButton();
		}
		else if(cmd == MDFN_MSC_POWER)
		{
			YabauseReset();
		}
	}

	MDFNGI									ModuleInfo =
	{
	/*	shortname:			*/	"yabause",
	/*	fullname:			*/	"Sega Saturn (yabause)",
	/*	FileExtensions:		*/	ModuleExtensions,
	/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
	/*	Debugger:			*/	0,
	/*	InputInfo:			*/	&ModuleInput,

	/*	Load:				*/	0,
	/*	TestMagic:			*/	0,
	/*	LoadCD:				*/	ModuleLoad,
	/*	TestMagicCD:		*/	ModuleTestMagic,
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
	/*	multires:			*/	false,
	/*	lcm_width:			*/	704,
	/*	lcm_height:			*/	512,
	/*  dummy_separator:	*/	0,
	/*	nominal_width:		*/	704,
	/*	nominal_height:		*/	512,
	/*	fb_width:			*/	704,
	/*	fb_height:			*/	512,
	/*	soundchan:			*/	2,
	/*	OpenGL:				*/	true
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
#define VERSION_FUNC yabauseGetVersion
#define GETEMU_FUNC yabauseGetEmulator
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

