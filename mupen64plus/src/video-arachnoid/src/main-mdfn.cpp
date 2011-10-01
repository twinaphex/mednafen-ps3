/******************************************************************************
 * Arachnoid Graphics Plugin for Mupen64Plus
 * http://bitbucket.org/wahrhaft/mupen64plus-video-arachnoid/
 *
 * Copyright (C) 2009 Jon Ring
 * Copyright (C) 2007 Kristofer Karlsson, Rickard Niklasson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

//Includes
#define M64P_PLUGIN_PROTOTYPES 1
#include "m64p.h"
#include "GraphicsPlugin.h"        //Main class
#include "config/Config.h"         //Configuration   
#include "Logger.h"                //Debug logger
#include "MemoryLeakDetector.h"    //For detecting memory leaks

#include "m64p_types.h"
#include "m64p_common.h"
#include "m64p_plugin.h"
#include "m64p_config.h"
#include "m64p_vidext.h"

#include "osal_dynamiclib.h"

//Definitions
#define PLUGIN_NAME "Arachnoid Video Plugin"
#define PLUGIN_VERSION           0x016304
#define VIDEO_PLUGIN_API_VERSION 0x020000
#define CONFIG_API_VERSION       0x020000
#define VIDEXT_API_VERSION       0x020000

#define VERSION_PRINTF_SPLIT(x) (((x) >> 16) & 0xffff), (((x) >> 8) & 0xff), ((x) & 0xff)

#define MI_INTR_DP 0x00000020      //!< RDP Interrupt signal
#define MI_INTR_SP 0x00000001      //!< RSP Interrupt signal

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

char              g_cfgFilename[] = "ConfigGraphicsPlugin.cfg";  //!< Name configuration file
GFX_INFO          g_graphicsInfo;                                //!< Information about window, memory...
GraphicsPlugin    g_graphicsPlugin;                              //!< Main class for application
Config            g_config(&g_graphicsPlugin);                   //!< Handles configuration

void (*renderCallback)() = NULL;


//-----------------------------------------------------------------------------
// Mupen64plus 2.0 Common Plugin API Functions
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
EXPORT m64p_error CALL videoPluginStartup(m64p_dynlib_handle CoreLibHandle, void *Context,
                                   void (*DebugCallback)(void *, int, const char *))
{
    char logMsg[530];
    Logger::getSingleton().initialize(DebugCallback, Context);
    Logger::getSingleton().printMsg("PluginStartup");

    //Read configuration
    if (g_config.initialize())
    {
        g_config.load();
        g_graphicsPlugin.setConfig(g_config.getConfig());
    }

    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL videoPluginShutdown(void)
{
    //Close Logger
    Logger::getSingleton().printMsg("CloseDLL\n");
    Logger::getSingleton().dispose();   

    //g_graphicsPlugin.dispose();  
    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL videoPluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion, int *APIVersion, const char **PluginNamePtr, int *Capabilities)
{
    /* set version info */
    if (PluginType != NULL)
        *PluginType = M64PLUGIN_GFX;

    if (PluginVersion != NULL)
        *PluginVersion = PLUGIN_VERSION;

    if (APIVersion != NULL)
        *APIVersion = VIDEO_PLUGIN_API_VERSION;
    
    if (PluginNamePtr != NULL)
        *PluginNamePtr = PLUGIN_NAME;

    if (Capabilities != NULL)
    {
        *Capabilities = 0;
    }
                    
    return M64ERR_SUCCESS;
}



//-----------------------------------------------------------------------------
// Mupen64plus 2.0 Video Plugin API Functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//* InitiateGFX
//! This function is called when the DLL is started to give
//! information from the emulator that the n64 graphics
//!    uses. This is not called from the emulation thread.
//! @param[in] Gfx_Info Information about rom and emulator
//! @return true on success, FALSE on failure to initialise
//!           
//! @note on interrupts :
//!    To generate an interrupt set the appropriate bit in MI_INTR_REG
//!    and then  the function CheckInterrupts to tell the emulator
//!    that there is a waiting interrupt.
//-----------------------------------------------------------------------------
EXPORT BOOL CALL videoInitiateGFX(GFX_INFO Gfx_Info)
{
    char logMsg[530];
    Logger::getSingleton().initialize(0, 0);
    Logger::getSingleton().printMsg("PluginStartup");

    //Read configuration
//    if (g_config.initialize())
//    {
//        g_config.load();
    g_graphicsPlugin.setConfig(g_config.getConfig());
//    }

    Logger::getSingleton().printMsg("InitiateGFX");

    //Save Graphics Info
    memcpy(&g_graphicsInfo, &Gfx_Info, sizeof(GFX_INFO));
    return true;
}

//-----------------------------------------------------------------------------
//* Rom Open
//! This function is called when a rom is open. (from the emulation thread)
//-----------------------------------------------------------------------------
EXPORT int CALL videoRomOpen()
{
    Logger::getSingleton().printMsg("RomOpen\n");
    return g_graphicsPlugin.initialize(&g_graphicsInfo);
}

//-----------------------------------------------------------------------------
//* Rom Closed
//! This function is called when a rom is closed.
//-----------------------------------------------------------------------------
EXPORT void CALL videoRomClosed()
{
    //Logger::getSingleton().printMsg("RomClosed\n");
    //Destroy 
    g_graphicsPlugin.dispose();
}

//-----------------------------------------------------------------------------
//* Update Screen
//! This function is called in response to a vsync of the
//! screen where the VI bit in MI_INTR_REG has already been set
//-----------------------------------------------------------------------------
EXPORT void CALL videoUpdateScreen()
{
    if (g_config.getConfig()->screenUpdateSetting == SCREEN_UPDATE_VI)
        g_graphicsPlugin.drawScreen();
    else if (g_config.getConfig()->screenUpdateSetting == SCREEN_UPDATE_CI)
        g_graphicsPlugin.setDrawScreenSignal();
    else
    {
        Logger::getSingleton().printMsg("Invalid screen update setting!", M64MSG_WARNING);
        g_graphicsPlugin.drawScreen();
    }
}


//-----------------------------------------------------------------------------
//* ProcessDList
//! This function is called when there is a Dlist to be processed. (High level GFX list)
//-----------------------------------------------------------------------------
EXPORT void CALL videoProcessDList()
{
    Logger::getSingleton().printMsg("ProcessDList\n");

    try
    {
        g_graphicsPlugin.viStatusChanged();
        g_graphicsPlugin.processDisplayList();
    }
    catch (...)
    {
        Logger::getSingleton().printMsg("Unknown Error processing DisplayList", M64MSG_WARNING); 
        //MessageBox(0, "Unknown Error processing DisplayList", "Arachnoid Graphics Plugin", MB_OK|MB_SETFOREGROUND); 

        g_graphicsPlugin.dispose();
        g_graphicsPlugin.initialize(&g_graphicsInfo);

        //Trigger Interupts
        *(g_graphicsInfo.MI_INTR_REG) |= MI_INTR_DP;
        g_graphicsInfo.CheckInterrupts();
        *(g_graphicsInfo.MI_INTR_REG) |= MI_INTR_SP;
        g_graphicsInfo.CheckInterrupts();    
    }
}


//-----------------------------------------------------------------------------
//* ProcessRDPList
//! This function is called when there is a Dlist to be processed. (Low level GFX list)
//! @todo ProcessRDPList
//-----------------------------------------------------------------------------
EXPORT void CALL videoProcessRDPList()
{
    Logger::getSingleton().printMsg("ProcessRDPList\n");
    //TODO
}

//-----------------------------------------------------------------------------
//* Show CFB
//! Usally once Dlists are started being displayed, cfb is
//! ignored. This function tells the dll to start displaying
//! them again.
//-----------------------------------------------------------------------------
EXPORT void CALL videoShowCFB()
{
    Logger::getSingleton().printMsg("ShowCFB\n");
}

//-----------------------------------------------------------------------------
//* ViStatusChanged
//! This function is called to notify the dll that the
//! ViStatus registers value has been changed.
//-----------------------------------------------------------------------------
EXPORT void CALL videoViStatusChanged()
{
    Logger::getSingleton().printMsg("ViStatusChanged");

    //g_graphicsPlugin.viStatusChanged();
}

//-----------------------------------------------------------------------------
//* ViWidthChanged
//! This function is called to notify the dll that the
//! ViWidth registers value has been changed.
//-----------------------------------------------------------------------------
EXPORT void CALL videoViWidthChanged()
{    
    Logger::getSingleton().printMsg("ViWidthChanged");
    //g_graphicsPlugin.viStatusChanged();
}

//-----------------------------------------------------------------------------
//* MoveScreen
//! This function is called in response to the emulator
//! receiving a WM_MOVE passing the xpos and ypos passed
//! from that message.
//! @param xpos The x-coordinate of the upper-left corner of the 
//!             client area of the window.
//! @param ypos The y-coordinate of the upper-left corner of the
//!             client area of the window. 
//! @todo MoveScreen
//----------------------------------------------------------------------------- 
EXPORT void CALL videoMoveScreen(int xpos, int ypos)
{
    Logger::getSingleton().printMsg("MoveScreen\n");
    //TODO
}

//-----------------------------------------------------------------------------
//* ChangeWindow
//! Toggle between fullscreen and window mode
//-----------------------------------------------------------------------------
EXPORT void CALL videoChangeWindow()
{
    Logger::getSingleton().printMsg("ChangeWindow\n");
    //Toggle Fullscreen
    g_graphicsPlugin.toggleFullscreen();
}

//-----------------------------------------------------------------------------
//* ReadScreen2
//! This function reads the pixels of the currently displayed screen
//-----------------------------------------------------------------------------
EXPORT void CALL videoReadScreen2(void *dest, int *width, int *height, int front)
{
    g_graphicsPlugin.takeScreenshot(dest, width, height, front);
}

//-----------------------------------------------------------------------------
//* SetRenderingCallback
//! Allows the core to register a callback function that will be called by the 
//! graphics plugin just before the the frame buffers are swapped.
//-----------------------------------------------------------------------------
EXPORT void CALL videoSetRenderingCallback(void (*callback)())
{
    OpenGLManager::getSingleton().setRenderingCallback(callback);
}

//-----------------------------------------------------------------------------
//* FBRead
//! Read data from frame buffer into emulated RAM space 
//-----------------------------------------------------------------------------
EXPORT void CALL videoFBRead(unsigned int addr)
{
    //TODO
}

//-----------------------------------------------------------------------------
//* FBWrite
//! Write data from emulated RAM space into frame buffer
//-----------------------------------------------------------------------------
EXPORT void CALL videoFBWrite(unsigned int addr, unsigned int size)
{
    //TODO
}

//-----------------------------------------------------------------------------
//* FBWrite
//! Get some information about the frame buffer 
//-----------------------------------------------------------------------------
EXPORT void videoFBGetFrameBufferInfo(void *p)
{
    //TODO
}

#ifdef __cplusplus
}
#endif
