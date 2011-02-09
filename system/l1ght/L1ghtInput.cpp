#include <ps3_system.h>

namespace
{
	uint32_t		ButtonIndex[][2] = {{2,8}, {2, 1}, {3, 0x80}, {3, 0x40}, {3, 0x10}, {3, 0x20}, {2, 0x10}, {2, 0x40}, {2, 0x80}, {2, 0x20}, {3, 4}, {3, 1}, {2, 2}, {3, 8}, {3, 2}, {2, 4}};
	std::string		ButtonNames[16] = {"Start", "Select", "Square", "Cross", "Triangle", "Circle", "Up", "Down", "Left", "Right", "L1", "L2", "L3", "R1", "R2", "R3"};	
	enum{PS3_AXIS_LEFT_Y, PS3_AXIS_LEFT_X, PS3_AXIS_RIGHT_Y, PS3_AXIS_RIGHT_X, PS3_AXIS_COUNT};

}

					L1ghtInput::L1ghtInput					()
{
	ioPadInit(MAXPADS);
	ioPadGetInfo(&Info);
	
	Reset();

	Small = 0;
	Large = 0;
}

					L1ghtInput::~L1ghtInput					()
{
	ioPadEnd();
}

uint32_t			L1ghtInput::PadCount					()
{
	return Info.connected;
}

void				L1ghtInput::Reset						()
{
	memset(HeldState, 0xFF, sizeof(HeldState));
	memset(SingleState, 0xFF, sizeof(SingleState));
}

void				L1ghtInput::Refresh						()
{
	ioPadGetInfo(&Info);
	
	for(int p = 0; p != PadCount(); p ++)
	{
		ioPadGetData(p, &CurrentState[p]);
	
		for(int i = 0; i != BUTTONS - 8; i ++)
		{
			RefreshButton(CurrentState[p].button[ButtonIndex[i][0]] & ButtonIndex[i][1], HeldState[p][i], SingleState[p][i]);
		}

		for(int i = 0; i != 4; i ++)
		{
			RefreshButton(GetAxis(0, i) < -0x40, HeldState[p][16 + i * 2 + 0], SingleState[p][16 + i * 2 + 0]);
			RefreshButton(GetAxis(0, i) >  0x40, HeldState[p][16 + i * 2 + 1], SingleState[p][16 + i * 2 + 1]);
		}
	}

	PadActParam param = {Small & 1, Large & 0xFF};
	ioPadSetActDirect(0, &param);
}




int32_t				L1ghtInput::GetAxis						(uint32_t aPad, uint32_t aAxis)
{
	Assert(aPad, 0, aAxis);

	if(aPad < PadCount())
	{
		int realaxis = AXISCOUNT + (AXISCOUNT - 1 - aAxis);
		return (int16_t)(CurrentState[aPad].button[realaxis]) - 0x80;
	}
	else
	{
		return 0;
	}
}

bool				L1ghtInput::ButtonPressed				(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	return HeldState[aPad][aButton] == 1;
}

bool				L1ghtInput::ButtonDown					(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	return HandleSingleState(HeldState[aPad][aButton], SingleState[aPad][aButton]);	
}

uint32_t			L1ghtInput::GetAnyButton				(uint32_t aPad)
{
	Assert(aPad, 0);
	
	for(int i = 0; i != BUTTONS; i ++)
	{
		if(HeldState[aPad][i] == 1)
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

std::string			L1ghtInput::GetButtonImage				(uint32_t aButton)
{
	Assert(0, aButton);

	return ButtonNames[aButton] + "IMAGE";
}

void				L1ghtInput::Assert						(uint32_t aPad, uint32_t aButton, uint32_t aAxis)
{
	if(aPad >= MAXPADS || aButton >= BUTTONS || aAxis >= AXISCOUNT)
	{
		Abort("L1ghtInput: Pad or Button out of range");
	}
}

void				L1ghtInput::RumbleOn					(uint32_t aSmall, uint32_t aLarge)
{
	Small = aSmall;
	Large = aLarge;
	Refresh();
}

