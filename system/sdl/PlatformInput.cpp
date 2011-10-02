#include <es_system.h>
#include <SDL.h>
#include "SDLInputConfig.h"

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

	//SDL joystick objects
	std::vector<SDL_Joystick*>		Joysticks;

	//State callbacks
	uint16_t						FetchKey							(uint32_t aKey, uint32_t aA, uint32_t aB)
	{
		int numkeys;
		return SDL_GetKeyState(&numkeys)[aKey] ? 65535 : 0;
	}

	uint16_t						FetchAxisLow						(uint32_t aPad, uint32_t aAxis, uint32_t aA)
	{
		int16_t value = SDL_JoystickGetAxis(Joysticks[aPad], aAxis);
		return (value < -0x1000) ? abs(value) << 1 : 0;
	}

	uint16_t						FetchAxisHigh						(uint32_t aPad, uint32_t aAxis, uint32_t aA)
	{
		int16_t value = SDL_JoystickGetAxis(Joysticks[aPad], aAxis);
		return (value > 0x1000) ? abs(value) << 1 : 0;
	}

	uint16_t						FetchHat							(uint32_t aPad, uint32_t aHat, uint32_t aDirection)
	{
		return (SDL_JoystickGetHat(Joysticks[aPad], aHat) & aDirection) ? 65535 : 0;
	}

	uint16_t						FetchButton							(uint32_t aPad, uint32_t aButton, uint32_t aA)
	{
		return SDL_JoystickGetButton(Joysticks[aPad], aButton) ? 65535 : 0;
	}
}

void							ESInputPlatform::Initialize				(ESInput::InputDeviceList& aDevices, ESInput::InputDeviceList& aSubDevices, uint32_t aESKeyIndex[14])
{
	for(int i = 0; i != SDL_NumJoysticks(); i ++)
	{
		Joysticks.push_back(SDL_JoystickOpen(i));

		aDevices.push_back(ESInput::InputDevice());

		for(int j = 0; j != SDL_JoystickNumAxes(Joysticks[i]); j ++)
		{
			aDevices[i].push_back(ESInput_Button(FetchAxisLow, i, j, 0, vaprint("Joy %d Axis %d Low", i, j)));
			aDevices[i].push_back(ESInput_Button(FetchAxisHigh, i, j, 0, vaprint("Joy %d Axis %d High", i, j)));
		}

		for(int j = 0; j != SDL_JoystickNumHats(Joysticks[i]); j ++)
		{
			aDevices[i].push_back(ESInput_Button(FetchHat, i, j, SDL_HAT_UP, vaprint("Joy %d Hat %d Up", i, j)));
			aDevices[i].push_back(ESInput_Button(FetchHat, i, j, SDL_HAT_DOWN, vaprint("Joy %d Hat %d Down", i, j)));
			aDevices[i].push_back(ESInput_Button(FetchHat, i, j, SDL_HAT_LEFT, vaprint("Joy %d Hat %d Left", i, j)));
			aDevices[i].push_back(ESInput_Button(FetchHat, i, j, SDL_HAT_RIGHT, vaprint("Joy %d Hat %d Right", i, j)));
		}

		for(int j = 0; j != SDL_JoystickNumButtons(Joysticks[i]); j ++)
		{
			aDevices[i].push_back(ESInput_Button(FetchButton, i, j, 0, vaprint("Joy %d Button %d", i, j)));
		}
	}

	//Add a keyboard subdevice
	aSubDevices.push_back(ESInput::InputDevice());

	int numkeys;
	SDL_GetKeyState(&numkeys);
	for(int j = 0; j != numkeys; j ++)
	{
		aSubDevices[0].push_back(ESInput_Button(FetchKey, j, 0, 0, vaprint("KEY %s", SDL_GetKeyName((SDLKey)j))));
	}

	//Load ES Keys
	SDLInputConfig::Load(aESKeyIndex);
}

void							ESInputPlatform::Shutdown				()
{
	for(int i = 0; i != Joysticks.size(); i ++)
	{
		SDL_JoystickClose(Joysticks[i]);
	}

	Joysticks.clear();
}

void							SetExit									();
void							ESInputPlatform::Refresh				()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			SetExit();
		}
	}
	
	int numkeys;
	if(SDL_GetKeyState(&numkeys)[SDLK_F10])
	{
		SetExit();
	}
}


