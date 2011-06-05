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
* Functions for PSX hardware control.
*/

#include "psxhw.h"
#include "mdec.h"
#include "cdrom.h"
#include "gpu.h"

void psxHwReset() {
	if (Config.Sio) psxHu32ref(0x1070) |= SWAP32(0x80);
	if (Config.SpuIrq) psxHu32ref(0x1070) |= SWAP32(0x200);

	memset(PSXMEM_Memory.ScratchRAM, 0, 0x10000);

	mdecInit(); // initialize mdec decoder
	cdrReset();
	psxRcntInit();
}

#define	READ_REGISTER8(a,x)	{REGR8_##a:{return x;}}
#define	READ_REGISTER16(a,x){REGR16_##a:{return x;}}
#define	READ_REGISTER32(a,x){REGR32_##a:{return x;}}
#define	BEGIN_REGISTERR(a)	{REGR32_##a:{
#define	BEGIN_REGISTER8(a)	{REG8_##a:{
#define	BEGIN_REGISTER16(a)	{REG16_##a:{
#define	BEGIN_REGISTER(a)	{REG32_##a:{
#define END_REGISTER		return;}}

uint8_t					psxHwRead8				(uint32_t aAddress)
{

	static const void* const DispatchTable[0x1000] = 
	{
		#include "dispatch/HwRead8.inc"
	};

	goto *DispatchTable[(aAddress & 0xFFF)];

	READ_REGISTER8(040, sioRead8())
	READ_REGISTER8(800, cdrRead0())
	READ_REGISTER8(801, cdrRead1())
	READ_REGISTER8(802, cdrRead2())
	READ_REGISTER8(803, cdrRead3())
	READ_REGISTER8(UNK, psxHu8(aAddress))
}

uint16_t				psxHwRead16				(uint32_t aAddress)
{
	static const void* const DispatchTable[0x800] = 
	{
		#include "dispatch/HwRead16.inc"
	};

	goto *DispatchTable[(aAddress & 0xFFF) / 2];

	READ_REGISTER16(040, sioRead8() | (sioRead8() << 8))
	READ_REGISTER16(044, PSXSIO_ReadStatus())
	READ_REGISTER16(048, PSXSIO_ReadMode())
	READ_REGISTER16(04A, PSXSIO_ReadControl())
	READ_REGISTER16(04E, PSXSIO_ReadBaud())
	READ_REGISTER16(100, psxRcntRcount(0))
	READ_REGISTER16(104, psxRcntRmode(0))
	READ_REGISTER16(108, psxRcntRtarget(0))
	READ_REGISTER16(110, psxRcntRcount(1))
	READ_REGISTER16(114, psxRcntRmode(1))
	READ_REGISTER16(118, psxRcntRtarget(1))
	READ_REGISTER16(120, psxRcntRcount(2))
	READ_REGISTER16(124, psxRcntRmode(2))
	READ_REGISTER16(128, psxRcntRtarget(2));
	READ_REGISTER16(SPU, pkSPUreadRegister(aAddress))
	READ_REGISTER16(UNK, psxHu16(aAddress))
}

uint32_t				psxHwRead32				(uint32_t aAddress)
{
	static const void* const DispatchTable[0x400] = 
	{
		#include "dispatch/HwRead32.inc"
	};

	goto *DispatchTable[(aAddress & 0xFFF) / 4];

	READ_REGISTER32(040, sioRead8() | (sioRead8() << 8) | (sioRead8() << 16) | (sioRead8() << 24))
	READ_REGISTER32(810, pkGPUreadData())
	READ_REGISTER32(814, gpuReadStatus())
	READ_REGISTER32(820, mdecRead0())
	READ_REGISTER32(824, mdecRead1())
	READ_REGISTER32(100, psxRcntRcount(0))
	READ_REGISTER32(104, psxRcntRmode(0))
	READ_REGISTER32(108, psxRcntRtarget(0))
	READ_REGISTER32(110, psxRcntRcount(1))
	READ_REGISTER32(114, psxRcntRmode(1))
	READ_REGISTER32(118, psxRcntRtarget(1))
	READ_REGISTER32(120, psxRcntRcount(2))
	READ_REGISTER32(124, psxRcntRmode(2))
	READ_REGISTER32(128, psxRcntRtarget(2))
	READ_REGISTER32(UNK, psxHu32(aAddress))
}

void					psxHwWrite8				(uint32_t aAddress, uint8_t aValue)
{
	static const void* const DispatchTable[0x1000] = 
	{
		#include "dispatch/HwWrite8.inc"
	};

	goto *DispatchTable[(aAddress & 0xFFF)];


	BEGIN_REGISTER8(040)	sioWrite8(aValue);				END_REGISTER
	BEGIN_REGISTER8(800)	cdrWrite0(aValue);				END_REGISTER
	BEGIN_REGISTER8(801)	cdrWrite1(aValue); 				END_REGISTER
	BEGIN_REGISTER8(802)	cdrWrite2(aValue);				END_REGISTER
	BEGIN_REGISTER8(803)	cdrWrite3(aValue); 				END_REGISTER
	BEGIN_REGISTER8(UNK)	psxHu8ref(aAddress) = aValue;	END_REGISTER
}

void					psxHwWrite16			(uint32_t aAddress, uint16_t aValue)
{
	static const void* const DispatchTable[0x800] = 
	{
		#include "dispatch/HwWrite16.inc"
	};

	goto *DispatchTable[(aAddress & 0xFFF) / 2];

	BEGIN_REGISTER16(040)	sioWrite8((unsigned char)aValue); sioWrite8((unsigned char)(aValue>>8)); 	END_REGISTER
	BEGIN_REGISTER16(044)	PSXSIO_WriteStatus(aValue);													END_REGISTER
	BEGIN_REGISTER16(048)	PSXSIO_WriteMode(aValue);													END_REGISTER
	BEGIN_REGISTER16(04A)	PSXSIO_WriteControl(aValue);												END_REGISTER
	BEGIN_REGISTER16(04E)	PSXSIO_WriteBaud(aValue);													END_REGISTER
	BEGIN_REGISTER16(070)
			if (Config.Sio) psxHu16ref(0x1070) |= SWAPu16(0x80);
			if (Config.SpuIrq) psxHu16ref(0x1070) |= SWAPu16(0x200);
			psxHu16ref(0x1070) &= SWAPu16((psxHu16(0x1074) & aValue));
			return;
	END_REGISTER

	BEGIN_REGISTER16(074)	psxHu16ref(0x1074) = SWAPu16(aValue);										END_REGISTER
	BEGIN_REGISTER16(100)	psxRcntWcount(0, aValue);													END_REGISTER
	BEGIN_REGISTER16(104)	psxRcntWmode(0, aValue);													END_REGISTER
	BEGIN_REGISTER16(108)	psxRcntWtarget(0, aValue);													END_REGISTER
	BEGIN_REGISTER16(110)	psxRcntWcount(1, aValue);													END_REGISTER
	BEGIN_REGISTER16(114)	psxRcntWmode(1, aValue);													END_REGISTER
	BEGIN_REGISTER16(118)	psxRcntWtarget(1, aValue);													END_REGISTER
	BEGIN_REGISTER16(120)	psxRcntWcount(2, aValue);													END_REGISTER
	BEGIN_REGISTER16(124)	psxRcntWmode(2, aValue);													END_REGISTER
	BEGIN_REGISTER16(128)	psxRcntWtarget(2, aValue);													END_REGISTER

	BEGIN_REGISTER16(SPU)	pkSPUwriteRegister(aAddress, aValue);										END_REGISTER
	BEGIN_REGISTER16(UNK)	psxHu16ref(aAddress) = SWAPu16(aValue);										END_REGISTER
}

#define DmaExec(n) { \
	HW_DMA##n##_CHCR = SWAPu32(aValue); \
\
	if (SWAPu32(HW_DMA##n##_CHCR) & 0x01000000 && SWAPu32(HW_DMA_PCR) & (8 << (n * 4))) { \
		psxDma##n(SWAPu32(HW_DMA##n##_MADR), SWAPu32(HW_DMA##n##_BCR), SWAPu32(HW_DMA##n##_CHCR)); \
	} \
}

void				psxHwWrite32				(uint32_t aAddress, uint32_t aValue)
{
	static const void* const DispatchTable[0x400] = 
	{
		#include "dispatch/HwWrite32.inc"
	};

	goto *DispatchTable[(aAddress & 0xFFF) / 4];

	BEGIN_REGISTER(074)	psxHu32ref(0x1074) = SWAPu32(aValue);	END_REGISTER
	BEGIN_REGISTER(088)	DmaExec(0);								END_REGISTER
	BEGIN_REGISTER(098)	DmaExec(1);								END_REGISTER
	BEGIN_REGISTER(0A8)	DmaExec(2);								END_REGISTER
	BEGIN_REGISTER(0B8)	DmaExec(3);								END_REGISTER
	BEGIN_REGISTER(0C8)	DmaExec(4);								END_REGISTER
	BEGIN_REGISTER(0E8)	DmaExec(6);								END_REGISTER
	BEGIN_REGISTER(810)	pkGPUwriteData(aValue);					END_REGISTER
	BEGIN_REGISTER(814)	pkGPUwriteStatus(aValue);				END_REGISTER
	BEGIN_REGISTER(820)	mdecWrite0(aValue);						END_REGISTER
	BEGIN_REGISTER(824)	mdecWrite1(aValue);						END_REGISTER
	BEGIN_REGISTER(100)	psxRcntWcount(0, aValue & 0xffff);		END_REGISTER
	BEGIN_REGISTER(104)	psxRcntWmode(0, aValue);				END_REGISTER
	BEGIN_REGISTER(108)	psxRcntWtarget(0, aValue & 0xffff);		END_REGISTER
	BEGIN_REGISTER(110)	psxRcntWcount(1, aValue & 0xffff);		END_REGISTER
	BEGIN_REGISTER(114)	psxRcntWmode(1, aValue);				END_REGISTER
	BEGIN_REGISTER(118)	psxRcntWtarget(1, aValue & 0xffff);		END_REGISTER
	BEGIN_REGISTER(120)	psxRcntWcount(2, aValue & 0xffff);		END_REGISTER
	BEGIN_REGISTER(124)	psxRcntWmode(2, aValue);				END_REGISTER
	BEGIN_REGISTER(128)	psxRcntWtarget(2, aValue & 0xffff);		END_REGISTER

	BEGIN_REGISTER(040)
	    sioWrite8((unsigned char)aValue);
		sioWrite8((unsigned char)((aValue&0xff) >>  8));
		sioWrite8((unsigned char)((aValue&0xff) >> 16));
		sioWrite8((unsigned char)((aValue&0xff) >> 24));
	END_REGISTER

	BEGIN_REGISTER(070)
		if (Config.Sio) psxHu32ref(0x1070) |= SWAPu32(0x80);
		if (Config.SpuIrq) psxHu32ref(0x1070) |= SWAPu32(0x200);
		psxHu32ref(0x1070) &= SWAPu32((psxHu32(0x1074) & aValue));
	END_REGISTER

	BEGIN_REGISTER(0F4)
		u32 tmp = (~aValue) & SWAPu32(HW_DMA_ICR);
		HW_DMA_ICR = SWAPu32(((tmp ^ aValue) & 0xffffff) ^ tmp);
	END_REGISTER

	BEGIN_REGISTER(SPU)
		pkSPUwriteRegister(aAddress, aValue&0xffff);
		aAddress += 2;
		aValue >>= 16;

		if(aAddress>=0x1f801c00 && aAddress<0x1f801e00)
			pkSPUwriteRegister(aAddress, aValue&0xffff);
	END_REGISTER

	BEGIN_REGISTER(UNK)
		psxHu32ref(aAddress) = SWAPu32(aValue); return;
	END_REGISTER
}

int psxHwFreeze(gzFile f, int Mode) {
	return 0;
}


