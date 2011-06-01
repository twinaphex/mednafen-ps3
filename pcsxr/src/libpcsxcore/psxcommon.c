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

#include "psxcommon.h"
#include "r3000a.h"
#include "psxbios.h"

#include "ppf.h"

PcsxConfig Config;

int EmuInit()
{
	return psxInit();
}

void EmuReset()
{
	psxReset();
}

void EmuShutdown()
{
	FreePPFCache();
	psxShutdown();
}

void EmuUpdate()
{
	// Do not allow hotkeys inside a softcall from HLE BIOS
	if(!Config.HLE || !hleSoftCall)
	{
		SysUpdate();
	}
}

