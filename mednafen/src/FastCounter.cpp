#include <es_system.h>
#include "FastCounter.h"

bool						FastCounter::Fast					()
{
	return ESInput::ButtonPressed(0, Button);
}

uint32_t					FastCounter::GetFPS					(uint32_t* aSkip)
{
	uint32_t time = Utility::GetTicks();
	
	if(time - LastFPSTime >= 1000)
	{
		LastFPS = FrameCount;
		LastSkip = SkipCount;
		FrameCount = 0;
		SkipCount = 0;
		LastFPSTime = time;
	}
	
	if(aSkip)
	{
		*aSkip = LastSkip;
	}

	return LastFPS;
}
		
