#include <es_system.h>

namespace
{
	uint32_t		ButtonIndex[][2] = {{2,8}, {2, 1}, {3, 0x80}, {3, 0x40}, {3, 0x10}, {3, 0x20}, {2, 0x10}, {2, 0x40}, {2, 0x80}, {2, 0x20}, {3, 4}, {3, 1}, {2, 2}, {3, 8}, {3, 2}, {2, 4}};
	std::string		ButtonNames[16] = {"Start", "Select", "Square", "Cross", "Triangle", "Circle", "Up", "Down", "Left", "Right", "L1", "L2", "L3", "R1", "R2", "R3"};	
	enum{PS3_AXIS_LEFT_Y, PS3_AXIS_LEFT_X, PS3_AXIS_RIGHT_Y, PS3_AXIS_RIGHT_X, PS3_AXIS_COUNT};

}

					DorkInput::DorkInput					()
{
	cellPadInit(MAXPADS);

	cellPadGetInfo2(&PadInfo);
	
	Reset();

	Small = 0;
	Large = 0;
}

					DorkInput::~DorkInput					()
{
	cellPadEnd();
}

uint32_t			DorkInput::PadCount						()
{
	return PadInfo.now_connect;
}

void				DorkInput::Reset						()
{
	for(int i = 0; i != PadCount(); i ++)
	{
		cellPadClearBuf(i);
	}

	memset(HeldState, 0xFF, sizeof(HeldState));
	memset(SingleState, 0xFF, sizeof(SingleState));
}

void				DorkInput::Refresh						()
{
	cellPadGetInfo2(&PadInfo);

	if(!(PadInfo.system_info & CELL_PAD_INFO_INTERCEPTED))
	{
		for(int p = 0; p != PadCount(); p ++)
		{
			CellPadData newstate;
			cellPadGetData(p, &newstate);

			if(newstate.len != 0)
			{
				CurrentState[p] = newstate;

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
		}

		//TODO: Support rumble for more than 1 controller
		CellPadActParam param = {Small & 1, Large & 0xFF};
		cellPadSetActDirect(0, &param);
	}
}

int32_t				DorkInput::GetAxis						(uint32_t aPad, uint32_t aAxis)
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

bool				DorkInput::ButtonPressed				(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	return HeldState[aPad][aButton] == 1;
}

bool				DorkInput::ButtonDown					(uint32_t aPad, uint32_t aButton)
{
	Assert(aPad, aButton);

	return HandleSingleState(HeldState[aPad][aButton], SingleState[aPad][aButton]);	
}

uint32_t			DorkInput::GetAnyButton					(uint32_t aPad)
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

std::string			DorkInput::GetButtonName				(uint32_t aButton)
{
	Assert(0, aButton);

	return ButtonNames[aButton];
}

std::string			DorkInput::GetButtonImage				(uint32_t aButton)
{
	Assert(0, aButton);

	return ButtonNames[aButton] + "IMAGE";
}

void				DorkInput::Assert						(uint32_t aPad, uint32_t aButton, uint32_t aAxis)
{
	if(aPad >= MAXPADS || aButton >= BUTTONS || aAxis >= AXISCOUNT)
	{
		Abort("DorkInput: Pad or Button out of range");
	}
}

void				DorkInput::RumbleOn						(uint32_t aSmall, uint32_t aLarge)
{
	Small = aSmall;
	Large = aLarge;
	Refresh();
}

