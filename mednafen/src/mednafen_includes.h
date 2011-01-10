#ifndef MEDNAFEN_INCLUDES_H
#define	MEDNAFEN_INCLUDES_H 

#include "src/mednafen.h"
#include "src/driver.h"

#include <ps3_system.h>

#include "filters/filter.h"
#include "inputhandler.h"
#include "mednafen_help.h"
#include "commandmenu.h"
#include "settingmenu.h"
#include "StateMenu.h"

void							ReloadEmulator			();
void							Exit					();
MDFNGI*							GetNestopia				();
MDFNGI*							GetGambatte				();
extern "C" MDFNGI*				vbamGetVBAM				();
MDFNGI*							GetPCSX					();

void							MDFNDES_SetStateTarget	(MednafenStateMenu* aMenu);


extern std::vector<MDFNGI *>	MDFNSystems;


#endif


