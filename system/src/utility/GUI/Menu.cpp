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
	uint32_t lasthit = 0;

	while(!WantToDie())
	{
		uint32_t now = Utility::GetTicks();

		if(now > lasthit + (20 * InputDelay))
		{
			es_input->Refresh();

			if(Hook && Hook->Input())
			{
				break;
			}
			
			if(Input())
			{
				break;
			}

			lasthit = now;
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

