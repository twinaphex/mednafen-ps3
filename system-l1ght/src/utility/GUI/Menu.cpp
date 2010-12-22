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
		printf("Menu::SetInputDelay: Delay cannot be more than ten frames, you tried '%d.\n", aDelay);
		aDelay = 10;
	}
	
	InputDelay = aDelay;
}

void						Menu::Do					()
{
	uint32_t ticks = 0;

	while(!WantToDie())
	{
		if(ticks ++ == InputDelay)
		{
			ticks = 0;
		
			if(Hook && Hook->Input())
			{
				break;
			}
			
			if(Input())
			{
				break;
			}
		}
		
		if(Draw())
		{
			break;
		}
				
		PS3Video::Flip();
	}
	
	PS3Input::Reset();
}

void						Menu::SetHook				(MenuHook* aHook)
{
	Hook = aHook;
}
