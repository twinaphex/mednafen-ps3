#include <es_system.h>

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
	InputDelay = aDelay;
}

void						Menu::Do					()
{
	uint32_t ticks = InputDelay;

	while(!WantToDie())
	{
		if(ticks ++ == InputDelay)
		{
			es_input->Refresh();

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
				
		es_video->Flip();
	}
	
	es_input->Reset();
}

void						Menu::SetHook				(MenuHook* aHook)
{
	Hook = aHook;
}

