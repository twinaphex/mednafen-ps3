#ifndef MEDNAFEN_INCLUDES_H
#define	MEDNAFEN_INCLUDES_H 

#include "src/mednafen.h"
#include "src/driver.h"
#include "src/general.h"

#include <ps3_system.h>

#include "ers.h"
#include "filters/filter.h"
#include "inputhandler.h"
#include "mednafen_help.h"
#include "commandmenu.h"
#include "settingmenu.h"
#include "StateMenu.h"

void							ReloadEmulator			();
void							Exit					();
extern "C" MDFNGI*				nestGetEmulator			();
extern "C" MDFNGI*				gmbtGetEmulator			();
extern "C" MDFNGI*				vbamGetEmulator			();
extern "C" MDFNGI*				pcsxGetEmulator			();

void							MDFNDES_SetStateTarget	(MednafenStateMenu* aMenu);
void							MDFNDES_BlockExit		(bool aExit);

extern std::vector<MDFNGI *>	MDFNSystems;


#endif


