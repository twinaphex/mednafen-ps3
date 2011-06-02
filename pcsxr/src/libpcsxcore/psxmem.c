/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
* PSX memory functions.
*/

#include "psxmem.h"
#include "r3000a.h"
#include "psxhw.h"

PSXMEM_MemoryMap			PSXMEM_Memory;

/*  Playstation Memory Map (from Playstation doc by Joshua Walker)
0x0000_0000-0x0000_ffff		Kernel (64K)
0x0001_0000-0x001f_ffff		User Memory (1.9 Meg)

0x1f00_0000-0x1f00_ffff		Parallel Port (64K)

0x1f80_0000-0x1f80_03ff		Scratch Pad (1024 bytes)

0x1f80_1000-0x1f80_2fff		Hardware Registers (8K)

0x1fc0_0000-0x1fc7_ffff		BIOS (512K)

0x8000_0000-0x801f_ffff		Kernel and User Memory Mirror (2 Meg) Cached
0x9fc0_0000-0x9fc7_ffff		BIOS Mirror (512K) Cached

0xa000_0000-0xa01f_ffff		Kernel and User Memory Mirror (2 Meg) Uncached
0xbfc0_0000-0xbfc7_ffff		BIOS Mirror (512K) Uncached
*/


OPFUNC(psxNULL);
int							psxMemInit					()
{
	memset(&PSXMEM_Memory, 0, sizeof(PSXMEM_MemoryMap));

	//Init ops
	psxOpFunc* opLists[4] = {PSXMEM_Memory.WorkOPS, PSXMEM_Memory.ScratchOPS, PSXMEM_Memory.BIOSOPS, PSXMEM_Memory.ParallelOPS};
	uint32_t opLens[4] = {2 * 1024 * 1024 / 4, 9 * 1024 / 4, 512 * 1024 / 4, 64 * 1024 / 4};

	for(int i = 0; i != 4; i ++)
	{
		for(int j = 0; j != opLens[i]; j ++)
		{
			opLists[i][j] = PSXCPU_Resolve;
		}
	}

	for(int i = 0; i != 0x10000; i ++)
	{
		PSXMEM_Memory.FakeOPS[i] = psxNULL;
	}

	//Init LUTs
	for(int i = 0; i != 0x10000; i ++)	//Better than just crashing!
	{
		PSXMEM_Memory.ReadTable[i]				=	PSXMEM_Memory.FakePage;
		PSXMEM_Memory.WriteTable[i]				=	PSXMEM_Memory.FakePage;
		PSXMEM_Memory.OPTable[i]				=	PSXMEM_Memory.FakeOPS;
	}

	for(int i = 0; i != 0x80; i ++)
	{
		PSXMEM_Memory.ReadTable[i]				=	&PSXMEM_Memory.WorkRAM[(i & 0x1F) << 16];
		PSXMEM_Memory.WriteTable[i]				=	&PSXMEM_Memory.WorkRAM[(i & 0x1F) << 16];
		PSXMEM_Memory.OPTable[i]				=	&PSXMEM_Memory.WorkOPS[(i & 0x1F) << 14];
	}

	PSXMEM_Memory.ReadTable[0x1F00]				=	PSXMEM_Memory.Parallel;
	PSXMEM_Memory.ReadTable[0x1F80]				=	PSXMEM_Memory.ScratchRAM;
	PSXMEM_Memory.OPTable[0x1F00]				=	PSXMEM_Memory.ParallelOPS;
	PSXMEM_Memory.OPTable[0x1F80]				=	PSXMEM_Memory.ScratchOPS;
	PSXMEM_Memory.WriteTable[0x1F00]			=	PSXMEM_Memory.Parallel;
	PSXMEM_Memory.WriteTable[0x1F80]			=	PSXMEM_Memory.ScratchRAM;

	memcpy(PSXMEM_Memory.ReadTable + 0x8000,	PSXMEM_Memory.ReadTable,			0x80 * sizeof(void *));
	memcpy(PSXMEM_Memory.ReadTable + 0xA000,	PSXMEM_Memory.ReadTable,			0x80 * sizeof(void *));
	memcpy(PSXMEM_Memory.OPTable + 0x8000,		PSXMEM_Memory.OPTable,				0x80 * sizeof(void *));
	memcpy(PSXMEM_Memory.OPTable + 0xA000,		PSXMEM_Memory.OPTable,				0x80 * sizeof(void *));
	memcpy(PSXMEM_Memory.WriteTable + 0x8000,	PSXMEM_Memory.WriteTable,			0x80 * sizeof(void *));
	memcpy(PSXMEM_Memory.WriteTable + 0xA000,	PSXMEM_Memory.WriteTable,			0x80 * sizeof(void *));

	//Map BIOS
	for(int i = 0; i != 8; i ++)
	{
		PSXMEM_Memory.ReadTable[i + 0x1FC0]		=	&PSXMEM_Memory.BIOS[i << 16];
		PSXMEM_Memory.OPTable[i + 0x1FC0]		=	&PSXMEM_Memory.BIOSOPS[i << 14];
	}

	memcpy(PSXMEM_Memory.ReadTable + 0x9FC0,	PSXMEM_Memory.ReadTable + 0x1FC0,	8 * sizeof(void *));
	memcpy(PSXMEM_Memory.ReadTable + 0xBFC0,	PSXMEM_Memory.ReadTable + 0x1FC0,	8 * sizeof(void *));
	memcpy(PSXMEM_Memory.OPTable + 0x9FC0,		PSXMEM_Memory.OPTable + 0x1FC0,		8 * sizeof(void *));
	memcpy(PSXMEM_Memory.OPTable + 0xBFC0,		PSXMEM_Memory.OPTable + 0x1FC0,		8 * sizeof(void *));

	return 0;
}

void						psxMemReset					()
{
	FILE *f = NULL;
	char bios[1024];

	//Clear memory
	memset(PSXMEM_Memory.WorkRAM, 0, sizeof(PSXMEM_Memory.WorkRAM));
	memset(PSXMEM_Memory.Parallel, 0, sizeof(PSXMEM_Memory.Parallel));

	// Load BIOS
	//TODO: Neaten
	if(strcmp(Config.Bios, "HLE") != 0)
	{
		sprintf(bios, "%s/%s", Config.BiosDir, Config.Bios);
		f = fopen(bios, "rb");

		if(f == NULL)
		{
			SysMessage(_("Could not open BIOS:\"%s\". Enabling HLE Bios!\n"), bios);
			memset(PSXMEM_Memory.BIOS, 0, 0x80000);
			Config.HLE = TRUE;
		}
		else
		{
			fread(PSXMEM_Memory.BIOS, 1, 0x80000, f);
			fclose(f);
			Config.HLE = FALSE;
		}
	} else Config.HLE = TRUE;
}

void						psxMemShutdown				()
{
	//NOTE: Nothing
}

static int writeok = 1;

//Still needed for psxLWL psxLWR...
uint32_t					psxMemRead32				(uint32_t mem)
{
	psxRegs.cycle += 1;

	if((mem & 0xFFFF0000) == 0x1F800000)
	{
		return (mem < 0x1F801000) ? psxHu32(mem) : psxHwRead32(mem);
	}
	else
	{
		u8* p = (u8*)PSXMEM_Memory.ReadTable[mem >> 16];
		return p ? GETLE32((u32*)(p + (mem & 0xFFFF))) : 0;
	}
}

void						psxMemWrite32				(uint32_t mem, uint32_t value)
{
	psxRegs.cycle += 1;

	if((mem & 0xFFFF0000) == 0x1F800000)
	{
		if(mem < 0x1F801000)
		{
			PUTLE32(psxHu32adr(mem), value);
		}
		else
		{
			psxHwWrite32(mem, value);
		}
	}
	else
	{
		if(mem == 0xFFFE0130)	//Cache flushing
		{
			if((value == 0x800 || value == 0x804) && writeok)
			{
				writeok = 0;
				psxRegs.ICache_valid = 0;

				for(int i = 0; i != 0x80; i ++)
				{
					PSXMEM_Memory.WriteTable[i + 0x0000] = PSXMEM_Memory.FakePage;
					PSXMEM_Memory.WriteTable[i + 0x8000] = PSXMEM_Memory.FakePage;
					PSXMEM_Memory.WriteTable[i + 0xA000] = PSXMEM_Memory.FakePage;
				}
			}
			else if((value == 0 || value == 0x1E988) && !writeok)
			{
				writeok = 1;
				for(int i = 0; i != 0x80; i ++)
				{
					PSXMEM_Memory.WriteTable[i + 0x0000] = &PSXMEM_Memory.WorkRAM[(i & 0x1F) << 16];
					PSXMEM_Memory.WriteTable[i + 0x8000] = &PSXMEM_Memory.WorkRAM[(i & 0x1F) << 16];
					PSXMEM_Memory.WriteTable[i + 0xA000] = &PSXMEM_Memory.WorkRAM[(i & 0x1F) << 16];
				}
			}
		}
		else
		{
			u8* p = (u8*)PSXMEM_Memory.WriteTable[mem >> 16];
			PUTLE32((u32*)(p + (mem & 0xFFFF)), value);
			PSXCPU_Clear((mem & (~3)), 1);
		}
	}
}


