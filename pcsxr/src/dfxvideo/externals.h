/***************************************************************************
                        externals.h -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#include <stdint.h>

#define INFO_TW        0
#define INFO_DRAWSTART 1
#define INFO_DRAWEND   2
#define INFO_DRAWOFF   3

#define SHADETEXBIT(x) ((x>>24) & 0x1)
#define SEMITRANSBIT(x) ((x>>25) & 0x1)
#define PSXRGB(r,g,b) ((g<<10)|(b<<5)|r)

#define DATAREGISTERMODES unsigned short

#define DR_NORMAL        0
#define DR_VRAMTRANSFER  1


#define GPUSTATUS_ODDLINES            0x80000000
#define GPUSTATUS_DMABITS             0x60000000 // Two bits
#define GPUSTATUS_READYFORCOMMANDS    0x10000000
#define GPUSTATUS_READYFORVRAM        0x08000000
#define GPUSTATUS_IDLE                0x04000000
#define GPUSTATUS_DISPLAYDISABLED     0x00800000
#define GPUSTATUS_INTERLACED          0x00400000
#define GPUSTATUS_RGB24               0x00200000
#define GPUSTATUS_PAL                 0x00100000
#define GPUSTATUS_DOUBLEHEIGHT        0x00080000
#define GPUSTATUS_WIDTHBITS           0x00070000 // Three bits
#define GPUSTATUS_MASKENABLED         0x00001000
#define GPUSTATUS_MASKDRAWN           0x00000800
#define GPUSTATUS_DRAWINGALLOWED      0x00000400
#define GPUSTATUS_DITHER              0x00000200

#define GPUIsBusy (lGPUstatusRet &= ~GPUSTATUS_IDLE)
#define GPUIsIdle (lGPUstatusRet |= GPUSTATUS_IDLE)

#define GPUIsNotReadyForCommands (lGPUstatusRet &= ~GPUSTATUS_READYFORCOMMANDS)
#define GPUIsReadyForCommands (lGPUstatusRet |= GPUSTATUS_READYFORCOMMANDS)


/////////////////////////////////////////////////////////////////////////////

typedef struct VRAMLOADTTAG
{
 short x;
 short y;
 short Width;
 short Height;
 short RowsRemaining;
 short ColsRemaining;
 unsigned short *ImagePtr;
} VRAMLoad_t;

/////////////////////////////////////////////////////////////////////////////

typedef struct PSXPOINTTAG
{
 int32_t x;
 int32_t y;
} PSXPoint_t;

typedef struct PSXSPOINTTAG
{
 short x;
 short y;
} PSXSPoint_t;

typedef struct PSXRECTTAG
{
 short x0;
 short x1;
 short y0;
 short y1;
} PSXRect_t;

#define CALLBACK
#define FALSE 0
#define TRUE 1
#define BOOL unsigned short
#define LOWORD(l)           ((unsigned short)(l))
#define HIWORD(l)           ((unsigned short)(((uint32_t)(l) >> 16) & 0xFFFF))
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define DWORD uint32_t

typedef struct RECTTAG
{
 int left;
 int top;
 int right;
 int bottom;
} RECT;

/////////////////////////////////////////////////////////////////////////////

typedef struct TWINTAG
{
 PSXRect_t  Position;
} TWin_t;

/////////////////////////////////////////////////////////////////////////////

typedef struct PSXDISPLAYTAG
{
 PSXPoint_t  DisplayModeNew;
 PSXPoint_t  DisplayMode;
 PSXPoint_t  DisplayPosition;
 PSXPoint_t  DisplayEnd;
 
 int32_t        Double;
 int32_t        Height;
 int32_t        PAL;
 int32_t        InterlacedNew;
 int32_t        Interlaced;
 int32_t        RGB24New;
 int32_t        RGB24;
 PSXSPoint_t DrawOffset;
 int32_t        Disabled;
 PSXRect_t   Range;

} PSXDisplay_t;

/////////////////////////////////////////////////////////////////////////////

// draw.c


extern int32_t			GlobalTextAddrX,GlobalTextAddrY,GlobalTextTP;
extern int32_t			GlobalTextREST,GlobalTextABR,GlobalTextPAGE;
extern short			ly0,lx0,ly1,lx1,ly2,lx2,ly3,lx3;
extern long				lLowerpart;
extern BOOL				bCheckMask;
extern unsigned long	lSetMask;
extern short			g_m1;
extern short			g_m2;
extern short			g_m3;
extern short			DrawSemiTrans;


extern BOOL				bUsingTWin;
extern TWin_t			TWin;
extern void				(*primTableJ[256])(unsigned char *);
extern unsigned short	usMirror;
extern int				iDither;
extern uint32_t			dwEmuFixes;
extern int				iUseDither;
extern int32_t			drawX;
extern int32_t			drawY;
extern int32_t			drawW;
extern int32_t			drawH;


extern VRAMLoad_t		VRAMWrite;
extern VRAMLoad_t		VRAMRead;
extern DATAREGISTERMODES DataWriteMode;
extern DATAREGISTERMODES DataReadMode;
extern short			sDispWidths[];
extern PSXDisplay_t		PSXDisplay;
extern PSXDisplay_t		PreviousPSXDisplay;
extern long				lGPUstatusRet;
extern unsigned char*	psxVSecure;
extern unsigned char*	psxVub;
extern signed char*		psxVsb;
extern unsigned short*	psxVuw;
extern signed short*	psxVsw;
extern uint32_t*		psxVul;
extern int32_t*			psxVsl;
extern unsigned short*	psxVuw_eom;
extern uint32_t			ulStatusControl[];
extern uint32_t			vBlank;

extern uint32_t			lGPUInfoVals[];
extern BOOL				bCheckMask;
extern unsigned short	sSetMask;
extern unsigned long	lSetMask;

extern int				iGPUHeight;
extern int				iGPUHeightMask;
extern int				GlobalTextIL;

//Checkeroni
extern int				iTileCheat;
extern int				UseFrameSkip;			//Always 0 now
extern uint32_t			dwGPUVersion;			//Always 0 now
extern BOOL				bDoVSyncUpdate;			//Value is never checked
extern uint32_t			dwActFixes;				//Never assigned to
extern uint32_t			dwCfgFixes;				//Never referenced
