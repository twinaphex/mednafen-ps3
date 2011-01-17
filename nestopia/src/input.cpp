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
	Input::Type					Types[Input::NUM_PADS];
	uint32_t					Flags[Input::NUM_PADS];
	uint8_t*					Ports[Input::NUM_PADS];

	struct						InputNameTable
	{
		std::string				Name;
		Input::Type				Type;
		uint32_t				Flags;
	};

	InputNameTable				InputTypes[] = 
	{
		{"none",		Input::UNCONNECTED,	0},
		{"gamepad", 	Input::PAD1,		0},
		{"zapper",		Input::ZAPPER,		0},
		{"powerpada",	Input::POWERPAD,	0},
		{"powerpadb",	Input::POWERPAD,	1},
		{"arkanoid",	Input::PADDLE,		0}
	};

	//Functions
	void						PluginController					(int32_t aPort, const char* aType, uint8_t* aData)
	{
		if(aPort >= 0 && aPort < Input::NUM_PADS)
		{
			Ports[aPort] = aData;
			Types[aPort] = Input::UNCONNECTED;
			Flags[aPort] = 0;

			for(int i = 0; i != 6; i ++)
			{
				if(InputTypes[i].Name == aType)
				{
					Types[aPort] = InputTypes[i].Type;
					Flags[aPort] = InputTypes[i].Flags;
				}
			}

			Input(Nestopia).ConnectController(aPort, Types[aPort]);
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
			if(Types[i] == Input::PAD1)
			{
				EmuPads.pad[i].buttons = Ports[i] ? Ports[i][0] : 0;
			}
			else if(Types[i] == Input::ZAPPER)
			{
				EmuPads.zapper.x = *(uint32_t*)&Ports[i][0];
				EmuPads.zapper.y = *(uint32_t*)&Ports[i][4];
				EmuPads.zapper.fire = Ports[i][8];
			}
			else if(Types[i] == Input::POWERPAD)
			{
				bool* buttonstash = Flags[i] ? EmuPads.powerPad.sideA : EmuPads.powerPad.sideB;
				uint32_t numbuttons = Flags[i] ? EmuPads.powerPad.NUM_SIDE_A_BUTTONS : EmuPads.powerPad.NUM_SIDE_B_BUTTONS;

				for(int j = 0; j != EmuPads.powerPad.NUM_SIDE_A_BUTTONS; j ++)
				{
					buttonstash[j] = (Ports[i][0] & (1 << j)) ? 1 : 0;
				}

				if(numbuttons > 8)
				{
					for(int j = 8; j != numbuttons; j ++)
					{
						buttonstash[j] = (Ports[i][1] & (1 << (j - 8))) ? 1 : 0;
					}
				}
			}
			else if(Types[i] == Input::PADDLE)
			{
				EmuPads.paddle.x = *(uint32_t*)&Ports[i][0];
				EmuPads.paddle.button = Ports[i][4];
			}
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

	InputDeviceInputInfoStruct	ZapperIDII[] =
	{
		{"x_axis",			"X Axis",		-1,	IDIT_X_AXIS},
		{"y_axis",			"Y Axis",		-1,	IDIT_Y_AXIS},
		{"trigger",			"Trigger",		0,	IDIT_BUTTON,	NULL},
	};

	InputDeviceInputInfoStruct	PowerpadIDII[] =
	{
		{"1",	"1",	0,	IDIT_BUTTON, NULL},
		{"2",	"2",	1,	IDIT_BUTTON, NULL},
		{"3",	"3",	2,	IDIT_BUTTON, NULL},
		{"4",	"4",	3,	IDIT_BUTTON, NULL},
		{"5",	"5",	4,	IDIT_BUTTON, NULL},
		{"6",	"6",	5,	IDIT_BUTTON, NULL},
		{"7",	"7",	6,	IDIT_BUTTON, NULL},
		{"8",	"8",	7,	IDIT_BUTTON, NULL},
		{"9",	"9",	8,	IDIT_BUTTON, NULL},
		{"10",	"10",	9,	IDIT_BUTTON, NULL},
		{"11",	"11",	10,	IDIT_BUTTON, NULL},
		{"12",	"12",	11,	IDIT_BUTTON, NULL},
	};

	InputDeviceInputInfoStruct	ArkanoidIDII[] =
	{
		{"x_axis",	"X Axis",	-1,	IDIT_X_AXIS},
		{"button",	"Button",	0,	IDIT_BUTTON,	NULL},
	};

	InputDeviceInfoStruct		InputDeviceInfoNESPort[] =
	{
		{"none",		"none",				NULL,	0,	NULL},
		{"gamepad",		"Gamepad",			NULL,	8,	GamepadIDII},
		{"zapper",		"Zapper",			NULL,	3,	ZapperIDII},
		{"powerpada",	"Power Pad Side A",	NULL,	12,	PowerpadIDII},
		{"powerpadb",	"Power Pad Side b",	NULL,	12,	PowerpadIDII},
		{"arkanoid",	"Arkanoid Paddle",	NULL,	2,	ArkanoidIDII},
	};

	InputPortInfoStruct			PortInfo[] =
	{
		{0, "port1", "Port 1", 6, InputDeviceInfoNESPort, "gamepad"},
		{0, "port2", "Port 2", 6, InputDeviceInfoNESPort, "gamepad"},
	};

	InputInfoStruct				NestInput =
	{
		sizeof(PortInfo) / sizeof(InputPortInfoStruct),
		PortInfo
	};
}


