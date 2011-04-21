#ifndef NEST_INPUT_H_MDFN
#define NEST_INPUT_H_MDFN

#include "core/api/NstApiInput.hpp"
using namespace Nes;
using namespace Nes::Api;


namespace nestMDFN
{
	void								PluginController					(int32_t aPort, const char* aType, uint8_t* aData);
	void								UpdateControllers					();
	Input::Controllers*					GetControllers						();

	extern InputInfoStruct				NestInput;
}

#endif


