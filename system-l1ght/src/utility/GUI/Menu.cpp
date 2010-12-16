#include <ps3_system.h>

							Menu::Menu					(MenuHook* aHook)
{
	Hook = aHook;
	InputDelay = 5;
}

							Menu::~Menu					()
{
							
}

void						Menu::SetInputDelay			(uint32_t aDelay)
{
	if(aDelay > 10)
	{
		throw "Menu::SetInputDelay: Delay cannot be more than ten frames.";
	}
	
	InputDelay = aDelay;
}

bool						Menu::Do					()
{
	uint32_t ticks = 0;

	while(!WantToDie())
	{
		if(ticks ++ == InputDelay)
		{
			ticks = 0;
		
			if(Hook && Hook->Input())
			{
				PS3Input::Reset();			
				return true;
			}
			
			if(Input())
			{
				PS3Input::Reset();			
				return true;
			}
		}
		
		if(Draw())
		{
			PS3Input::Reset();
			return true;
		}
				
		PS3Video::Flip();
	}
	
	return false;
}

void						Menu::SetHook				(MenuHook* aHook)
{
	Hook = aHook;
}
