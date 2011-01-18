#include <ps3_system.h>


							FastCounter::FastCounter			(uint32_t aSpeed, uint32_t aButton)
{
	Button = aButton;
	Speed = aSpeed;
	LastFPS = 0;
	LastFPSTime = 0;
	FrameCount = 0;
}

							FastCounter::~FastCounter			()
{
}

bool						FastCounter::Fast					()
{
	return es_input->ButtonPressed(0, Button);
}

uint32_t					FastCounter::GetSpeed				()
{
	return Fast() ? Speed : 1;
}

void						FastCounter::Tick					()
{
	FrameCount ++;
}

uint32_t					FastCounter::GetFPS					()
{
	uint32_t time = Utility::GetTicks();
	
	if(time - LastFPSTime >= 1000)
	{
		LastFPS = FrameCount;
		FrameCount = 0;
		LastFPSTime = time;
	}
	
	return LastFPS;
}
		
