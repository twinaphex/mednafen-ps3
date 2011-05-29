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

#ifndef __PSXMEMORY_H__
#define __PSXMEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

//ROBO:
#include <stdint.h>
#define	TOTAL_INLINE __attribute__((always_inline))

static TOTAL_INLINE uint16_t GETLE16(uint16_t *ptr)
{
    uint16_t ret;
	__asm__ ("lhbrx %0, 0, %1" : "=r" (ret) : "r" (ptr));
    return ret;
}

static TOTAL_INLINE uint32_t GETLE32(uint32_t *ptr)
{
    uint32_t ret;
    __asm__ ("lwbrx %0, 0, %1" : "=r" (ret) : "r" (ptr));
    return ret;
}

static TOTAL_INLINE uint32_t GETLE16D(uint32_t *ptr)
{
    uint32_t ret;
    __asm__ ("lwbrx %0, 0, %1\n" "rlwinm %0, %0, 16, 0, 31" : "=r" (ret) : "r" (ptr));
    return ret;
}

static TOTAL_INLINE void PUTLE16(uint16_t *ptr, uint16_t val)
{
    __asm__ ("sthbrx %0, 0, %1" : : "r" (val), "r" (ptr) : "memory");
}

static TOTAL_INLINE void PUTLE32(uint32_t *ptr, uint32_t val)
{
    __asm__ ("stwbrx %0, 0, %1" : : "r" (val), "r" (ptr) : "memory");
}

#include "psxcommon.h"

#if defined(__BIGENDIAN__)

#define _SWAP16(b) ((((unsigned char *)&(b))[0] & 0xff) | (((unsigned char *)&(b))[1] & 0xff) << 8)
#define _SWAP32(b) ((((unsigned char *)&(b))[0] & 0xff) | ((((unsigned char *)&(b))[1] & 0xff) << 8) | ((((unsigned char *)&(b))[2] & 0xff) << 16) | (((unsigned char *)&(b))[3] << 24))

#define SWAP16(v) ((((v) & 0xff00) >> 8) +(((v) & 0xff) << 8))
#define SWAP32(v) ((((v) & 0xff000000ul) >> 24) + (((v) & 0xff0000ul) >> 8) + (((v) & 0xff00ul)<<8) +(((v) & 0xfful) << 24))
#define SWAPu32(v) SWAP32((u32)(v))
#define SWAPs32(v) SWAP32((s32)(v))

#define SWAPu16(v) SWAP16((u16)(v))
#define SWAPs16(v) SWAP16((s16)(v))

#else

#define SWAP16(b) (b)
#define SWAP32(b) (b)

#define SWAPu16(b) (b)
#define SWAPu32(b) (b)

#endif

extern s8 *psxM;
//ROBO: psxMs gone
#define psxMu8(mem)		(*(u8 *)&psxM[(mem) & 0x1fffff])
#define psxMu16(mem)	(GETLE16((u16*)&psxM[(mem) & 0x1fffff]))
#define psxMu32(mem)	(GETLE32((u32*)&psxM[(mem) & 0x1fffff]))
#define psxMu8ref(mem)	(*(u8 *)&psxM[(mem) & 0x1fffff])
#define psxMu16ref(mem)	(*(u16 *)&psxM[(mem) & 0x1fffff])
#define psxMu32ref(mem)	(*(u32 *)&psxM[(mem) & 0x1fffff])

//ROBO: psxPs gone
extern s8 *psxP;
#define psxPu8(mem)		(*(u8 *)&psxP[(mem) & 0xffff])
#define psxPu16(mem)	(GETLE16((u16*)&psxP[(mem) & 0xffff]))
#define psxPu32(mem)	(GETLE32((u32*)&psxP[(mem) & 0xffff]))
#define psxPu8ref(mem)	(*(u8 *)&psxP[(mem) & 0xffff])
#define psxPu16ref(mem)	(*(u16 *)&psxP[(mem) & 0xffff])
#define psxPu32ref(mem)	(*(u32 *)&psxP[(mem) & 0xffff])

//ROBO: psxRs* gone
extern s8 *psxR;
#define psxRu8(mem)		(*(u8* )&psxR[(mem) & 0x7ffff])
#define psxRu16(mem)	(GETLE16((u16*)&psxR[(mem) & 0x7ffff]))
#define psxRu32(mem)	(GETLE32((u32*)&psxR[(mem) & 0x7ffff]))
#define psxRu8ref(mem)	(*(u8 *)&psxR[(mem) & 0x7ffff])
#define psxRu16ref(mem)	(*(u16*)&psxR[(mem) & 0x7ffff])
#define psxRu32ref(mem)	(*(u32*)&psxR[(mem) & 0x7ffff])

//ROBO: psxHs* gone
extern s8 *psxH;
#define psxHu8(mem)		(*(u8 *)&psxH[(mem) & 0xffff])
#define psxHu16(mem)	(GETLE16((u16 *)&psxH[(mem) & 0xffff]))
#define psxHu32(mem)	(GETLE32((u32 *)&psxH[(mem) & 0xffff]))
#define psxHu8ref(mem)	(*(u8 *)&psxH[(mem) & 0xffff])
#define psxHu16ref(mem)	(*(u16 *)&psxH[(mem) & 0xffff])
#define psxHu32ref(mem)	(*(u32 *)&psxH[(mem) & 0xffff])
#define psxHu8adr(mem)	((u8 *)&psxH[(mem) & 0xffff])
#define psxHu16adr(mem)	((u16 *)&psxH[(mem) & 0xffff])
#define psxHu32adr(mem)	((u32 *)&psxH[(mem) & 0xffff])



extern u8 **psxMemWLUT;
extern u8 **psxMemRLUT;

//ROBO: PSXMs* gone
#define PSXM(mem)		(psxMemRLUT[(mem) >> 16] == 0 ? NULL : (u8*)(psxMemRLUT[(mem) >> 16] + ((mem) & 0xffff)))
#define PSXMu8(mem)		(*(u8 *)PSXM(mem))
#define PSXMu16(mem)	(GETLE16((u16 *)PSXM(mem)))
#define PSXMu32(mem)	(GETLE32(*(u32 *)PSXM(mem)))
#define PSXMu32ref(mem)	(*(u32 *)PSXM(mem))

//ROBO
#ifdef STUPID_SPEED_TEST
#define PSXMfast(mem)	((u8*)(psxMemRLUT[(mem) >> 16] + ((mem) & 0xffff)))
#endif

int psxMemInit();
void psxMemReset();
void psxMemShutdown();

u8 psxMemRead8 (u32 mem);
u16 psxMemRead16(u32 mem);
u32 psxMemRead32(u32 mem);
void psxMemWrite8 (u32 mem, u8 value);
void psxMemWrite16(u32 mem, u16 value);
void psxMemWrite32(u32 mem, u32 value);
void *psxMemPointer(u32 mem);

#ifdef __cplusplus
}
#endif
#endif

