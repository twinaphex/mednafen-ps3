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

int psxInit() {
	SysPrintf(_("Running PCSX Version %s (%s).\n"), PACKAGE_VERSION, __DATE__);

	psxCpu = &psxInt;

	Log = 0;

	if (psxMemInit() == -1) return -1;

	return psxCpu->Init();
}

void psxReset() {
	psxCpu->Reset();

	psxMemReset();

	memset(&psxRegs, 0, sizeof(psxRegs));

	psxRegs.pc = 0xbfc00000; // Start in bootstrap

	psxRegs.CP0.r[12] = 0x10900000; // COP0 enabled | BEV = 1 | TS = 1
	psxRegs.CP0.r[15] = 0x00000002; // PRevID = Revision ID, same as R3000A

	psxHwReset();
	psxBiosInit();

	if (!Config.HLE)
		psxExecuteBios();

#ifdef EMU_LOG
	EMU_LOG("*BIOS END*\n");
#endif
	Log = 0;
}

void psxShutdown() {
	psxMemShutdown();
	psxBiosShutdown();

	psxCpu->Shutdown();
}

void psxException(u32 code, u32 bd) {
	// Set the Cause
	psxRegs.CP0.n.Cause = code;

	// Set the EPC & PC
	if (bd) {
#ifdef PSXCPU_LOG
		PSXCPU_LOG("bd set!!!\n");
#endif
		SysPrintf("bd set!!!\n");
		psxRegs.CP0.n.Cause |= 0x80000000;
		psxRegs.CP0.n.EPC = (psxRegs.pc - 4);
	} else
		psxRegs.CP0.n.EPC = (psxRegs.pc);

	if (psxRegs.CP0.n.Status & 0x400000)
		psxRegs.pc = 0xbfc00180;
	else
		psxRegs.pc = 0x80000080;

	// Set the Status
	psxRegs.CP0.n.Status = (psxRegs.CP0.n.Status &~0x3f) |
						  ((psxRegs.CP0.n.Status & 0xf) << 2);

	if (Config.HLE) psxBiosException();
}

void psxBranchTest() {
	// GameShark Sampler: Give VSync pin some delay before exception eats it
	if (psxHu32(0x1070) & psxHu32(0x1074)) {
		if ((psxRegs.CP0.n.Status & 0x401) == 0x401) {
			u32 opcode;

			// Crash Bandicoot 2: Don't run exceptions when GTE in pipeline
			opcode = GETLE32(Read_ICache(psxRegs.pc, TRUE));
			if( ((opcode >> 24) & 0xfe) != 0x4a ) {
#ifdef PSXCPU_LOG
				PSXCPU_LOG("Interrupt: %x %x\n", psxHu32(0x1070), psxHu32(0x1074));
#endif
				psxException(0x400, 0);
			}
		}
	}

	if ((psxRegs.cycle - psxNextsCounter) >= psxNextCounter)
		psxRcntUpdate();

	//Running tales of destiny until you get drug into the first room the count is
	//I need to try this again with other games to get a better idea
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


	if (UNLIKELY(psxRegs.interrupt))
	{
		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_CDREAD)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDREAD].sCycle) >= psxRegs.intCycle[PSXINT_CDREAD].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDREAD);
				cdrReadInterrupt();
			}
		}

		if((psxRegs.interrupt & (1 << PSXINT_SIO)) && !Config.Sio)
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_SIO].sCycle) >= psxRegs.intCycle[PSXINT_SIO].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_SIO);
				sioInterrupt();
			}
		}

		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_CDR)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDR].sCycle) >= psxRegs.intCycle[PSXINT_CDR].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDR);
				cdrInterrupt();
			}
		}


		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_GPUDMA)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_GPUDMA].sCycle) >= psxRegs.intCycle[PSXINT_GPUDMA].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_GPUDMA);
				gpuInterrupt();
			}
		}

		if(LIKELY((psxRegs.interrupt & (1 << PSXINT_CDRLID)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRLID].sCycle) >= psxRegs.intCycle[PSXINT_CDRLID].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRLID);
				cdrLidSeekInterrupt();
			}
		}

		//Try to bail now
		if(LIKELY(psxRegs.interrupt == 0))
		{
			return;
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_MDECOUTDMA)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_MDECOUTDMA].sCycle) >= psxRegs.intCycle[PSXINT_MDECOUTDMA].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_MDECOUTDMA);
				mdec1Interrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_SPUDMA)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_SPUDMA].sCycle) >= psxRegs.intCycle[PSXINT_SPUDMA].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_SPUDMA);
				spuInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_GPUBUSY)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_GPUBUSY].sCycle) >= psxRegs.intCycle[PSXINT_GPUBUSY].cycle)
			{
		        psxRegs.interrupt &= ~(1 << PSXINT_GPUBUSY);
        		GPU_idle();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_MDECINDMA)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_MDECINDMA].sCycle) >= psxRegs.intCycle[PSXINT_MDECINDMA].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_MDECINDMA);
				mdec0Interrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_GPUOTCDMA)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_GPUOTCDMA].sCycle) >= psxRegs.intCycle[PSXINT_GPUOTCDMA].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_GPUOTCDMA);
				gpuotcInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_CDRDMA)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRDMA].sCycle) >= psxRegs.intCycle[PSXINT_CDRDMA].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRDMA);
				cdrDmaInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_CDRPLAY)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRPLAY].sCycle) >= psxRegs.intCycle[PSXINT_CDRPLAY].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRPLAY);
				cdrPlayInterrupt();
			}
		}

		if(UNLIKELY((psxRegs.interrupt & (1 << PSXINT_CDRDBUF)) != 0))
		{
			if ((psxRegs.cycle - psxRegs.intCycle[PSXINT_CDRDBUF].sCycle) >= psxRegs.intCycle[PSXINT_CDRDBUF].cycle)
			{
				psxRegs.interrupt &= ~(1 << PSXINT_CDRDBUF);
				cdrDecodedBufferInterrupt();
			}
		}
	}
}

void psxJumpTest() {
	if (!Config.HLE && Config.PsxOut) {
		u32 call = psxRegs.GPR.n.t1 & 0xff;
		switch (psxRegs.pc & 0x1fffff) {
			case 0xa0:
#ifdef PSXBIOS_LOG
				if (call != 0x28 && call != 0xe) {
					PSXBIOS_LOG("Bios call a0: %s (%x) %x,%x,%x,%x\n", biosA0n[call], call, psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3); }
#endif
				if (biosA0[call])
					biosA0[call]();
				break;
			case 0xb0:
#ifdef PSXBIOS_LOG
				if (call != 0x17 && call != 0xb) {
					PSXBIOS_LOG("Bios call b0: %s (%x) %x,%x,%x,%x\n", biosB0n[call], call, psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3); }
#endif
				if (biosB0[call])
					biosB0[call]();
				break;
			case 0xc0:
#ifdef PSXBIOS_LOG
				PSXBIOS_LOG("Bios call c0: %s (%x) %x,%x,%x,%x\n", biosC0n[call], call, psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3);
#endif
				if (biosC0[call])
					biosC0[call]();
				break;
		}
	}
}

void psxExecuteBios() {
	while (psxRegs.pc != 0x80030000)
		psxCpu->ExecuteBlock();
}

