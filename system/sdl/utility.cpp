#include <ps3_system.h>


							FastCounter::FastCounter			(uint32_t aSpeed, uint32_t aButton)
{
	Button = aButton;
	Speed = aSpeed;
	Counter = 0;
	LastFPS = 0;
	LastFPSTime = 0;
	FrameCount = 0;
}

							FastCounter::~FastCounter			()
{
}

void						FastCounter::Tick					()
{
	Counter = ((Counter + 1) == Speed) ? 0 : Counter + 1;
	
	FrameCount ++;
}

bool						FastCounter::DrawNow				()
{
	return !es_input->ButtonPressed(0, Button) || Counter == 0;
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
		
