#include <es_system.h>
#include "src/thirdparty/simpleini/SimpleIni.h"

static uint32_t				GetInty								(const char* aValue)
{
	assert(aValue);
	return strtoll(aValue, 0, 16);
}

void						Colors::LoadColors					()
{
	CSimpleIniA ini;
	ini.LoadFile(es_paths->Build(std::string("assets/colors.ini")).c_str());

	BackGround = GetInty(ini.GetValue("uicolors", "BackGround", "D0D0D0C0"));
	SpecialBackGround = GetInty(ini.GetValue("uicolors", "SpecialBackGround", "40404040"));
	Border = GetInty(ini.GetValue("uicolors", "Border", "604040FF"));
	Normal = GetInty(ini.GetValue("uicolors", "Normal", "202020FF"));
	HighLight = GetInty(ini.GetValue("uicolors", "HighLight", "A02020FF"));
	SpecialNormal = GetInty(ini.GetValue("uicolors", "SpecialNormal", "206020FF"));
	SpecialHighLight = GetInty(ini.GetValue("uicolors", "SpecialHighLight", "20A020FF"));

	printf("%X\n", BackGround);

/*
	BackGround				= 0xD0D0D0C0;
	Border					= 0x604040FF;
	Normal 					= 0x202020FF;
	HighLight				= 0xA02020FF;
	SpecialNormal 			= 0x206020FF;
	SpecialHighLight		= 0x20A020FF;
	SpecialBackGround		= 0x40404040;*/
}

uint32_t					Colors::BackGround;
uint32_t					Colors::Border;
uint32_t					Colors::Normal;
uint32_t					Colors::HighLight;
uint32_t					Colors::SpecialNormal;
uint32_t					Colors::SpecialHighLight;
uint32_t					Colors::SpecialBackGround;

