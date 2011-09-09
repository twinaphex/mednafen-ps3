#include <es_system.h>
#include <map>
#include "src/thirdparty/simpleini/SimpleIni.h"

namespace
{
	bool								Inited;
	CSimpleIniA							INI;
}

static uint32_t							GetInty								(const char* aValue)
{
	assert(aValue);
	return strtoll(aValue, 0, 16);
}

										Color::Color						() :
	Value(0)
{

}

										Color::Color						(uint32_t aColor) :
	Value(aColor)
{

}

										Color::Color						(const char* aName, uint32_t aDefault) :
	Value(Colors::GetColor(aName, aDefault))
{

}



void									Colors::Initialize					()
{
	if(!Inited)
	{
		Inited = true;
		INI.LoadFile(es_paths->Build(std::string("assets/colors.ini")).c_str());
	}
}

uint32_t								Colors::GetColor					(const char* aName, uint32_t aDefaultValue)
{
	Initialize();

	char defaultValue[10];
	snprintf(defaultValue, 9, "%0X8", aDefaultValue);

	uint32_t color = GetInty(INI.GetValue("uicolors", aName, defaultValue));

	return color ? color : 0x01000000;
}

