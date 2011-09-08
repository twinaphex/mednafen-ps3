#include <mednafen/mednafen.h>
#include "settings.h"

#include "core/api/NstApiMachine.hpp"
using namespace Nes::Api;

namespace nestMDFN
{
	//Locals
	NestSettingsList					NestopiaSettings;

	//Functions
	void								GetSettings						(const char* aName)
	{
		NestopiaSettings.NeedRefresh = true;

		NestopiaSettings.ClipSides = MDFN_GetSettingB("nest.clipsides");
		NestopiaSettings.ScanLineStart = MDFN_GetSettingUI("nest.slstart");
		NestopiaSettings.ScanLineEnd = MDFN_GetSettingUI("nest.slend");
		NestopiaSettings.FDSBios = MDFN_GetSettingS("nest.fdsbios");
		NestopiaSettings.EnableNTSC = MDFN_GetSettingB("nest.ntsc");
		NestopiaSettings.NTSCMode = MDFN_GetSettingUI("nest.ntscmode");
		NestopiaSettings.DisableSpriteLimit = MDFN_GetSettingB("nest.nospritelmt");
		NestopiaSettings.FavoredMachine = MDFN_GetSettingUI("nest.favored");
	}

	//Definitions
	const MDFNSetting_EnumList			NTSCTypes[] =
	{
		{"Composite",	0,	"Composite",	""},
		{"SVIDEO",		1,	"SVIDEO",		""},
		{"RGB",			2,	"RGB",			""},
		{0,				0,	0,				0}
	};

	const MDFNSetting_EnumList			MachineTypes[] =
	{
		{"NTSC",		Machine::FAVORED_NES_NTSC,	"NTSC",			""},
		{"PAL",			Machine::FAVORED_NES_PAL,	"PAL",			""},
		{"Famicom",		Machine::FAVORED_FAMICOM,	"Famicom",		""},
		{"Dendy",		Machine::FAVORED_DENDY,		"Dendy",		""},
		{0,				0,							0,				0}
	};

	MDFNSetting							NestSettings[] =
	{
		{"nest.clipsides",	MDFNSF_NOFLAGS,	"Clip left+right 8 pixel columns.",			NULL, MDFNST_BOOL,	"0",			0,		0,		0, nestMDFN::GetSettings},
		{"nest.slstart",	MDFNSF_NOFLAGS,	"First displayed scanline in NTSC mode.",	NULL, MDFNST_UINT,	"8",			"0",	"239",	0, nestMDFN::GetSettings},
		{"nest.slend",		MDFNSF_NOFLAGS,	"Last displayed scanlines in NTSC mode.",	NULL, MDFNST_UINT,	"231",			"0",	"239",	0, nestMDFN::GetSettings},
		{"nest.fdsbios",	MDFNSF_NOFLAGS,	"Path to FDS BIOS.",						NULL, MDFNST_STRING,"disksys.rom",	0,		0,		0, nestMDFN::GetSettings},
		{"nest.ntsc",		MDFNSF_NOFLAGS, "Enable the NTSC filter",					NULL, MDFNST_BOOL,	"0",			0,		0,		0, nestMDFN::GetSettings},
		{"nest.ntscmode",	MDFNSF_NOFLAGS, "Type of NTSC filter",						NULL, MDFNST_ENUM,	"Composite",	0,		0,		0, nestMDFN::GetSettings,	NTSCTypes},
		{"nest.nospritelmt",MDFNSF_NOFLAGS, "Disable NES Sprite limit",					NULL, MDFNST_BOOL,	"0",			0,		0,		0, nestMDFN::GetSettings},
		{"nest.favored",	MDFNSF_NOFLAGS,	"Choose favored machine type",				NULL, MDFNST_ENUM,	"NTSC",			0,		0,		0, nestMDFN::GetSettings,	MachineTypes},
		{NULL}
	};
}

