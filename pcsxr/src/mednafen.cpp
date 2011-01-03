#include <src/mednafen.h>
#include <src/git.h>
#include <ps3_system.h>
#include <src/general.h>

namespace
{
	EmulateSpecStruct*			ESpec;
	uint8_t*					Ports[8];
}

//Definitions for PCSX
extern "C"
{
	void		SysInit					();
	void		SysClose				();
	void		SysFrame				(uint32_t* aPixels, uint32_t aPitch, uint32_t aKeys, uint32_t* aWidth, uint32_t* aHeight, uint32_t* aSound, uint32_t* aSoundLen);
	void		SetMCDS					(const char* aOne, const char* aTwo);



	uint32_t	DoesFileExist			(const char* aPath)
	{
		return Utility::FileExists(aPath) ? 1 : 0;
	}
}

int				PcsxrLoad				(const char *name, MDFNFILE *fp)
{
	std::string filename = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav");
	std::string filename2 = MDFN_MakeFName(MDFNMKF_SAV, 0, "sav2");
	SetMCDS(filename.c_str(), filename2.c_str());

	SysInit();
	return 1;
}

bool			PcsxrTestMagic			(const char *name, MDFNFILE *fp)
{
	return true;
}

void			PcsxrCloseGame			(void)
{
	SysClose();
}

bool			PcsxrToggleLayer		(int which)
{
	//TODO:
	return false;
}

void			PcsxrInstallReadPatch	(uint32 address)
{
	//TODO:
}

void			PcsxrRemoveReadPatches	(void)
{
	//TODO:
}

uint8			PcsxrMemRead			(uint32 addr)
{
	//TODO:
	return 0;
}

int				PcsxrStateAction		(StateMem *sm, int load, int data_only)
{
	return 0;
}

void			PcsxrEmulate			(EmulateSpecStruct *espec)
{
    ESpec = espec;

    if(espec->SoundFormatChanged)
    {
    }

	uint32_t width, height;
	uint32_t sndsize;
	SysFrame(espec->surface->pixels, espec->surface->pitch32, Ports[0][0] | (Ports[0][1] << 8), &width, &height, (uint32_t*)espec->SoundBuf, &sndsize);
	espec->SoundBufSize = sndsize;

    //TODO: Support color shift
    //TODO: Support multiplayer

    espec->DisplayRect.x = 0;
    espec->DisplayRect.y = 0;
    espec->DisplayRect.w = width;
    espec->DisplayRect.h = height;
}

void			PcsxrSetInput			(int port, const char *type, void *ptr)
{
	if(port >= 0 && port <= 4)
	{
		Ports[port] = (uint8_t*)ptr;
	}
}

void			PcsxrDoSimpleCommand	(int cmd)
{
}

static const InputDeviceInputInfoStruct GamepadIDII[] =
{
 { "select", "SELECT", 15, IDIT_BUTTON, NULL },
 { "l3", "L3", 13, IDIT_BUTTON, NULL },
 { "r3", "R3", 10, IDIT_BUTTON, NULL },
 { "start", "START", 14, IDIT_BUTTON, NULL },
 { "up", "UP", 0, IDIT_BUTTON, "down"},
 { "right", "RIGHT", 3, IDIT_BUTTON, "left" },
 { "down", "DOWN", 1, IDIT_BUTTON, "up" },
 { "left", "LEFT", 2, IDIT_BUTTON, "right" },
 { "l2", "L2", 12, IDIT_BUTTON, NULL },
 { "r2", "R2", 9, IDIT_BUTTON, NULL },
 { "l1", "L1", 11, IDIT_BUTTON, NULL },
 { "r1", "R1", 8, IDIT_BUTTON, NULL },
 { "triangle", "TRIANGLE", 4, IDIT_BUTTON, 0 },
 { "circle", "CIRCLE", 5, IDIT_BUTTON, 0 },
 { "cross", "CROSS", 6, IDIT_BUTTON, 0 },
 { "square", "SQUARE", 7, IDIT_BUTTON, 0 },
};

static InputDeviceInfoStruct InputDeviceInfoPSXPort[] =
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
 { 0, "port1", "Port 1", sizeof(InputDeviceInfoPSXPort) / sizeof(InputDeviceInfoStruct), InputDeviceInfoPSXPort, "gamepad" },
};

InputInfoStruct		PcsxrInput = 
{
	sizeof(PortInfo) / sizeof(InputPortInfoStruct),
	PortInfo
};


static FileExtensionSpecStruct	extensions[] = 
{
	{".tim", "iNES Format ROM Image"},
	{0, 0}
};


static MDFNSetting PcsxrSettings[] =
{
  { NULL }
};


MDFNGI	PcsxrInfo = 
{
	shortname:			"pcsxr",
	fullname:			"PCSX-Reloaded",
	FileExtensions:		extensions,
	ModulePriority:		MODPRIO_EXTERNAL_HIGH,
	Debugger:			0,
	InputInfo:			&PcsxrInput,

	Load:				PcsxrLoad,
	TestMagic:			PcsxrTestMagic,
	LoadCD:				0,
	TestMagicCD:		0,
	CloseGame:			PcsxrCloseGame,
	ToggleLayer:		PcsxrToggleLayer,
	LayerNames:			"Background\0",
	InstallReadPatch:	PcsxrInstallReadPatch,
	RemoveReadPatches:	PcsxrRemoveReadPatches,
	MemRead:			PcsxrMemRead,
	StateAction:		PcsxrStateAction,
	Emulate:			PcsxrEmulate,
	SetInput:			PcsxrSetInput,
	DoSimpleCommand:	PcsxrDoSimpleCommand,
	Settings:			PcsxrSettings,
	MasterClock:		0,
	fps:				0,
	multires:			false,
	nominal_width:		1024,
	nominal_height:		512,
	fb_width:			1024,
	fb_height:			512,
	soundchan:			2
};
	
MDFNGI* GetPCSX()
{
	return &PcsxrInfo;
}
