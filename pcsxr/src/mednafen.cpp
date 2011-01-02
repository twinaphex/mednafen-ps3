#include <src/mednafen.h>
#include <src/git.h>
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
	void		SysFrame				(uint32_t* aPixels, uint32_t aPitch);
}

int				PcsxrLoad				(const char *name, MDFNFILE *fp)
{
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

	SysFrame(espec->surface->pixels, espec->surface->pitch32);

    //TODO: Support color shift
    //TODO: Support multiplayer

    if(espec->SoundBuf && (espec->SoundBufMaxSize > espec->SoundRate / 60))
    {
        espec->SoundBufSize = 48000 / 60;
        memset(espec->SoundBuf, 0, espec->SoundBufSize * 2);
    }

    espec->DisplayRect.x = 0;
    espec->DisplayRect.y = 0;
    espec->DisplayRect.w = 1024;
    espec->DisplayRect.h = 512;
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
 { "a", "A", 7, IDIT_BUTTON_CAN_RAPID, NULL },
 { "b", "B", 6, IDIT_BUTTON_CAN_RAPID, NULL },
 { "select", "SELECT", 4, IDIT_BUTTON, NULL },
 { "start", "START", 5, IDIT_BUTTON, NULL },
 { "up", "UP", 0, IDIT_BUTTON, "down" },
 { "down", "DOWN", 1, IDIT_BUTTON, "up" },
 { "left", "LEFT", 2, IDIT_BUTTON, "right" },
 { "right", "RIGHT", 3, IDIT_BUTTON, "left" },
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
	soundchan:			1
};
	
MDFNGI* GetPCSX()
{
	return &PcsxrInfo;
}
