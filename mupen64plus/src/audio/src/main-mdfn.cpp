/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-sdl-audio - main.c                                        *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2007-2009 Richard Goedeken                              *
 *   Copyright (C) 2007-2008 Ebenblues                                     *
 *   Copyright (C) 2003 JttL                                               *
 *   Copyright (C) 2002 Hactarux                                           *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <mednafen/mednafen.h>
#define MODULENAMESPACE mupen64plus
#define SECONDARYINCLUDE
#include <module_helper.h>
using namespace mupen64plus;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define M64P_PLUGIN_PROTOTYPES 1
#include "api/m64p_types.h"
#include "api/m64p_plugin.h"
#include "api/m64p_common.h"
#include "api/m64p_config.h"

#include "main.h"

/* local variables */
static void (*l_DebugCallback)(void *, int, const char *) = NULL;
static void *l_DebugCallContext = NULL;

/* Read header for type definition */
static AUDIO_INFO AudioInfo;
static int GameFreq = 33600;

/* Global functions */
static void DebugMessage(int level, const char *message, ...)
{
  char msgbuf[1024];
  va_list args;

  if (l_DebugCallback == NULL)
      return;

  va_start(args, message);
  vsprintf(msgbuf, message, args);

  (*l_DebugCallback)(l_DebugCallContext, level, msgbuf);

  va_end(args);
}

/* Mupen64Plus plugin functions */
EXPORT m64p_error CALL PluginStartup(m64p_dynlib_handle CoreLibHandle, void *Context, void (*DebugCallback)(void *, int, const char *))
{
    l_DebugCallback = DebugCallback;
    l_DebugCallContext = Context;
    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginShutdown(void){l_DebugCallback = NULL;l_DebugCallContext = NULL;return M64ERR_SUCCESS;}

EXPORT m64p_error CALL PluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion, int *APIVersion, const char **PluginNamePtr, int *Capabilities)
{
    if (PluginType != NULL)		*PluginType = M64PLUGIN_AUDIO;
    if (PluginVersion != NULL)	*PluginVersion = SDL_AUDIO_PLUGIN_VERSION;
	if (APIVersion != NULL)		*APIVersion = AUDIO_PLUGIN_API_VERSION;
    if (PluginNamePtr != NULL)	*PluginNamePtr = "Mupen64Plus SDL Audio Plugin";
    if (Capabilities != NULL)	*Capabilities = 0;                    
    return M64ERR_SUCCESS;
}

/* ----------- Audio Functions ------------- */
EXPORT void CALL AiDacrateChanged( int SystemType )
{
    switch (SystemType)
    {
        case SYSTEM_NTSC: GameFreq = 48681812 / (*AudioInfo.AI_DACRATE_REG + 1); break;
        case SYSTEM_PAL:  GameFreq = 49656530 / (*AudioInfo.AI_DACRATE_REG + 1); break;
        case SYSTEM_MPAL: GameFreq = 48628316 / (*AudioInfo.AI_DACRATE_REG + 1); break;
    }
}


EXPORT void CALL AiLenChanged(void)
{
	uint32_t len = *AudioInfo.AI_LEN_REG;
	uint8_t* tar = (uint8_t*)Resampler::Buffer(len / 2);
	uint8_t* p = (uint8_t*)(AudioInfo.RDRAM + (*AudioInfo.AI_DRAM_ADDR_REG & 0xFFFFFF));

	for(int i = 0; i < len; i += 4)
	{
		tar[i] = p[ i + 2 ];
		tar[i + 1] = p[ i + 3 ];

		// Right channel
		tar[i + 2] = p[ i ];
		tar[i + 3] = p[ i + 1 ];
	}
}

EXPORT int CALL InitiateAudio(AUDIO_INFO Audio_Info){ AudioInfo = Audio_Info; return 1;}
EXPORT int CALL RomOpen(void){return 1;}
EXPORT void CALL RomClosed(void){}
EXPORT void CALL ProcessAList(void){}
EXPORT void CALL SetSpeedFactor(int percentage){}
EXPORT void CALL VolumeMute(void){}
EXPORT void CALL VolumeUp(void){}
EXPORT void CALL VolumeDown(void){}
EXPORT int CALL VolumeGetLevel(void){return 100;}
EXPORT void CALL VolumeSetLevel(int level){}
EXPORT const char * CALL VolumeGetString(void){return "Not Supported";}

