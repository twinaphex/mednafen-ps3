#include <ps3_system.h>

					SDLInput::SDLInput						()
{
	for(int i = 0; i != SDL_NumJoysticks(); i ++)
	{
		Joysticks.push_back(SDL_JoystickOpen(i));
	}

	Reset();

	RefreshTimer = SDL_AddTimer(10, RefreshESInput, 0);
}

					SDLInput::~SDLInput						()
{
	SDL_RemoveTimer(RefreshTimer);

	for(int i = 0; i != Joysticks.size(); i ++)
	{
		SDL_JoystickClose(Joysticks[i]);
	}
}



uint32_t			SDLInput::PadCount						()
{
	return Joysticks.size() ? Joysticks.size() : 1;
}

void				SDLInput::Reset							()
{
	memset(HeldState, 0, sizeof(HeldState));
	memset(SingleState, 0xFF, sizeof(SingleState));
	
	memset(KeyState, 0, sizeof(KeyState));
	memset(KeySingle, 0xFF, sizeof(KeySingle));
}


int32_t				SDLInput::GetAxis						(uint32_t aPad, uint32_t aAxis)
{
	//TODO: 
	return 0;
}

bool				SDLInput::ButtonPressed					(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	if(!IsJoystickButton(aButton))
	{
		return KeyState[aButton & 0xFFFF];
	}
	else
	{
		return HeldState[aPad][aButton & 0xFFFF];
	}
}

bool				SDLInput::ButtonDown					(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	if(!IsJoystickButton(aButton))
	{
		return HandleSingleState(KeyState[aButton & 0xFFFF], KeySingle[aButton & 0xFFFF]);
	}
	else
	{
		return HandleSingleState(HeldState[aPad][aButton & 0xFFFF], SingleState[aPad][aButton & 0xFFFF]);	
	}
}

uint32_t			SDLInput::GetAnyButton					(uint32_t aPad)
{
	Assert(aPad, 0);

	if(aPad == 0)
	{
		for(int i = 0; i != MAXKEYS; i ++)
		{
			if(KeyState[i])
			{
				return (1 << 16) | i;
			}
		}
	}

	for(int i = 0; i != (SDL_JoystickNumAxes(Joysticks[aPad]) * 2 + SDL_JoystickNumButtons(Joysticks[aPad])); i ++)
	{
		if(HeldState[aPad][i])
		{
			return i;
		}
	}

	return 0xFFFFFFFF;
}

std::string			SDLInput::GetButtonName					(uint32_t aButton)
{
	Assert(0, aButton);
	
	if(!IsJoystickButton(aButton))
	{
		return SDL_GetKeyName((SDLKey)(aButton & 0xFFFF));
	}
	else
	{
		//TODO: Return real string
		return std::string("JSBUTTON");
	}
}

void				SDLInput::Assert						(uint32_t aPad, uint32_t aButton)
{
	if(aPad >= MAXPADS || (IsJoystickButton(aButton) && aButton >= BUTTONS) || (!IsJoystickButton(aButton) && (aButton & 0xFFFF) >= MAXKEYS))
	{
		Abort("SDLInput: Pad or Button out of range.");
	}
}

bool				SDLInput::IsJoystickButton				(uint32_t aButton)
{
	return (aButton >> 16) == 0;
}

uint32_t			SDLInput::RefreshESInput				(uint32_t aInterval, void* aParam)
{
	if(es_input)
	{
		((SDLInput*)es_input)->Refresh();
	}

	return 1;
}

void				SDLInput::Refresh						()
{
	int numkeys;
	uint8_t* keys = SDL_GetKeyState(&numkeys);

	for(int j = 0; j != numkeys && j != MAXKEYS; j ++)
	{
		RefreshButton(keys[j], KeyState[j], KeySingle[j]);
	}

	for(int i = 0; i != Joysticks.size(); i ++)
	{
		uint32_t buttonCount = (2 * SDL_JoystickNumAxes(Joysticks[i])) + SDL_JoystickNumButtons(Joysticks[i]);
	
		for(int j = 0; j != SDL_JoystickNumAxes(Joysticks[i]); j ++)
		{
			RefreshButton(SDL_JoystickGetAxis(Joysticks[i], j) < -0x4000, HeldState[i][j * 2], SingleState[i][j * 2]);
			RefreshButton(SDL_JoystickGetAxis(Joysticks[i], j) > 0x4000, HeldState[i][j * 2 + 1], SingleState[i][j * 2 + 1]);
		}
	
		uint32_t buttonIndex = SDL_JoystickNumAxes(Joysticks[i]) * 2;
	
		for(int j = 0; j != SDL_JoystickNumButtons(Joysticks[i]); j ++)
		{
			RefreshButton(SDL_JoystickGetButton(Joysticks[i], j), HeldState[i][j + buttonIndex], SingleState[i][j + buttonIndex]);
		}
	}
}

