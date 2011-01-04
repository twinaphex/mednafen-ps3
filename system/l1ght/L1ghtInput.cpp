#include <ps3_system.h>

namespace
{
	uint32_t		ButtonIndex[][2] = {{2,8}, {2, 1}, {3, 0x80}, {3, 0x40}, {3, 0x10}, {3, 0x20}, {2, 0x10}, {2, 0x40}, {2, 0x80}, {2, 0x20}, {3, 4}, {3, 1}, {2, 2}, {3, 8}, {3, 2}, {2, 4}};
	std::string		ButtonNames[16] = {"START", "SELECT", "SQUARE", "CROSS", "TRIANGLE", "CIRCLE", "UP", "DOWN", "LEFT", "RIGHT", "L1", "L2", "L3", "R1", "R2", "R3"};	
	enum{PS3_AXIS_LEFT_Y, PS3_AXIS_LEFT_X, PS3_AXIS_RIGHT_Y, PS3_AXIS_RIGHT_X, PS3_AXIS_COUNT};

}

					L1ghtInput::L1ghtInput					()
{
	ioPadInit(MAXPADS);
	ioPadGetInfo(&Info);
	
	Reset();
	
	ThreadDie = false;
	sys_ppu_thread_create(&ThreadID, ProcessInputThread, 0, 0, 65536, 0, 0);

}

					L1ghtInput::~L1ghtInput					()
{
	ThreadDie = true;
	
	//Wait for at most one second for thread to die
	for(int i = 0; i != 10 && ThreadDie; i ++)
	{
		Utility::Sleep(100);
	}

	ioPadEnd();
}



uint32_t			L1ghtInput::PadCount					()
{
	return Info.connected;
}

void				L1ghtInput::Reset						()
{
	memset(HeldState, 0, sizeof(HeldState));
	memset(SingleState, 0xFF, sizeof(SingleState));
}


int32_t				L1ghtInput::GetAxis						(uint32_t aPad, uint32_t aAxis)
{
	Assert(aPad, 0, aAxis);

	int realaxis = AXISCOUNT + (AXISCOUNT - 1 - aAxis);
	return (int16_t)(CurrentState[aPad].button[realaxis]) - 0x80;
}

bool				L1ghtInput::ButtonPressed				(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	if(aButton == ES_BUTTON_UP && GetAxis(aPad, PS3_AXIS_LEFT_Y) < -0x40)	return true;
	if(aButton == ES_BUTTON_DOWN && GetAxis(aPad, PS3_AXIS_LEFT_Y) > 0x40)	return true;	
	if(aButton == ES_BUTTON_LEFT && GetAxis(aPad, PS3_AXIS_LEFT_X) < -0x40)return true;
	if(aButton == ES_BUTTON_RIGHT && GetAxis(aPad, PS3_AXIS_LEFT_X) > 0x40)return true;

	return HeldState[aPad][aButton];
}

bool				L1ghtInput::ButtonDown					(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	return HandleSingleState(HeldState[aPad][aButton & 0xFFFF], SingleState[aPad][aButton & 0xFFFF]);	
}

uint32_t			L1ghtInput::GetAnyButton				(uint32_t aPad)
{
	Assert(aPad, 0);
	
	for(int i = 0; i != BUTTONS; i ++)
	{
		if(HeldState[aPad][i])
		{
			return i;
		}
	}

	return 0xFFFFFFFF;
}

std::string			L1ghtInput::GetButtonName				(uint32_t aButton)
{
	Assert(0, aButton);

	return ButtonNames[aButton];
}

void				L1ghtInput::Assert						(uint32_t aPad, uint32_t aButton, uint32_t aAxis)
{
	if(aPad >= MAXPADS || aButton >= BUTTONS || aAxis >= AXISCOUNT)
	{
		Abort("L1ghtInput: Pad or Button out of range");
	}
}

void				L1ghtInput::ProcessInputThread			(uint64_t aBcD)
{
	while(!es_input)
	{
		Utility::Sleep(1);
	}

	L1ghtInput* input = (L1ghtInput*)es_input;

	while(!input->ThreadDie)
	{
		Utility::Sleep(15);
		input->Refresh();
	}

	if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3) && es_input->ButtonDown(0, ES_BUTTON_AUXLEFT3))
	{
		abort();
		exit(10);
	}

	input->ThreadDie = false;
	sys_ppu_thread_exit(0);	
}

void				L1ghtInput::Refresh						()
{
	ioPadGetInfo(&Info);
	
	for(int p = 0; p != PadCount(); p ++)
	{
		ioPadGetData(p, &CurrentState[p]);
	
		for(int i = 0; i != BUTTONS; i ++)
		{
			RefreshButton(CurrentState[p].button[ButtonIndex[i][0]] & ButtonIndex[i][1], HeldState[p][i], SingleState[p][i]);
		}
	}
}

