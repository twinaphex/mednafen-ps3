/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "mednafen.h"
#include "netplay.h"
#include "compress/minilzo.h"
#include "video.h"
#include "mmmreader.h"
#include <math.h>

extern MDFNGI MMPlayGI;

static MMM_Stream_Info StreamInfo;
static MMM_Reader *mmm_reader = NULL;
static MMM_Stream_Frame *Paused;

static uint8 *controller_ptr;
static uint8 last_controller;

static void Emulate(EmulateSpecStruct *espec)
{
 bool NeedTimeDisplay = FALSE;
 MMM_Stream_Frame *frame;
 uint8 c = *controller_ptr;
 uint8 lc = last_controller;

 if(espec->VideoFormatChanged)
  mmm_reader->SetRGBAFormat(espec->surface->format.Rshift, espec->surface->format.Gshift, espec->surface->format.Bshift, espec->surface->format.Ashift);


 espec->SoundBufSize = 0;

 if(Paused)
 {
  frame = Paused;
  if(frame->audio)
  {
   for(unsigned int i = 0; i < StreamInfo.soundchan * frame->audio_frame_count; i++)
    frame->audio[i] = 0;
  }
 }
 else
 {
  if(!(frame = mmm_reader->ReadFrame()))
   return;
 }

 if(frame->line_widths)
  for(int y = 0; y < frame->height; y++)
   espec->LineWidths[y].w = frame->line_widths[y];

 for(int y = 0; y < frame->height; y++)
 {
  uint32 *src_line_ptr = frame->video + frame->pitch32 * y;
  uint32 *dest_line_ptr = espec->surface->pixels + espec->surface->pitch32 * y;
  int line_width = frame->width ? frame->width : espec->LineWidths[y].w;

  memcpy(dest_line_ptr, src_line_ptr, line_width * sizeof(uint32));
 }

 espec->DisplayRect.x = 0;
 espec->DisplayRect.y = 0;
 espec->DisplayRect.w = frame->width;
 espec->DisplayRect.h = frame->height;

 espec->SoundBufSize = frame->audio_frame_count;
 memcpy(espec->SoundBuf, frame->audio, frame->audio_frame_count * sizeof(int16) * StreamInfo.soundchan);

 if(mmm_reader->GetCurFrameNum() >= StreamInfo.total_frames)
 {
  Paused = frame;
 }
 else
 {
  if((c & 1) && !(lc & 1))
  {
   Paused = Paused ? NULL : frame;
  }
 }

 if((c & 0x02) && !(lc & 0x02))
 {
  int64 dest = mmm_reader->GetCurFrameNum() - round(StreamInfo.average_fps) - 1;

  if(dest < 0)
   dest = 0;

  mmm_reader->SeekFrame(dest);
  Paused = NULL;
  NeedTimeDisplay = TRUE;
 }

 if((c & 0x04) && !(lc & 0x04))
 {
  mmm_reader->SeekFrame(mmm_reader->GetCurFrameNum() + round(StreamInfo.average_fps) - 1);
  Paused = NULL;
  NeedTimeDisplay = TRUE;
 }

 if((c & 0x08) && !(lc & 0x08))
 {
  int64 dest = mmm_reader->GetCurFrameNum() - round(StreamInfo.average_fps) * 10 - 1;

  if(dest < 0) 
   dest = 0;

  mmm_reader->SeekFrame(dest);
  Paused = NULL;
  NeedTimeDisplay = TRUE;
 }

 if((c & 0x10) && !(lc & 0x10))
 {
  mmm_reader->SeekFrame(mmm_reader->GetCurFrameNum() + round(StreamInfo.average_fps) * 10 - 1);
  Paused = NULL;
  NeedTimeDisplay = TRUE;
 }

 last_controller = *controller_ptr;

 if(Paused && Paused != frame)
  mmm_reader->FreeFrame(frame); 

 if(Paused || NeedTimeDisplay)
 {
  MDFN_DispMessage("%llu.%.03u", (unsigned long long)mmm_reader->GetCurFrameTime() / 1000, (unsigned int)(mmm_reader->GetCurFrameTime() % 999));
 }
}


static void CloseGame(void)
{
 if(mmm_reader)
 {
  mmm_reader->Close();
  delete mmm_reader;
 }
}

int MMPlay_Load(const char *fn)
{
 mmm_reader = new MMM_Reader();
 if(!(mmm_reader->Open(fn, TRUE, TRUE)))
 {
  delete mmm_reader;
  return(0);
 }

 mmm_reader->GetStreamInfo(&StreamInfo);

 Paused = NULL;

 MDFN_printf("Version:\t%08x\n", StreamInfo.version);
 MDFN_printf("Video compressor:\t%s\n", mmm_reader->GetVideoCompressorName());
 MDFN_printf("Audio compressor:\t%s\n", mmm_reader->GetAudioCompressorName());
 MDFN_printf("Minimum width:\t%d\n", StreamInfo.min_width);
 MDFN_printf("Maximum width:\t%d\n", StreamInfo.max_width);
 MDFN_printf("Minimum height:\t%d\n", StreamInfo.min_height);
 MDFN_printf("Maximum height:\t%d\n", StreamInfo.max_height);
 MDFN_printf("Nominal width:\t%d\n", StreamInfo.nominal_width);
 MDFN_printf("Nominal height:\t%d\n", StreamInfo.nominal_height);
 MDFN_printf("Sound playback rate:\t%d\n", (int)(StreamInfo.rate >> 32));
 MDFN_printf("Sound channels:\t%d\n", StreamInfo.soundchan);
 MDFN_printf("Total video/stream frames:\t%llu\n", (unsigned long long)StreamInfo.total_frames);
 MDFN_printf("Total audio frames(samples / channels):\t%llu\n", (unsigned long long)StreamInfo.total_audio_frames);
 MDFN_printf("Average FPS:\t%f\n", StreamInfo.average_fps);

 MMPlayGI.soundrate = StreamInfo.rate >> 32;
 MMPlayGI.soundchan = StreamInfo.soundchan;

 MMPlayGI.nominal_width = StreamInfo.nominal_width;
 MMPlayGI.nominal_height = StreamInfo.nominal_height;

 MMPlayGI.fb_width = StreamInfo.max_width;
 MMPlayGI.fb_height = StreamInfo.max_height;

 MMPlayGI.fps = (uint32)(StreamInfo.average_fps * 65536 * 256);

 MMPlayGI.name = NULL;

 return(1);
}

static void SetInput(int port, const char *type, void *ptr)
{
 controller_ptr = (uint8 *)ptr;
}

static void DoSimpleCommand(int cmd)
{
 switch(cmd)
 {
  case MDFNNPCMD_POWER:
  case MDFNNPCMD_RESET: break;
 }
}

static MDFNSetting MMPlaySettings[] =
{

 { NULL }
};

static const InputDeviceInputInfoStruct IDII[] =
{
 { "pause", "Pause", 0, IDIT_BUTTON, NULL },
 { "reverse_seek", "Reverse Seek", 1, IDIT_BUTTON, NULL },
 { "forward_seek", "Forward Seek", 2, IDIT_BUTTON, NULL },
 { "fast_reverse_seek", "Fast Reverse Seek", 3, IDIT_BUTTON, NULL },
 { "fast_forward_seek", "Fast Forward Seek", 4, IDIT_BUTTON, NULL },
};

static InputDeviceInfoStruct InputDeviceInfo[] =
{
 {
  "controller",
  "Controller",
  NULL,
  sizeof(IDII) / sizeof(InputDeviceInputInfoStruct),
  IDII,
 }
};

static const InputPortInfoStruct PortInfo[] =
{
 { 0, "builtin", "Built-In", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo }
};

static InputInfoStruct InputInfo =
{
 sizeof(PortInfo) / sizeof(InputPortInfoStruct),
 PortInfo
};

MDFNGI MMPlayGI =
{
 "mmplay",
 "Mednafen Multimedia Movie Player",
 NULL, //".mmm\0", // TODO?
 MODPRIO_INTERNAL_HIGH,
 NULL,
 &InputInfo,
 NULL, //Load,
 NULL,
 NULL,
 NULL,
 CloseGame,
 NULL,
 NULL,
 NULL,
 NULL,
 NULL,
 NULL,
 Emulate,
 SetInput,
 DoSimpleCommand,
 MMPlaySettings,
 0,
 0,
 // Dummy nominal width/height values
 TRUE, // Multires possible?
 320,
 240,

 0,
 0,

 2	// Maximum number of sound channels(set to 1 or 2 in Load())
};


