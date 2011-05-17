#ifndef MEDNAFEN_INCLUDES_H
#define	MEDNAFEN_INCLUDES_H 

#include "src/mednafen.h"
#include "src/driver.h"
#include "src/general.h"

#include <es_system.h>

#include "ers.h"
#include "inputhandler.h"
#include "mednafen_help.h"
#include "settingmenu.h"

void							ReloadEmulator			();
void							Exit					();
extern "C" MDFNGI*				nestGetEmulator			();
extern "C" MDFNGI*				gmbtGetEmulator			();
extern "C" MDFNGI*				vbamGetEmulator			();
extern "C" MDFNGI*				pcsxGetEmulator			();

void							MDFNDES_BlockExit		(bool aExit);
void							MDFND_NetStart			();

extern std::vector<MDFNGI *>	MDFNSystems;


#endif


