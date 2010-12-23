#include <ps3_system.h>

void				ESInput::Init							()
{
	Reset();
}

void				ESInput::Quit							()
{
}



uint32_t			ESInput::PadCount						()
{
	return 1;
}

void				ESInput::Reset							()
{
	memset(HeldState, 0, sizeof(HeldState));
	memset(HeldTime, 0xFFFFFFFF, sizeof(HeldTime));	
	memset(SingleState, 0, sizeof(SingleState));
}


int32_t				ESInput::GetAxis						(uint32_t aPad, uint32_t aAxis)
{
//	if(aPad >= PadCount() || aAxis >= AXISCOUNT)
//	{
//		return 0;
//	}

//	int realaxis = AXISCOUNT + (AXISCOUNT - 1 - aAxis);
//	return (int16_t)(CurrentState[aPad].button[realaxis]) - 0x80;
}

bool				ESInput::ButtonPressed					(uint32_t aPad, uint32_t aButton)
{
	if(aPad >= PadCount()) return false;

	return HeldState[aPad][aButton];
}

uint32_t			ESInput::ButtonTime						(uint32_t aPad, uint32_t aButton)
{
	if(aPad >= PadCount()) return 0;
	
	return HeldTime[aPad][aButton];
}


bool				ESInput::ButtonDown						(uint32_t aPad, uint32_t aButton)
{
	if(aPad >= PadCount()) return false;

	if(SingleState[aPad][aButton] == 0 || SingleState[aPad][aButton] == 2) return false;
	SingleState[aPad][aButton] = 2;
	return true;
}

uint32_t			ESInput::GetAnyButton					(uint32_t aPad)
{
	if(aPad >= PadCount()) return 0;

	int numkeys;
	uint8_t* keys = SDL_GetKeyState(&numkeys);
	
	for(int i = 0; i != numkeys; i ++)
	{
		if(keys[i])
		{
			return i;
		}
	}

	return 0;
}

std::string			ESInput::GetButtonName					(uint32_t aButton)
{
	return SDL_GetKeyName((SDLKey)aButton);
}

void				ESInput::ProcessInputThread				(uint64_t aBcD)
{
}

void				ESInput::Refresh						()
{
	int numkeys;
	uint8_t* keys = SDL_GetKeyState(&numkeys);

	int p = 0;
	
	for(int i = 0; i != numkeys; i ++)
	{
		if(HeldTime[p][i] != 0xFFFFFFFF)
		{
			HeldState[p][i] = keys[i] ? 1 : 0;
			HeldTime[p][i] = HeldTime[p][i] == 0 ? Utility::GetTicks() : HeldTime[p][i];
			HeldTime[p][i] = HeldState[p][i] ? HeldTime[p][i] : 0;
	
			if(!ButtonPressed(p, i))
			{
				SingleState[p][i] = 0;
			}

			if(ButtonPressed(p, i) && SingleState[p][i] == 0)
			{
				SingleState[p][i] = 1;
			}
		}
		else
		{
			HeldTime[p][i] = (keys[i]) ? 0xFFFFFFFF : 0;
		}
	}
}

uint32_t			ESInput::HeldState[MAXPADS][BUTTONS];
uint32_t			ESInput::HeldTime[MAXPADS][BUTTONS];
uint32_t			ESInput::SingleState[MAXPADS][BUTTONS];		

