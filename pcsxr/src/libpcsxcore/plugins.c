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
* Plugin library callback/access functions.
*/

#include "plugins.h"

int					PSXPLUG_Load						()
{
	PSXPLUG_Close();

	pkCDRinit();
	pkGPUinit();
	pkSPUinit();
	pkPADinit(1);
	pkPADinit(2);

	pkCDRopen();
	pkGPUopen();
//	pkGPUregisterCallback(GPUbusy);
	pkSPUopen();
	pkSPUregisterCallback(SPUirq);
	pkPADopen(0);


	return 0;
}

void				PSXPLUG_Close						()
{
	pkCDRclose();
	pkSPUclose();
	pkPADclose();
	pkGPUclose();

	pkCDRshutdown();
	pkGPUshutdown();
	pkSPUshutdown();
	pkPADshutdown();
}


