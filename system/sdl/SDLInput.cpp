#include <es_system.h>

void							ESInputPlatform::Initialize				(ESInput::InputDeviceList& aDevices, ESInput::InputDeviceList& aSubDevices, uint32_t aESKeyIndex[14])
{
	for(int i = 0; i != SDL_NumJoysticks(); i ++)
	{
		Joysticks.push_back(SDL_JoystickOpen(i));

		aDevices.push_back(ESInput::InputDevice());

		for(int j = 0; j != SDL_JoystickNumAxes(Joysticks[i]); j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchAxisLow, i, j, 0));
			aDevices[i].push_back(ESInput::Button(FetchAxisHigh, i, j, 0));
		}

		for(int j = 0; j != SDL_JoystickNumHats(Joysticks[i]); j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchHat, i, j, SDL_HAT_UP));
			aDevices[i].push_back(ESInput::Button(FetchHat, i, j, SDL_HAT_DOWN));
			aDevices[i].push_back(ESInput::Button(FetchHat, i, j, SDL_HAT_LEFT));
			aDevices[i].push_back(ESInput::Button(FetchHat, i, j, SDL_HAT_RIGHT));
		}

		for(int j = 0; j != SDL_JoystickNumButtons(Joysticks[i]); j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchButton, i, j, 0));
		}
	}

	//Add a keyboard subdevice
	aSubDevices.push_back(ESInput::InputDevice());

	int numkeys;
	SDL_GetKeyState(&numkeys);
	for(int j = 0; j != numkeys; j ++)
	{
		aSubDevices[0].push_back(ESInput::Button(FetchKey, j, 0, 0));
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

bool							ESInputPlatform::FetchKey				(uint32_t aKey, uint32_t aA, uint32_t aB)
{
	int numkeys;
	return SDL_GetKeyState(&numkeys)[aKey];
}

bool							ESInputPlatform::FetchAxisLow			(uint32_t aPad, uint32_t aAxis, uint32_t aA)
{
	return SDL_JoystickGetAxis(Joysticks[aPad], aAxis) < -0x4000;
}

bool							ESInputPlatform::FetchAxisHigh			(uint32_t aPad, uint32_t aAxis, uint32_t aA)
{
	return SDL_JoystickGetAxis(Joysticks[aPad], aAxis) > 0x4000;
}

bool							ESInputPlatform::FetchHat				(uint32_t aPad, uint32_t aHat, uint32_t aDirection)
{
	return SDL_JoystickGetHat(Joysticks[aPad], aHat) & aDirection;
}

bool							ESInputPlatform::FetchButton			(uint32_t aPad, uint32_t aButton, uint32_t aA)
{
	return SDL_JoystickGetButton(Joysticks[aPad], aButton);
}

std::vector<SDL_Joystick*>		ESInputPlatform::Joysticks;


