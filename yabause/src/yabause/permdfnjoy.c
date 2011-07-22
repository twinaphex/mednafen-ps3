/*  Copyright 2005 Guillaume Duhamel
	Copyright 2005-2006 Theo Berkau
	Copyright 2008 Filipe Azevedo

	This file is part of Yabause.

	Yabause is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Yabause is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Yabause; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifdef MDFNPS3
#include "permdfnjoy.h"

static PerPad_struct *pad[12];

int PERMDFNJoyInit(void);
void PERMDFNJoyDeInit(void);
int PERMDFNJoyHandleEvents(void);
void PERMDFNJoyNothing(void);

u32 PERMDFNJoyScan(void);
void PERMDFNJoyFlush(void);
void PERMDFNKeyName(u32 key, char * name, int size);

PerInterface_struct PERMDFNJoy = {
PERCORE_MDFNJOY,
"Mednafen Joystick Interface",
PERMDFNJoyInit,
PERMDFNJoyDeInit,
PERMDFNJoyHandleEvents,
PERMDFNJoyNothing,
PERMDFNJoyScan,
1,
PERMDFNJoyFlush
#ifdef PERKEYNAME
,PERMDFNKeyName
#endif
};

extern u8*	mdfnyab_inputports[4];
static u16	lastportstate[4];

//////////////////////////////////////////////////////////////////////////////

int PERMDFNJoyInit(void) {
	//Add the pads
	pad[0] = PerPadAdd(&PORTDATA1);
	pad[1] = PerPadAdd(&PORTDATA2);

	//Add the button mapping
	for(int i = 0; i != 13; i ++)
	{
		PerSetKey(((0 << 16) | i) + 1, i, pad[0]);
		PerSetKey(((1 << 16) | i) + 1, i, pad[1]);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void PERMDFNJoyDeInit(void) {
}

//////////////////////////////////////////////////////////////////////////////

void PERMDFNJoyNothing(void) {
}

//////////////////////////////////////////////////////////////////////////////

int PERMDFNJoyHandleEvents(void) {
	// check each joysticks
	for(int i = 0; i != 4; i ++)
	{
		u16 newportstate = mdfnyab_inputports[i] ? (mdfnyab_inputports[i][0] | (mdfnyab_inputports[i][1] << 8)) : 0;
		for(int j = 0; j != 16; j ++)
		{
			if(newportstate & (1 << j))
			{
				PerKeyDown((i << 16) + j + 1);
			}
			else
			{
				PerKeyUp((i << 16) + j + 1);
			}
		}
	}
	
	// execute yabause
	if ( YabauseExec() != 0 )
	{
		return -1;
	}
	
	// return success
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

u32 PERMDFNJoyScan( void ) {
	for(int i = 0; i != 4; i ++)
	{
		if(mdfnyab_inputports[i])
		{
			u16 portdata = mdfnyab_inputports[i][0] | (mdfnyab_inputports[i][1] << 8);
			if(portdata)
			{
				for(int j = 0; j != 16; j ++)
				{
					if(portdata & (1 << j))
					{
						return ((i << 16) | j) + 1;
					}
				}
			}
		}
	}

	return 0;
}

void PERMDFNJoyFlush(void) {
}

void PERMDFNKeyName(u32 key, char * name, UNUSED int size)
{
	sprintf(name, "%x", (int)key);
}

#endif
