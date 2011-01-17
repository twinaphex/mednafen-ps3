#include <src/mednafen.h>

#include "core/api/NstApiEmulator.hpp"
#include "core/api/NstApiFds.hpp"
#include "core/api/NstApiCartridge.hpp"

using namespace Nes;
using namespace Nes::Api;

#include "mednafen.h"
#include "input.h"

namespace nestMDFN
{
	//Locals
	Input::Controllers			EmuPads;
	uint8_t*					Ports[Input::NUM_PADS];

	//Functions
	void						PluginController					(int32_t aPort, const char* aType, uint8_t* aData)
	{
		if(aPort >= 0 && aPort < Input::NUM_PADS)
		{
			Ports[aPort] = aData;

			Input(Nestopia).ConnectController(aPort, aPort == 0 ? Input::PAD1 : Input::PAD2);
		}
		else
		{
			MDFND_Message("Couldn't plugin in controller");
		}
	}

	void						UpdateControllers					()
	{
		for(int i = 0; i != Input::NUM_PADS; i ++)
		{
			EmuPads.pad[i].buttons = Ports[i] ? Ports[i][0] : 0;
		}
	}

	Input::Controllers*					GetControllers				()
	{
		return &EmuPads;
	}

	//Definitions
	InputDeviceInputInfoStruct	GamepadIDII[] =
	{
		{"a",		"A",		7, IDIT_BUTTON_CAN_RAPID,	NULL},
		{"b",		"B",		6, IDIT_BUTTON_CAN_RAPID,	NULL},
		{"select",	"SELECT",	4, IDIT_BUTTON,				NULL},
		{"start",	"START",	5, IDIT_BUTTON,				NULL},
		{"up",		"UP",		0, IDIT_BUTTON,				"down"},
		{"down",	"DOWN",		1, IDIT_BUTTON,				"up"},
		{"left",	"LEFT",		2, IDIT_BUTTON,				"right"},
		{"right",	"RIGHT",	3, IDIT_BUTTON,				"left"},
	};

	InputDeviceInfoStruct		InputDeviceInfoNESPort[] =
	{
		{"none",	"none",		NULL, 0,														NULL},
		{"gamepad",	"Gamepad",	NULL, sizeof(GamepadIDII) / sizeof(InputDeviceInputInfoStruct), GamepadIDII},
	};

	InputPortInfoStruct			PortInfo[] =
	{
		{0, "port1", "Port 1", sizeof(InputDeviceInfoNESPort) / sizeof(InputDeviceInfoStruct), InputDeviceInfoNESPort, "gamepad"},
	};

	InputInfoStruct				NestInput =
	{
		sizeof(PortInfo) / sizeof(InputPortInfoStruct),
		PortInfo
	};
}


