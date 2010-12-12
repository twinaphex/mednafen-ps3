#include <ps3_system.h>

namespace
{
	uint32_t		ButtonIndex[][2] = {{2,8}, {2, 1}, {3, 0x80}, {3, 0x40}, {3, 0x10}, {3, 0x20}, {2, 0x10}, {2, 0x40}, {2, 0x80}, {2, 0x20}, {3, 4}, {3, 1}, {2, 2}, {3, 8}, {3, 2}, {2, 4}};
}


std::string ButtonNames[16] = {"START", "SELECT", "SQUARE", "CROSS", "TRIANGLE", "CIRCLE", "UP", "DOWN", "LEFT", "RIGHT", "L1", "L2", "L3", "R1", "R2", "R3"};

void				PS3Input::Init							()
{
	ioPadInit(MAXPADS);
	ioPadGetInfo(&Info);
	
	Reset();
	
	ThreadDie = false;
	sys_ppu_thread_create(&ThreadID, ProcessInputThread, 0, 0, 65536, 0, 0);

}

void				PS3Input::Quit							()
{
	ThreadDie = true;
	
	//Wait for at most one second for thread to die
	for(int i = 0; i != 10 && ThreadDie; i ++)
	{
		Utility::Sleep(100);
	}

	ioPadEnd();
}



uint32_t			PS3Input::PadCount						()
{
	return Info.connected;
}

void				PS3Input::Reset							()
{
	memset(HeldState, 0, sizeof(HeldState));
	memset(HeldTime, 0xFFFFFFFF, sizeof(HeldTime));	
	memset(SingleState, 0, sizeof(SingleState));
}


int32_t				PS3Input::GetAxis						(uint32_t aPad, uint32_t aAxis)
{
	if(aPad >= PadCount() || aAxis >= AXISCOUNT)
	{
		return 0;
	}

	int realaxis = AXISCOUNT + (AXISCOUNT - 1 - aAxis);
	return (int16_t)(CurrentState[aPad].button[realaxis]) - 0x80;
}

bool				PS3Input::ButtonPressed					(uint32_t aPad, uint32_t aButton)
{
	if(aButton >= BUTTONS || aPad >= PadCount()) return false;

	if(aButton == PS3_BUTTON_UP && GetAxis(aPad, PS3_AXIS_LEFT_Y) < -0x40)	return true;
	if(aButton == PS3_BUTTON_DOWN && GetAxis(aPad, PS3_AXIS_LEFT_Y) > 0x40)	return true;	
	if(aButton == PS3_BUTTON_LEFT && GetAxis(aPad, PS3_AXIS_LEFT_X) < -0x40)return true;
	if(aButton == PS3_BUTTON_RIGHT && GetAxis(aPad, PS3_AXIS_LEFT_X) > 0x40)return true;
	
	return HeldState[aPad][aButton];
}

uint32_t			PS3Input::ButtonTime					(uint32_t aPad, uint32_t aButton)
{
	if(aButton >= BUTTONS || aPad >= PadCount()) return 0;
	
	return HeldTime[aPad][aButton];
}


bool				PS3Input::ButtonDown					(uint32_t aPad, uint32_t aButton)
{
	if(aButton >= BUTTONS || aPad >= PadCount()) return false;

	if(SingleState[aPad][aButton] == 0 || SingleState[aPad][aButton] == 2) return false;
	SingleState[aPad][aButton] = 2;
	return true;
}


void				PS3Input::ProcessInputThread			(uint64_t aBcD)
{
	while(!ThreadDie)
	{
		Utility::Sleep(15);
		Refresh();
	}
	
	ThreadDie = false;
}

void				PS3Input::Refresh						()
{
	ioPadGetInfo(&Info);
	
	for(int p = 0; p != PadCount(); p ++)
	{
		ioPadGetData(p, &CurrentState[p]);
	
		for(int i = 0; i != BUTTONS; i ++)
		{
			if(HeldTime[p][i] != 0xFFFFFFFF)
			{
				HeldState[p][i] = CurrentState[p].button[ButtonIndex[i][0]] & ButtonIndex[i][1];
				HeldTime[p][i] = HeldTime[p][i] == 0 ? Utility::GetTicks() : HeldTime[p][i];
				HeldTime[p][i] = HeldState[p][i] ? HeldTime[p][i] : 0;
		
				if(!ButtonPressed(p, i))
				{
					SingleState[p][i] = 0;
				}

				if(ButtonPressed(p, i) && SingleState[p][i] == 0)
				{
					SingleState[p][i] = 1;
				}
			}
			else
			{
				HeldTime[p][i] = (CurrentState[p].button[ButtonIndex[i][0]] & ButtonIndex[i][1]) ? 0xFFFFFFFF : 0;
			}
		}
	}
}

sys_ppu_thread_t	PS3Input::ThreadID;
volatile bool		PS3Input::ThreadDie = false;

PadInfo				PS3Input::Info;
PadData				PS3Input::CurrentState[MAXPADS];

uint32_t			PS3Input::HeldState[MAXPADS][BUTTONS];
uint32_t			PS3Input::HeldTime[MAXPADS][BUTTONS];
uint32_t			PS3Input::SingleState[MAXPADS][BUTTONS];		

