/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-input-sdl - plugin.c                                      *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2008-2011 Richard Goedeken                              *
 *   Copyright (C) 2008 Tillin9                                            *
 *   Copyright (C) 2002 Blight                                             *
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

#define M64P_PLUGIN_PROTOTYPES 1

extern "C"
{
#include "m64p_types.h"
#include "m64p_plugin.h"
#include "m64p_common.h"
#include "m64p_config.h"

#include "plugin-mdfn.h"
#include "version.h"

/* global data definitions */
SController controller[4];   // 4 controllers

EXPORT m64p_error CALL inputPluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion, int *APIVersion, const char **PluginNamePtr, int *Capabilities)
{
    if (PluginType != NULL)			*PluginType = M64PLUGIN_INPUT;
    if (PluginVersion != NULL)		*PluginVersion = PLUGIN_VERSION;
    if (APIVersion != NULL)			*APIVersion = INPUT_PLUGIN_API_VERSION;
	if (PluginNamePtr != NULL)		*PluginNamePtr = PLUGIN_NAME;
    if (Capabilities != NULL)		*Capabilities = 0;

    return M64ERR_SUCCESS;
}


static unsigned char DataCRC( unsigned char *Data, int iLenght )
{
    unsigned char Remainder = Data[0];

    int iByte = 1;
    unsigned char bBit = 0;

    while( iByte <= iLenght )
    {
        int HighBit = ((Remainder & 0x80) != 0);
        Remainder = Remainder << 1;

        Remainder += ( iByte < iLenght && Data[iByte] & (0x80 >> bBit )) ? 1 : 0;

        Remainder ^= (HighBit) ? 0x85 : 0;

        bBit++;
        iByte += bBit/8;
        bBit %= 8;
    }

    return Remainder;
}

/******************************************************************
  Function: ControllerCommand
  Purpose:  To process the raw data that has just been sent to a
            specific controller.
  input:    - Controller Number (0 to 3) and -1 signalling end of
              processing the pif ram.
            - Pointer of data to be processed.
  output:   none

  note:     This function is only needed if the DLL is allowing raw
            data, or the plugin is set to raw

            the data that is being processed looks like this:
            initilize controller: 01 03 00 FF FF FF
            read controller:      01 04 01 FF FF FF FF
*******************************************************************/
EXPORT void CALL inputControllerCommand(int Control, unsigned char *Command)
{
    unsigned char *Data = &Command[5];

    if (Control == -1)
        return;

    switch (Command[2])
    {
        case RD_GETSTATUS:
            break;
        case RD_READKEYS:
            break;
        case RD_READPAK:
            if (controller[Control].control->Plugin == PLUGIN_RAW)
            {
                unsigned int dwAddress = (Command[3] << 8) + (Command[4] & 0xE0);

                if(( dwAddress >= 0x8000 ) && ( dwAddress < 0x9000 ) )
                    memset( Data, 0x80, 32 );
                else
                    memset( Data, 0x00, 32 );

                Data[32] = DataCRC( Data, 32 );
            }
            break;
        case RD_WRITEPAK:
            if (controller[Control].control->Plugin == PLUGIN_RAW)
            {
                unsigned int dwAddress = (Command[3] << 8) + (Command[4] & 0xE0);
              if (dwAddress == PAK_IO_RUMBLE && *Data)
                    DebugMessage(M64MSG_VERBOSE, "Triggering rumble pack.");
                Data[32] = DataCRC( Data, 32 );
            }
            break;
        case RD_RESETCONTROLLER:
            break;
        case RD_READEEPROM:
            break;
        case RD_WRITEEPROM:
            break;
        }
}

/******************************************************************
  Function: GetKeys
  Purpose:  To get the current state of the controllers buttons.
  input:    - Controller Number (0 to 3)
            - A pointer to a BUTTONS structure to be filled with
            the controller state.
  output:   none
*******************************************************************/
EXPORT void CALL inputGetKeys( int Control, BUTTONS *Keys )
{
	Keys->Value = Input::GetPort<2>(Control);
}


EXPORT void CALL inputInitiateControllers(CONTROL_INFO ControlInfo)
{
    int i;

    memset( controller, 0, sizeof( SController ) * 4 );

    for (i = 0; i < 4; i++)
	{
        controller[i].control = ControlInfo.Controls + i;
		controller[i].control->Present = 1;
		controller[i].control->RawData = 0;
		controller[i].control->Plugin = PLUGIN_MEMPAK;
	}

    DebugMessage(M64MSG_INFO, "%s version %i.%i.%i initialized.", PLUGIN_NAME, VERSION_PRINTF_SPLIT(PLUGIN_VERSION));
}

EXPORT m64p_error CALL inputPluginStartup(m64p_dynlib_handle CoreLibHandle, void *Context, void (*DebugCallback)(void *, int, const char *)){memset(controller, 0, sizeof(SController) * 4); return M64ERR_SUCCESS;}
EXPORT m64p_error CALL inputPluginShutdown(void){return M64ERR_SUCCESS;}
EXPORT void CALL inputReadController(int Control, unsigned char *Command){}
EXPORT void CALL inputRomClosed(void){}
EXPORT int CALL inputRomOpen(void){return 1;}
EXPORT void CALL inputSDL_KeyDown(int keymod, int keysym){}
EXPORT void CALL inputSDL_KeyUp(int keymod, int keysym){}

}
