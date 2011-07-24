#ifndef MEDNAFEN_INCLUDES_H
#define	MEDNAFEN_INCLUDES_H 

//Main Mednafen includes, relativly static
#include "src/mednafen.h"
#include "src/driver.h"
#include "src/general.h"
#include "src/mempatcher-driver.h"

#include <es_system.h>

void							ReloadEmulator			(const std::string& aFileName);
void							Exit					();
extern "C" MDFNGI*				nestGetEmulator			(uint32_t aIndex);
extern "C" MDFNGI*				gmbtGetEmulator			(uint32_t aIndex);
extern "C" MDFNGI*				vbamGetEmulator			(uint32_t aIndex);
extern "C" MDFNGI*				pcsxGetEmulator			(uint32_t aIndex);
extern "C" MDFNGI*				stellaGetEmulator		(uint32_t aIndex);

#ifdef TEST_MODULES
//Disabled because it requires a user provided build of libsnes and conflicts with the build in snes emulator
extern "C" MDFNGI*				lsnesGetEmulator		(uint32_t aIndex);

//Disabled because the c68k emu conflicts with the built in megadrive emulator
extern "C" MDFNGI*				yabauseGetEmulator		(uint32_t aIndex);
#endif

void							MDFND_NetStart			();

extern std::vector<MDFNGI *>	MDFNSystems;


#endif


