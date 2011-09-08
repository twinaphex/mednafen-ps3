#ifndef MEDNAFEN_INCLUDES_H
#define	MEDNAFEN_INCLUDES_H 

//Main Mednafen includes, relativly static
#include "mednafen/mednafen.h"
#include "mednafen/driver.h"
#include "mednafen/general.h"
#include "mednafen/mempatcher-driver.h"

#include <es_system.h>

void							ReloadEmulator			(const std::string& aFileName);
void							Exit					();

void							MDFND_NetStart			();

extern std::vector<MDFNGI *>	MDFNSystems;


#endif


