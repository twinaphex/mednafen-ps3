#include <string.h>
#include <stdio.h>

#include "Gfx_#1.3.h"
#include "rsp.h"
#include "vi_MDFN.h"

static GFX_INFO gfxInfo;

VI_MDFN *vi;

extern "C"{

/******************************************************************
  Function: CaptureScreen
  Purpose:  This function dumps the current frame to a file
  input:    pointer to the directory to save the file to
  output:   none
*******************************************************************/ 
EXPORT void CALL videoCaptureScreen ( char * Directory )
{
}

/******************************************************************
  Function: ChangeWindow
  Purpose:  to change the window between fullscreen and window 
            mode. If the window was in fullscreen this should 
			change the screen to window mode and vice vesa.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoChangeWindow (void)
{
   static bool fullScreen = false;
   if (!fullScreen)
     {
	vi->switchFullScreenMode();
	fullScreen = true;
     }
   else
     {
	vi->switchWindowMode();
	fullScreen = false;
     }
}

/******************************************************************
  Function: DrawScreen
  Purpose:  This function is called when the emulator receives a
            WM_PAINT message. This allows the gfx to fit in when
			it is being used in the desktop.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoDrawScreen (void)
{
}

/******************************************************************
  Function: InitiateGFX
  Purpose:  This function is called when the DLL is started to give
            information from the emulator that the n64 graphics
			uses. This is not called from the emulation thread.
  Input:    Gfx_Info is passed to this function which is defined
            above.
  Output:   TRUE on success
            FALSE on failure to initialise
             
  ** note on interrupts **:
  To generate an interrupt set the appropriate bit in MI_INTR_REG
  and then call the function CheckInterrupts to tell the emulator
  that there is a waiting interrupt.
*******************************************************************/ 
EXPORT BOOL CALL videoInitiateGFX (GFX_INFO Gfx_Info)
{
   gfxInfo = Gfx_Info;
   return TRUE;
}

/******************************************************************
  Function: MoveScreen
  Purpose:  This function is called in response to the emulator
            receiving a WM_MOVE passing the xpos and ypos passed
			from that message.
  input:    xpos - the x-coordinate of the upper-left corner of the
            client area of the window.
			ypos - y-coordinate of the upper-left corner of the
			client area of the window. 
  output:   none
*******************************************************************/ 
EXPORT void CALL videoMoveScreen (int xpos, int ypos)
{
}

/******************************************************************
  Function: ProcessDList
  Purpose:  This function is called when there is a Dlist to be
            processed. (High level GFX list)
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoProcessDList(void)
{
   RSP rsp(gfxInfo);
   /*static int firstTime=0;
   
   if(firstTime< 1)
     {
	firstTime++;
	
	bool found = true;
	int level = 1;
	OSTask_t *task = (OSTask_t*)(gfxInfo.DMEM+0xFC0);
	char *udata = (char*)gfxInfo.RDRAM + task->ucode_data;
	int length = task->ucode_data_size;
	
	while (found)
	  {
	     printf("searching strings... (level %d)\n", level);
	     found = false;
	     for (int i=level; i<length; i++)
	       {
		  if(udata[i^3] >= 32 && udata[i^3] <=126)
		    {
		       bool isString = true;
		       for (int j=1; j<=level; j++)
			 if (udata[(i-j)^3] < 32 || udata[(i-j)^3] > 126)
			   isString = false;
		       if (isString)
			 {
			    found = true;
			    for (int j=level; j>=0; j--)
			      printf("%c", udata[(i-j)^3]);
			    printf("\n");
			    getchar();
			 }
		    }
	       }
	     level++;
	  }
     }*/
}

/******************************************************************
  Function: ProcessRDPList
  Purpose:  This function is called when there is a Dlist to be
            processed. (Low level GFX list)
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoProcessRDPList(void)
{
}

/******************************************************************
  Function: RomClosed
  Purpose:  This function is called when a rom is closed.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoRomClosed (void)
{
   delete vi;
}

/******************************************************************
  Function: RomOpen
  Purpose:  This function is called when a rom is open. (from the 
            emulation thread)
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoRomOpen (void)
{
   vi = new VI_MDFN(gfxInfo);
}

/******************************************************************
  Function: ShowCFB
  Purpose:  Useally once Dlists are started being displayed, cfb is
            ignored. This function tells the dll to start displaying
			them again.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoShowCFB (void)
{
}

/******************************************************************
  Function: UpdateScreen
  Purpose:  This function is called in response to a vsync of the
            screen were the VI bit in MI_INTR_REG has already been
			set
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoUpdateScreen (void)
{
   vi->updateScreen();
}

/******************************************************************
  Function: ViStatusChanged
  Purpose:  This function is called to notify the dll that the
            ViStatus registers value has been changed.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoViStatusChanged (void)
{
   vi->statusChanged();
}

/******************************************************************
  Function: ViWidthChanged
  Purpose:  This function is called to notify the dll that the
            ViWidth registers value has been changed.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL videoViWidthChanged (void)
{
   vi->widthChanged();
}

EXPORT void CALL videoReadScreen2(void *dest, int *width, int *height, int front){};
EXPORT void CALL videoSetRenderingCallback(void (*callback)(void)){};
EXPORT void CALL videoFBRead(unsigned int addr){};
EXPORT void CALL videoFBWrite(unsigned int addr, unsigned int size){};
EXPORT void CALL videoFBGetFrameBufferInfo(void *p){};

}
