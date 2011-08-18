#include <es_system.h>

//Cancel VC specific keywords
#ifdef		__MINGW32__
# define	__in
# define	__out
# define	__reserved
#endif

//Includes for XInput
#include <windows.h>
#include <XInput.h>

namespace
{
	const char*					vaprint									(const char* aFormat, ...)
	{
		static char buf[1024];
		va_list args;
		va_start(args, aFormat);
		vsnprintf(buf, 1024, aFormat, args);
		va_end(args);

		return buf;
	}

	//Button Defintions for XInput pads
	static const struct
	{
		uint32_t					Mask;
		const char*					Name;
	}								XIButtons[] = 
	{
	
		{XINPUT_GAMEPAD_DPAD_UP,			"Up"			},
		{XINPUT_GAMEPAD_DPAD_DOWN,			"Down"			},
		{XINPUT_GAMEPAD_DPAD_LEFT,			"Left"			},
		{XINPUT_GAMEPAD_DPAD_RIGHT,			"Right"			},
		{XINPUT_GAMEPAD_START,				"Start"			},
		{XINPUT_GAMEPAD_BACK,				"Back"			},
		{XINPUT_GAMEPAD_LEFT_THUMB,			"Left Thumb"	},
		{XINPUT_GAMEPAD_RIGHT_THUMB,		"Right Thumb"	},
		{XINPUT_GAMEPAD_LEFT_SHOULDER,		"Left Shoulder"	},
		{XINPUT_GAMEPAD_RIGHT_SHOULDER,		"Right Shoulder"},
		{XINPUT_GAMEPAD_A,					"A"				},
		{XINPUT_GAMEPAD_B,					"B"				},
		{XINPUT_GAMEPAD_X,					"X"				},
		{XINPUT_GAMEPAD_Y,					"Y"				},
	};

	static uint32_t				esIndex[14] = {0, 1, 2, 3, 10, 11, 12, 13, 8, 9, 23, 24, 6, 7};

	//XInpuit pad info
	struct						PadState
	{
		bool					Valid;
		XINPUT_STATE			State;

		short					Axis[6];
	};

	static PadState				Pads[4];

	bool						FetchAxisLow							(uint32_t aPad, uint32_t aAxis, uint32_t aA)
	{
		return Pads[aPad].Valid ? ((Pads[aPad].Axis[aAxis] < -8192) ? true : false) : false;
	}

	bool						FetchAxisHigh							(uint32_t aPad, uint32_t aAxis, uint32_t aA)
	{
		return Pads[aPad].Valid ? ((Pads[aPad].Axis[aAxis] > 8192) ? true : false) : false;
	}

	bool						FetchButton								(uint32_t aPad, uint32_t aButton, uint32_t aA)
	{
		return Pads[aPad].Valid ? ((Pads[aPad].State.Gamepad.wButtons & aButton) ? true : false) : false;
	}
}

void							ESInputPlatform::Initialize				(ESInput::InputDeviceList& aDevices, ESInput::InputDeviceList& aSubDevices, uint32_t aESKeyIndex[14])
{
	memcpy(aESKeyIndex, esIndex, sizeof(esIndex));

	//Setup XInput
	XInputEnable(true);

	for(int i = 0; i != 4; i ++)
	{
		aDevices.push_back(ESInput::InputDevice());

		for(int j = 0; j != 14; j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchButton, i, XIButtons[j].Mask, 0, vaprint("Joy %d %s", i, XIButtons[j].Name)));
		}

		for(int j = 0; j != 4; j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchAxisHigh, i, j, 0, vaprint("Joy %d Axis %d High", i, j)));
			aDevices[i].push_back(ESInput::Button(FetchAxisLow, i, j, 0, vaprint("Joy %d Axis %d Low", i, j)));
		}

		aDevices[i].push_back(ESInput::Button(FetchAxisHigh, i, 4, 0, vaprint("Joy %d Left Trigger", i)));
		aDevices[i].push_back(ESInput::Button(FetchAxisHigh, i, 5, 0, vaprint("Joy %d Right Trigger", i)));
	}
}

void							ESInputPlatform::Shutdown				()
{
	XInputEnable(false);
}


void							SetExit									();
void							ESInputPlatform::Refresh				()
{
	//HACK
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			SetExit();
		}
	}

	//Poll XInput
	for(int i = 0; i != 4; i ++)
	{
		Pads[i].Valid = (ERROR_SUCCESS == XInputGetState(i, &Pads[i].State));

		if(Pads[i].Valid)
		{
			Pads[i].Axis[0] = Pads[i].State.Gamepad.sThumbLX;
			Pads[i].Axis[1] = Pads[i].State.Gamepad.sThumbLY;
			Pads[i].Axis[2] = Pads[i].State.Gamepad.sThumbRX;
			Pads[i].Axis[3] = Pads[i].State.Gamepad.sThumbRY;
			Pads[i].Axis[4] = Pads[i].State.Gamepad.bLeftTrigger << 7;
			Pads[i].Axis[5] = Pads[i].State.Gamepad.bRightTrigger << 7;
		}
	}	
}


