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

void							MDFND_NetStart			();

extern std::vector<MDFNGI *>	MDFNSystems;


#endif


