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
	return !PS3Input::ButtonPressed(0, Button) || Counter == 0;
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
		
							Logger::Logger						(std::string aHeader) : TextViewer("", aHeader)
{
}

							Logger::~Logger						()
{
}
									
void						Logger::Log							(const char* aMessage, ...)
{
	char array[1024];
	va_list args;
	va_start (args, aMessage);
	vsnprintf(array, 1024, aMessage, args);
	va_end(args);
	
	Lines.push_back(std::string(array));
}

