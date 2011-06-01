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
//ROBO: Not using cdriso
//#include "cdriso.h"

NETinit               NET_init;
NETshutdown           NET_shutdown;
NETopen               NET_open;
NETclose              NET_close;
NETtest               NET_test;
NETconfigure          NET_configure;
NETabout              NET_about;
NETpause              NET_pause;
NETresume             NET_resume;
NETqueryPlayer        NET_queryPlayer;
NETsendData           NET_sendData;
NETrecvData           NET_recvData;
NETsendPadData        NET_sendPadData;
NETrecvPadData        NET_recvPadData;
NETsetInfo            NET_setInfo;
NETkeypressed         NET_keypressed;

#ifdef ENABLE_SIO1API

SIO1init              SIO1_init;
SIO1shutdown          SIO1_shutdown;
SIO1open              SIO1_open;
SIO1close             SIO1_close;
SIO1test              SIO1_test;
SIO1configure         SIO1_configure;
SIO1about             SIO1_about;
SIO1pause             SIO1_pause;
SIO1resume            SIO1_resume;
SIO1keypressed        SIO1_keypressed;
SIO1writeData8        SIO1_writeData8;
SIO1writeData16       SIO1_writeData16;
SIO1writeData32       SIO1_writeData32;
SIO1writeStat16       SIO1_writeStat16;
SIO1writeStat32       SIO1_writeStat32;
SIO1writeMode16       SIO1_writeMode16;
SIO1writeMode32       SIO1_writeMode32;
SIO1writeCtrl16       SIO1_writeCtrl16;
SIO1writeCtrl32       SIO1_writeCtrl32;
SIO1writeBaud16       SIO1_writeBaud16;
SIO1writeBaud32       SIO1_writeBaud32;
SIO1readData8         SIO1_readData8;
SIO1readData16        SIO1_readData16;
SIO1readData32        SIO1_readData32;
SIO1readStat16        SIO1_readStat16;
SIO1readStat32        SIO1_readStat32;
SIO1readMode16        SIO1_readMode16;
SIO1readMode32        SIO1_readMode32;
SIO1readCtrl16        SIO1_readCtrl16;
SIO1readCtrl32        SIO1_readCtrl32;
SIO1readBaud16        SIO1_readBaud16;
SIO1readBaud32        SIO1_readBaud32;
SIO1registerCallback  SIO1_registerCallback;

#endif

static const char *err;

#define CheckErr(func) { \
	err = SysLibError(); \
	if (err != NULL) { SysMessage(_("Error loading %s: %s"), func, err); return -1; } \
}

#define LoadSym(dest, src, name, checkerr) { \
	dest = (src)SysLoadSym(drv, name); \
	if (checkerr) { CheckErr(name); } else SysLibError(); \
}


void *hNETDriver = NULL;

void CALLBACK NET__setInfo(netInfo *info) {}
void CALLBACK NET__keypressed(int key) {}
long CALLBACK NET__configure(void) { return 0; }
long CALLBACK NET__test(void) { return 0; }
void CALLBACK NET__about(void) {}

#define LoadNetSym1(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, TRUE);

#define LoadNetSymN(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, FALSE);

#define LoadNetSym0(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, FALSE); \
	if (NET_##dest == NULL) NET_##dest = (NET##dest) NET__##dest;

static int LoadNETplugin(const char *NETdll) {
	void *drv;

	hNETDriver = SysLoadLibrary(NETdll);
	if (hNETDriver == NULL) {
		SysMessage (_("Could not load NetPlay plugin %s!"), NETdll); return -1;
	}
	drv = hNETDriver;
	LoadNetSym1(init, "NETinit");
	LoadNetSym1(shutdown, "NETshutdown");
	LoadNetSym1(open, "NETopen");
	LoadNetSym1(close, "NETclose");
	LoadNetSymN(sendData, "NETsendData");
	LoadNetSymN(recvData, "NETrecvData");
	LoadNetSym1(sendPadData, "NETsendPadData");
	LoadNetSym1(recvPadData, "NETrecvPadData");
	LoadNetSym1(queryPlayer, "NETqueryPlayer");
	LoadNetSym1(pause, "NETpause");
	LoadNetSym1(resume, "NETresume");
	LoadNetSym0(setInfo, "NETsetInfo");
	LoadNetSym0(keypressed, "NETkeypressed");
	LoadNetSym0(configure, "NETconfigure");
	LoadNetSym0(test, "NETtest");
	LoadNetSym0(about, "NETabout");

	return 0;
}

#ifdef ENABLE_SIO1API

void *hSIO1Driver = NULL;

long CALLBACK SIO1__init(void) { return 0; }
long CALLBACK SIO1__shutdown(void) { return 0; }
long CALLBACK SIO1__open(void) { return 0; }
long CALLBACK SIO1__close(void) { return 0; }
long CALLBACK SIO1__configure(void) { return 0; }
long CALLBACK SIO1__test(void) { return 0; }
void CALLBACK SIO1__about(void) {}
void CALLBACK SIO1__pause(void) {}
void CALLBACK SIO1__resume(void) {}
long CALLBACK SIO1__keypressed(int key) { return 0; }
void CALLBACK SIO1__writeData8(unsigned char val) {}
void CALLBACK SIO1__writeData16(unsigned short val) {}
void CALLBACK SIO1__writeData32(unsigned long val) {}
void CALLBACK SIO1__writeStat16(unsigned short val) {}
void CALLBACK SIO1__writeStat32(unsigned long val) {}
void CALLBACK SIO1__writeMode16(unsigned short val) {}
void CALLBACK SIO1__writeMode32(unsigned long val) {}
void CALLBACK SIO1__writeCtrl16(unsigned short val) {}
void CALLBACK SIO1__writeCtrl32(unsigned long val) {}
void CALLBACK SIO1__writeBaud16(unsigned short val) {}
void CALLBACK SIO1__writeBaud32(unsigned long val) {}
unsigned char CALLBACK SIO1__readData8(void) { return 0; }
unsigned short CALLBACK SIO1__readData16(void) { return 0; }
unsigned long CALLBACK SIO1__readData32(void) { return 0; }
unsigned short CALLBACK SIO1__readStat16(void) { return 0; }
unsigned long CALLBACK SIO1__readStat32(void) { return 0; }
unsigned short CALLBACK SIO1__readMode16(void) { return 0; }
unsigned long CALLBACK SIO1__readMode32(void) { return 0; }
unsigned short CALLBACK SIO1__readCtrl16(void) { return 0; }
unsigned long CALLBACK SIO1__readCtrl32(void) { return 0; }
unsigned short CALLBACK SIO1__readBaud16(void) { return 0; }
unsigned long CALLBACK SIO1__readBaud32(void) { return 0; }
void CALLBACK SIO1__registerCallback(void (CALLBACK *callback)(void)) {};

void CALLBACK SIO1irq(void) {
    psxHu32ref(0x1070) |= SWAPu32(0x100);
}

#define LoadSio1Sym1(dest, name) \
    LoadSym(SIO1_##dest, SIO1##dest, name, TRUE);

#define LoadSio1SymN(dest, name) \
    LoadSym(SIO1_##dest, SIO1##dest, name, FALSE);

#define LoadSio1Sym0(dest, name) \
    LoadSym(SIO1_##dest, SIO1##dest, name, FALSE); \
    if (SIO1_##dest == NULL) SIO1_##dest = (SIO1##dest) SIO1__##dest;

static int LoadSIO1plugin(const char *SIO1dll) {
    void *drv;

    hSIO1Driver = SysLoadLibrary(SIO1dll);
    if (hSIO1Driver == NULL) {
        SysMessage (_("Could not load SIO1 plugin %s!"), SIO1dll); return -1;
    }
    drv = hSIO1Driver;

    LoadSio1Sym0(init, "SIO1init");
    LoadSio1Sym0(shutdown, "SIO1shutdown");
    LoadSio1Sym0(open, "SIO1open");
    LoadSio1Sym0(close, "SIO1close");
    LoadSio1Sym0(pause, "SIO1pause");
    LoadSio1Sym0(resume, "SIO1resume");
    LoadSio1Sym0(keypressed, "SIO1keypressed");
    LoadSio1Sym0(configure, "SIO1configure");
    LoadSio1Sym0(test, "SIO1test");
    LoadSio1Sym0(about, "SIO1about");
    LoadSio1Sym0(writeData8, "SIO1writeData8");
    LoadSio1Sym0(writeData16, "SIO1writeData16");
    LoadSio1Sym0(writeData32, "SIO1writeData32");
    LoadSio1Sym0(writeStat16, "SIO1writeStat16");
    LoadSio1Sym0(writeStat32, "SIO1writeStat32");
    LoadSio1Sym0(writeMode16, "SIO1writeMode16");
    LoadSio1Sym0(writeMode32, "SIO1writeMode32");
    LoadSio1Sym0(writeCtrl16, "SIO1writeCtrl16");
    LoadSio1Sym0(writeCtrl32, "SIO1writeCtrl32");
    LoadSio1Sym0(writeBaud16, "SIO1writeBaud16");
    LoadSio1Sym0(writeBaud32, "SIO1writeBaud32");
    LoadSio1Sym0(readData16, "SIO1readData16");
    LoadSio1Sym0(readData32, "SIO1readData32");
    LoadSio1Sym0(readStat16, "SIO1readStat16");
    LoadSio1Sym0(readStat32, "SIO1readStat32");
    LoadSio1Sym0(readMode16, "SIO1readMode16");
    LoadSio1Sym0(readMode32, "SIO1readMode32");
    LoadSio1Sym0(readCtrl16, "SIO1readCtrl16");
    LoadSio1Sym0(readCtrl32, "SIO1readCtrl32");
    LoadSio1Sym0(readBaud16, "SIO1readBaud16");
    LoadSio1Sym0(readBaud32, "SIO1readBaud32");
    LoadSio1Sym0(registerCallback, "SIO1registerCallback");

    return 0;
}

#endif

void CALLBACK clearDynarec(void) {
	PSXCPU_Reset();
}

int LoadPlugins() {
	int ret;
	char Plugin[MAXPATHLEN];

	ReleasePlugins();

	if (strcmp("Disabled", Config.Net) == 0 || strcmp("", Config.Net) == 0)
		Config.UseNet = FALSE;
	else {
		Config.UseNet = TRUE;
		sprintf(Plugin, "%s/%s", Config.PluginsDir, Config.Net);
		if (LoadNETplugin(Plugin) == -1) Config.UseNet = FALSE;
	}

#ifdef ENABLE_SIO1API
	sprintf(Plugin, "%s/%s", Config.PluginsDir, Config.Sio1);
	if (LoadSIO1plugin(Plugin) == -1) return -1;
#endif

	pkCDRinit();
	pkGPUinit();
	pkSPUinit();
	pkPADinit(1);
	pkPADinit(2);

	if (Config.UseNet) {
		ret = NET_init();
		if (ret < 0) { SysMessage (_("Error initializing NetPlay plugin: %d"), ret); return -1; }
	}

#ifdef ENABLE_SIO1API
	ret = SIO1_init();
	if (ret < 0) { SysMessage (_("Error initializing SIO1 plugin: %d"), ret); return -1; }
#endif

	SysPrintf(_("Plugins loaded.\n"));
	return 0;
}

void ReleasePlugins() {
	if (Config.UseNet) {
		int ret = NET_close();
		if (ret < 0) Config.UseNet = FALSE;
	}
	NetOpened = FALSE;

	pkCDRshutdown();
	pkGPUshutdown();
	pkSPUshutdown();
	pkPADshutdown();

	if (Config.UseNet && hNETDriver != NULL) NET_shutdown();

	if (Config.UseNet && hNETDriver != NULL) {
		SysCloseLibrary(hNETDriver); hNETDriver = NULL;
	}

#ifdef ENABLE_SIO1API
	if (hSIO1Driver != NULL) {
		SIO1_shutdown();
		SysCloseLibrary(hSIO1Driver);
		hSIO1Driver = NULL;
	}
#endif
}


