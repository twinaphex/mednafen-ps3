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

void							MDFNDES_BlockExit		(bool aExit);
void							MDFND_NetStart			();

extern std::vector<MDFNGI *>	MDFNSystems;


#endif


