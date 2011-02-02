#include <ps3_system.h>

//HACK: Secret menu for me, not you
#ifdef L1GHT
#include <l1ght/SecretMenu.h>
#endif

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
			ticks = 0;
		
			if(Hook && Hook->Input())
			{
				break;
			}
			
			if(Input())
			{
				break;
			}

			//HACK: Secret menu for me, not you
			#ifdef L1GHT
			//Start and select
			if(es_input->ButtonDown(0, 0) && es_input->ButtonDown(0, 1))
			{
				L1ghtSecret().Do();
			}
			#endif
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

