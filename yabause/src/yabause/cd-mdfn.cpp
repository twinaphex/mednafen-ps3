/*  Copyright 2004-2008 Theo Berkau
    Copyright 2005 Joost Peters
    Copyright 2005-2006 Guillaume Duhamel
    
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

#include "src/mednafen.h"
#include "src/cdrom/cdromif.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

extern "C"
{
	#include "cdbase.h"
	#include "error.h"
	#include "debug.h"
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
{
static int MDFNCDInit(const char *);
static void MDFNCDDeInit();
static int MDFNCDGetStatus();
static s32 MDFNCDReadTOC(u32 *);
static int MDFNCDReadSectorFAD(u32, void *);
static void MDFNCDReadAheadFAD(u32);


CDInterface ArchCD = {
CDCORE_ARCH,
"Mednafen CD Interface",
MDFNCDInit,
MDFNCDDeInit,
MDFNCDGetStatus,
MDFNCDReadTOC,
MDFNCDReadSectorFAD,
MDFNCDReadAheadFAD
};

//////////////////////////////////////////////////////////////////////////////
// Mednafen Interface
//////////////////////////////////////////////////////////////////////////////

static int MDFNCDInit(UNUSED const char *cdrom_name)
{
	// Initialization function. cdrom_name can be whatever you want it to be.
	// Obviously with some ports(e.g. the dreamcast port) you probably won't
	// even use it.
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

static void MDFNCDDeInit()
{
	// Cleanup function. Enough said.
	return;
}

//////////////////////////////////////////////////////////////////////////////

static int MDFNCDGetStatus()
{
	// This function is called periodically to see what the status of the
	// drive is.
	//
	// Should return one of the following values:
	// 0 - CD Present, disc spinning
	// 1 - CD Present, disc not spinning
	// 2 - CD not present
	// 3 - Tray open
	//
	// If you really don't want to bother too much with this function, just
	// return status 0. Though it is kind of nice when the bios's cd player,
	// etc. recognizes when you've ejected the tray and popped in another disc.

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

static s32 MDFNCDReadTOC(u32 *TOC)
{
	// The format of TOC is as follows:
	// TOC[0] - TOC[98] are meant for tracks 1-99. Each entry has the following
	// format:
	// bits 0 - 23: track FAD address
	// bits 24 - 27: track addr
	// bits 28 - 31: track ctrl
	//
	// Any Unused tracks should be set to 0xFFFFFFFF
	//
	// TOC[99] - Point A0 information 
	// Uses the following format:
	// bits 0 - 7: PFRAME(should always be 0)
	// bits 7 - 15: PSEC(Program area format: 0x00 - CDDA or CDROM, 0x10 - CDI, 0x20 - CDROM-XA)
	// bits 16 - 23: PMIN(first track's number)
	// bits 24 - 27: first track's addr
	// bits 28 - 31: first track's ctrl
	//
	// TOC[100] - Point A1 information
	// Uses the following format:
	// bits 0 - 7: PFRAME(should always be 0)
	// bits 7 - 15: PSEC(should always be 0)
	// bits 16 - 23: PMIN(last track's number)
	// bits 24 - 27: last track's addr
	// bits 28 - 31: last track's ctrl
	//
	// TOC[101] - Point A2 information
	// Uses the following format:
	// bits 0 - 23: leadout FAD address
	// bits 24 - 27: leadout's addr
	// bits 28 - 31: leadout's ctrl
	//
	// Special Note: To convert from LBA/LSN to FAD, add 150.

	CD_TOC toc;
	CDIF_ReadTOC(&toc);

	int first_track = -1, last_track = -1;

	for(int i = 1; i != 100; i ++)
	{
		if(i < toc.first_track && i > toc.last_track)
		{
			TOC[i] = 0xFFFFFFFF;
		}
		else
		{
			first_track == (first_track == -1) ? i : first_track;
			last_track = i;

			TOC[i] = ((toc.tracks[i].adr & 0xF) << 24);
			TOC[i] |= ((toc.tracks[i].control & 0xF) << 28);
			TOC[i] |= ((toc.tracks[i].lba & 0xFFFFFF) + 150);
		}
	}

	TOC[99] = (toc.disc_type << 8) | (first_track << 16) | ((toc.tracks[first_track].adr & 0xF) << 24) | ((toc.tracks[first_track].control & 0xF) << 28);
	TOC[100] = (last_track << 16) | ((toc.tracks[last_track].adr & 0xF) << 24) | ((toc.tracks[last_track].control & 0xF) << 28);
	TOC[101] = ((toc.tracks[100].adr & 0xF) << 24) | ((toc.tracks[100].control & 0xF) << 28) | ((toc.tracks[100].lba & 0xFFFFFF) + 150);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

static int MDFNCDReadSectorFAD(u32 FAD, void * buffer)
{
	// This function is supposed to read exactly 1 -RAW- 2352-byte sector at
	// the specified FAD address to buffer. Should return true if successful,
	// false if there was an error.
	//
	// Special Note: To convert from FAD to LBA/LSN, minus 150.
	//
	// The whole process needed to be changed since I need more control over
	// sector detection, etc. Not to mention it means less work for the porter
	// since they only have to implement raw sector reading as opposed to
	// implementing mode 1, mode 2 form1/form2, -and- raw sector reading.

	CDIF_ReadRawSector((uint8*)buffer, FAD - 150);
	return 1;
}

static void MDFNCDReadAheadFAD(u32 FAD)
{
	// This function is called to tell the driver which sector (FAD
	// address) is expected to be read next. If the driver supports
	// read-ahead, it should start reading the given sector in the
	// background while the emulation continues, so that when the
	// sector is actually read with ReadSectorFAD() it'll be available
	// immediately. (Note that there's no guarantee this sector will
	// actually be requested--the emulated CD might be stopped before
	// the sector is read, for example.)
	//
	// This function should NOT block. If the driver can't perform
	// asynchronous reads (or you just don't want to bother handling
	// them), make this function a no-op and just read sectors
	// normally.
}
}
#endif

