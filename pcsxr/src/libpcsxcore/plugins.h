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

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psxcommon.h"

//#define ENABLE_SIO1API 1

//ROBO: Never win32
//#ifndef _WIN32
#if 1

typedef void* HWND;
#define CALLBACK

typedef long (*NETopen)(unsigned long *);
typedef long (*SIO1open)(unsigned long *);

#else

#include <windows.h>

typedef long (CALLBACK* NETopen)(HWND);
typedef long (CALLBACK* SIO1open)(HWND);

#endif

#include "spu.h"

#include "psemu_plugin_defs.h"
#include "decode_xa.h"

int LoadPlugins();
void ReleasePlugins();
int OpenPlugins();
void ClosePlugins();

typedef unsigned long (CALLBACK* PSEgetLibType)(void);
typedef unsigned long (CALLBACK* PSEgetLibVersion)(void);
typedef char *(CALLBACK* PSEgetLibName)(void);

// GPU Functions
typedef struct {
	uint32_t ulFreezeVersion;
	uint32_t ulStatus;
	uint32_t ulControl[256];
	unsigned char psxVRam[1024*512*2];
} GPUFreeze_t;

// CD-ROM Functions
struct CdrStat {
	uint32_t Type;
	uint32_t Status;
	unsigned char Time[3];
};
struct SubQ {
	char res0[12];
	unsigned char ControlAndADR;
	unsigned char TrackNumber;
	unsigned char IndexNumber;
	unsigned char TrackRelativeAddress[3];
	unsigned char Filler;
	unsigned char AbsoluteAddress[3];
	unsigned char CRC[2];
	char res1[72];
};

// SPU Functions
typedef struct {
	unsigned char PluginName[8];
	uint32_t PluginVersion;
	uint32_t Size;
	unsigned char SPUPorts[0x200];
	unsigned char SPURam[0x80000];
	xa_decode_t xa;
	unsigned char *SPUInfo;
} SPUFreeze_t;

// NET Functions
typedef long (CALLBACK* NETinit)(void);
typedef long (CALLBACK* NETshutdown)(void);
typedef long (CALLBACK* NETclose)(void);
typedef long (CALLBACK* NETconfigure)(void);
typedef long (CALLBACK* NETtest)(void);
typedef void (CALLBACK* NETabout)(void);
typedef void (CALLBACK* NETpause)(void);
typedef void (CALLBACK* NETresume)(void);
typedef long (CALLBACK* NETqueryPlayer)(void);
typedef long (CALLBACK* NETsendData)(void *, int, int);
typedef long (CALLBACK* NETrecvData)(void *, int, int);
typedef long (CALLBACK* NETsendPadData)(void *, int);
typedef long (CALLBACK* NETrecvPadData)(void *, int);

typedef struct {
	char EmuName[32];
	char CdromID[9];	// ie. 'SCPH12345', no \0 trailing character
	char CdromLabel[11];
	void *psxMem;
	char GPUpath[256];	// paths must be absolute
	char SPUpath[256];
	char CDRpath[256];
	char MCD1path[256];
	char MCD2path[256];
	char BIOSpath[256];	// 'HLE' for internal bios
	char Unused[1024];
} netInfo;

typedef long (CALLBACK* NETsetInfo)(netInfo *);
typedef long (CALLBACK* NETkeypressed)(int);

// NET function pointers
extern NETinit               NET_init;
extern NETshutdown           NET_shutdown;
extern NETopen               NET_open;
extern NETclose              NET_close;
extern NETtest               NET_test;
extern NETconfigure          NET_configure;
extern NETabout              NET_about;
extern NETpause              NET_pause;
extern NETresume             NET_resume;
extern NETqueryPlayer        NET_queryPlayer;
extern NETsendData           NET_sendData;
extern NETrecvData           NET_recvData;
extern NETsendPadData        NET_sendPadData;
extern NETrecvPadData        NET_recvPadData;
extern NETsetInfo            NET_setInfo;
extern NETkeypressed         NET_keypressed;

#ifdef ENABLE_SIO1API

// SIO1 Functions (link cable)
typedef long (CALLBACK* SIO1init)(void);
typedef long (CALLBACK* SIO1shutdown)(void);
typedef long (CALLBACK* SIO1close)(void);
typedef long (CALLBACK* SIO1configure)(void);
typedef long (CALLBACK* SIO1test)(void);
typedef void (CALLBACK* SIO1about)(void);
typedef void (CALLBACK* SIO1pause)(void);
typedef void (CALLBACK* SIO1resume)(void);
typedef long (CALLBACK* SIO1keypressed)(int);
typedef void (CALLBACK* SIO1writeData8)(unsigned char);
typedef void (CALLBACK* SIO1writeData16)(unsigned short);
typedef void (CALLBACK* SIO1writeData32)(unsigned long);
typedef void (CALLBACK* SIO1writeStat16)(unsigned short);
typedef void (CALLBACK* SIO1writeStat32)(unsigned long);
typedef void (CALLBACK* SIO1writeMode16)(unsigned short);
typedef void (CALLBACK* SIO1writeMode32)(unsigned long);
typedef void (CALLBACK* SIO1writeCtrl16)(unsigned short);
typedef void (CALLBACK* SIO1writeCtrl32)(unsigned long);
typedef void (CALLBACK* SIO1writeBaud16)(unsigned short);
typedef void (CALLBACK* SIO1writeBaud32)(unsigned long);
typedef unsigned char (CALLBACK* SIO1readData8)(void);
typedef unsigned short (CALLBACK* SIO1readData16)(void);
typedef unsigned long (CALLBACK* SIO1readData32)(void);
typedef unsigned short (CALLBACK* SIO1readStat16)(void);
typedef unsigned long (CALLBACK* SIO1readStat32)(void);
typedef unsigned short (CALLBACK* SIO1readMode16)(void);
typedef unsigned long (CALLBACK* SIO1readMode32)(void);
typedef unsigned short (CALLBACK* SIO1readCtrl16)(void);
typedef unsigned long (CALLBACK* SIO1readCtrl32)(void);
typedef unsigned short (CALLBACK* SIO1readBaud16)(void);
typedef unsigned long (CALLBACK* SIO1readBaud32)(void);
typedef void (CALLBACK* SIO1registerCallback)(void (CALLBACK *callback)(void));

// SIO1 function pointers
extern SIO1init               SIO1_init;
extern SIO1shutdown           SIO1_shutdown;
extern SIO1open               SIO1_open;
extern SIO1close              SIO1_close;
extern SIO1test               SIO1_test;
extern SIO1configure          SIO1_configure;
extern SIO1about              SIO1_about;
extern SIO1pause              SIO1_pause;
extern SIO1resume             SIO1_resume;
extern SIO1keypressed         SIO1_keypressed;
extern SIO1writeData8         SIO1_writeData8;
extern SIO1writeData16        SIO1_writeData16;
extern SIO1writeData32        SIO1_writeData32;
extern SIO1writeStat16        SIO1_writeStat16;
extern SIO1writeStat32        SIO1_writeStat32;
extern SIO1writeMode16        SIO1_writeMode16;
extern SIO1writeMode32        SIO1_writeMode32;
extern SIO1writeCtrl16        SIO1_writeCtrl16;
extern SIO1writeCtrl32        SIO1_writeCtrl32;
extern SIO1writeBaud16        SIO1_writeBaud16;
extern SIO1writeBaud32        SIO1_writeBaud32;
extern SIO1readData8          SIO1_readData8;
extern SIO1readData16         SIO1_readData16;
extern SIO1readData32         SIO1_readData32;
extern SIO1readStat16         SIO1_readStat16;
extern SIO1readStat32         SIO1_readStat32;
extern SIO1readMode16         SIO1_readMode16;
extern SIO1readMode32         SIO1_readMode32;
extern SIO1readCtrl16         SIO1_readCtrl16;
extern SIO1readCtrl32         SIO1_readCtrl32;
extern SIO1readBaud16         SIO1_readBaud16;
extern SIO1readBaud32         SIO1_readBaud32;
extern SIO1registerCallback   SIO1_registerCallback;

#endif

void CALLBACK clearDynarec(void);

void CALLBACK GPUbusy( int ticks );

void SetIsoFile(const char *filename);
const char *GetIsoFile(void);
boolean UsingIso(void);
void SetCdOpenCaseTime(s64 time);

#ifdef __cplusplus
}
#endif
#endif
