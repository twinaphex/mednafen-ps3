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
extern "C" MDFNGI*				nestGetEmulator			();
extern "C" MDFNGI*				gmbtGetEmulator			();
extern "C" MDFNGI*				vbamGetEmulator			();
extern "C" MDFNGI*				pcsxGetEmulator			();
extern "C" MDFNGI*				stellaGetEmulator		();

void							MDFNDES_BlockExit		(bool aExit);
void							MDFND_NetStart			();

extern std::vector<MDFNGI *>	MDFNSystems;


#endif


