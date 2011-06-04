#include <es_system.h>


							FastCounter::FastCounter			(uint32_t aSpeed, uint32_t aButton)
{
	Button = aButton;
	Speed = aSpeed;
	LastFPS = 0;
	LastSkip = 0;
	LastFPSTime = 0;
	FrameCount = 0;
	SkipCount = 0;
}

							FastCounter::~FastCounter			()
{
}

bool						FastCounter::Fast					()
{
	return ESInput::ButtonPressed(0, Button);
}

uint32_t					FastCounter::GetSpeed				()
{
	return Fast() ? Speed : 1;
}

void						FastCounter::Tick					(bool aSkip)
{
	FrameCount ++;
	SkipCount += aSkip ? 1 : 0;
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
		
