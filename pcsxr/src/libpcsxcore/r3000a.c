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
* R3000A CPU functions.
*/

#include "r3000a.h"
#include "cdrom.h"
#include "mdec.h"
#include "gpu.h"
#include "gte.h"

R3000Acpu *psxCpu = NULL;
psxRegisters psxRegs;

int psxInit()
{
	psxCpu = &psxInt;
	return psxMemInit() ? psxCpu->Init() : -1;
}

void psxReset()
{
	psxCpu->Reset();
	psxMemReset();

	memset(&psxRegs, 0, sizeof(psxRegs));

	psxRegs.pc = 0xbfc00000; // Start in bootstrap

	psxRegs.CP0.r[12] = 0x10900000; // COP0 enabled | BEV = 1 | TS = 1
	psxRegs.CP0.r[15] = 0x00000002; // PRevID = Revision ID, same as R3000A

	psxHwReset();
	psxBiosInit();

	if(!Config.HLE)
	{
		psxExecuteBios();
	}
}

void psxShutdown()
{
	psxMemShutdown();
	psxBiosShutdown();
	psxCpu->Shutdown();
}

void psxException(u32 code, u32 bd)
{
	// Set the Cause
	psxRegs.CP0.n.Cause = code;

	// Set the EPC & PC
	if (bd)
	{
		psxRegs.CP0.n.Cause |= 0x80000000;
		psxRegs.CP0.n.EPC = (psxRegs.pc - 4);
	}
	else
	{
		psxRegs.CP0.n.EPC = (psxRegs.pc);
	}

	if (psxRegs.CP0.n.Status & 0x400000)
	{
		psxRegs.pc = 0xbfc00180;
	}
	else
	{
		psxRegs.pc = 0x80000080;
	}

	psxRegs.CP0.n.Status = (psxRegs.CP0.n.Status &~0x3f) | ((psxRegs.CP0.n.Status & 0xf) << 2);

//	if (Config.HLE) psxBiosException();
}

u64 pizza, tacos;
u64 pasta[64];
void psxBranchTestt();
#include <sys/time_util.h>

//1109586773 / 201937692 = 5.494698696	: Bail Early
//1117988641 / 208921251 = 5.35124424	: No Early Bail
//1304846812 - 273684845 = 4.767698453	: No Early Bail, Comment out unused interrupts (I wish)
//1524149698 / 309169066 = 4.929825994	: All intterupts, no early bail, all inner conditions marked unlikely

/*void psxBranchTest()
{
 u64 en, ex;
 SYS_TIMEBASE_GET(en);
 psxBranchTestt();
 SYS_TIMEBASE_GET(ex);
 tacos ++;
 pizza += ex - en;
}*/


void psxBranchTest()
{
	//Running tales of destiny until you get drug into the first room with the captain
	//Many different iterations were used so the numbers won't match exactly
	//I need to try this again with other games to get a better idea

	// GameShark Sampler: Give VSync pin some delay before exception eats it
	//Taken: 16746366 (16 million) - Pass: 415816281 (415 million)
	if(UNLIKELY((psxHu32(0x1070) & psxHu32(0x1074)) != 0))
	{
		if((psxRegs.CP0.n.Status & 0x401) == 0x401)
		{
			u32 opcode;

			// Crash Bandicoot 2: Don't run exceptions when GTE in pipeline
			opcode = GETLE32(Read_ICache(psxRegs.pc, TRUE));
			if( ((opcode >> 24) & 0xfe) != 0x4a )
			{
				psxException(0x400, 0);
			}
		}
	}

	//Taken: 1697770 (1.6 million) - Pass: 408958146 (408 million)
	if(UNLIKELY((psxRegs.cycle - psxNextsCounter) >= psxNextCounter))
	{
		psxRcntUpdate();
	}

	//Taken: 68501038 (68 million) Passed: 349830640 (340 million)
	//PSXINT_CDREAD:		44062809 (44 million)
	//PSXINT_SIO:			10885594 (10 million)
	//PSXINT_CDR:			10397298 (10 million)
	//PSXINT_GPUDMA:		1892750  (1.8 million)
	//PSXINT_CDRLID: 		945890   (945 thousand)
	//PSXINT_CDRDMA: 		112581   (112 thousand)
	//PSXINT_MDECOUTDMA:	44444	 (44 thousand)
	//PSXINT_SPUDMA: 		4997	 (4 thousand)
	//PSXINT_MDECINDMA: 	4		 (4)
	//PSXINT_GPUBUSY: 		0
	//PSXINT_GPUOTCDMA: 	0
	//10: 					0
	//PSXINT_CDRDBUF: 		0
	//PSXINT_CDRPLAY: 		0

	//Tested: 595364289 (595 million)
	//NAME    TAKEN     PASSED
	//CDREAD: 2108		61900631
	//SIO:	  115463    16142379
	//CDR:    353       10626853
	//GPUDMA: 6         18786
	//CDLID   7         2808797
	//CDRDMA  5         945885
	//MDECOUT 2245      138318
	//SPUDMA  855       2002018
	//GPUBUSY 2         4995
    //TT6     0			0
	//TT7	  2		    2


	if(UNLIKELY(psxRegs.interrupt != 0))
	{
		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_CDREAD)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDREAD].sCycle) >= psxRegs.intCycle[PSXINT_CDREAD].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDREAD);
				cdrReadInterrupt();
			}
		}

		if((psxRegs.interrupt & (1 << PSXINT_SIO)) && !Config.Sio)
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_SIO].sCycle) >= psxRegs.intCycle[PSXINT_SIO].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_SIO);
				sioInterrupt();
			}
		}

		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_CDR)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDR].sCycle) >= psxRegs.intCycle[PSXINT_CDR].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDR);
				cdrInterrupt();
			}
		}

		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_GPUDMA)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_GPUDMA].sCycle) >= psxRegs.intCycle[PSXINT_GPUDMA].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_GPUDMA);
				gpuInterrupt();
			}
		}

		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_CDRLID)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRLID].sCycle) >= psxRegs.intCycle[PSXINT_CDRLID].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRLID);
				cdrLidSeekInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_CDRDMA)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRDMA].sCycle) >= psxRegs.intCycle[PSXINT_CDRDMA].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRDMA);
				cdrDmaInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_MDECOUTDMA)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_MDECOUTDMA].sCycle) >= psxRegs.intCycle[PSXINT_MDECOUTDMA].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_MDECOUTDMA);
				mdec1Interrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_SPUDMA)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_SPUDMA].sCycle) >= psxRegs.intCycle[PSXINT_SPUDMA].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_SPUDMA);
				spuInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_GPUBUSY)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_GPUBUSY].sCycle) >= psxRegs.intCycle[PSXINT_GPUBUSY].cycle))
			{
		        psxRegs.interrupt &= ~(1 << PSXINT_GPUBUSY);
        		GPU_idle();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_MDECINDMA)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_MDECINDMA].sCycle) >= psxRegs.intCycle[PSXINT_MDECINDMA].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_MDECINDMA);
				mdec0Interrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_GPUOTCDMA)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_GPUOTCDMA].sCycle) >= psxRegs.intCycle[PSXINT_GPUOTCDMA].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_GPUOTCDMA);
				gpuotcInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_CDRPLAY)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRPLAY].sCycle) >= psxRegs.intCycle[PSXINT_CDRPLAY].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRPLAY);
				cdrPlayInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_CDRDBUF)) != 0))
		{
			if(UNLIKELY((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRDBUF].sCycle) >= psxRegs.intCycle[PSXINT_CDRDBUF].cycle))
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRDBUF);
				cdrDecodedBufferInterrupt();
			}
		}
	}
}

void psxJumpTest()
{
	if (!Config.HLE && Config.PsxOut)
	{
		u32 call = psxRegs.GPR.n.t1 & 0xff;
		switch (psxRegs.pc & 0x1fffff)
		{
			case 0xa0:
				if (biosA0[call])
					biosA0[call]();
				break;
			case 0xb0:
				if (biosB0[call])
					biosB0[call]();
				break;
			case 0xc0:
				if (biosC0[call])
					biosC0[call]();
				break;
		}
	}
}

void psxExecuteBios()
{
	while (psxRegs.pc != 0x80030000)
	{
		psxCpu->ExecuteBlock();
	}
}

