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
#include "mmrecord.h"
#include "mmcommon.h"
#include <errno.h>
#include <vector>

#include "compress/minilzo.h"
#include "compress/quicklz.h"
#include "compress/blz.h"

static FILE *output_fp = NULL;
static uint8 *pixel_buffer = NULL;

#ifdef MM_DELTA_FRAMES
static uint8 *pixel_delta_buffer = NULL;
#endif

static uint32 compress_buffer_size;
static uint8 *compress_buffer = NULL;

// Filled out partially and written when movie playback starts, completed and rewritten when movie playback ends.
static MM_Main_Header main_header;

static uint64 video_frame_counter;
static uint64 audio_frame_counter;
static uint32 min_width, max_width, min_height, max_height;
static uint32 max_aframes_per_vframe;

static uint64 LastFrameOffset;
static uint64 LastFrameTime;

static std::vector<uint32> FrameRelativeIndex;
static std::vector<uint32> FrameRelativeTime;

bool MMRecord_Active(void)
{
 return(output_fp != NULL);
}

bool MMRecord_Start(const char *filename)
{
 MM_Main_Header header;

 memset(&header, 0, sizeof(MM_Main_Header));

 output_fp = fopen(filename, "wb");
 if(!output_fp)
 {
  ErrnoHolder ene(errno);

  MDFN_PrintError(_("Error opening \"%s\": %s\n"), filename, ene.StrError());
  return(0);
 }

 memcpy(main_header.magic, "Mednafen M Movie", 16);

 MDFN_enlsb(&main_header.version, 0);
 MDFN_enlsb(&main_header.video_compressor, 1);	// MiniLZO
 MDFN_enlsb(&main_header.audio_compressor, 0);	// Raw

 MDFN_enlsb(&main_header.nominal_width, MDFNGameInfo->nominal_width);
 MDFN_enlsb(&main_header.nominal_height, MDFNGameInfo->nominal_height);

 //// 32.32 sound rate
 // MDFN_enlsb(&main_header.sound_rate, (uint64)FSettings.SndRate << 32);

 // Number of sound channels
 MDFN_enlsb(&main_header.sound_channels, MDFNGameInfo->soundchan);


 fwrite(&main_header, 1, sizeof(MM_Main_Header), output_fp);

 if(!(pixel_buffer = (uint8 *)MDFN_malloc(MDFNGameInfo->fb_width * MDFNGameInfo->fb_height * sizeof(uint32), _("compression buffer"))))
 {
  fclose(output_fp);
  return(0);
 }

 #ifdef MM_DELTA_FRAMES
 if(!(pixel_delta_buffer = (uint8 *)MDFN_malloc(MDFNGameInfo->fb_width * MDFNGameInfo->fb_height * sizeof(uint32), _("compression buffer"))))
 {
  free(pixel_buffer);
  fclose(output_fp);
  return(0);
 }
 #endif

 compress_buffer_size = MDFNGameInfo->fb_width * MDFNGameInfo->fb_height * sizeof(uint32) * 1.10;
 if(!(compress_buffer = (uint8 *)MDFN_malloc(compress_buffer_size, _("compression buffer"))))
 {
  free(pixel_buffer);

  #ifdef MM_DELTA_FRAMES
  free(pixel_delta_buffer);
  #endif

  fclose(output_fp);
  return(0);
 }

 audio_frame_counter = 0;
 video_frame_counter = 0;

 min_width = min_height = 0xFFFFFFFF;
 max_width = max_height = 0x00000000;

 max_aframes_per_vframe = 1;

 LastFrameOffset = 0;
 LastFrameTime = 0;

 FrameRelativeIndex.clear(); 
 FrameRelativeTime.clear();

 return(1);
}

static bool WriteChunk(const EmulateSpecStruct *espec)
{
 MM_Frame_Header frame_header;

 uint32 frame_count = espec->SoundBufSize;
 uint16 height = espec->DisplayRect.h;
 uint16 width = (espec->LineWidths[0].w == ~0) ? espec->DisplayRect.w : 0;
 uint8 *pb_ptr = pixel_buffer;

 #ifdef MM_DELTA_FRAMES
 uint8 *pb_delta_ptr = pixel_delta_buffer;
 #endif

 uint8 workmem[LZO1X_1_MEM_COMPRESS];
 lzo_uint compressed_len;
 
 uint32 compressed_size;
 uint32 uncompressed_size;

 FrameRelativeIndex.push_back(ftell(output_fp) - LastFrameOffset);
 FrameRelativeTime.push_back(LastFrameTime);

 LastFrameOffset = ftell(output_fp);
 LastFrameTime = frame_count;


 // TODO FIXME: How should we handle rate changes during movie recording?
 // 32.32 sound rate
 MDFN_enlsb(&main_header.sound_rate, espec->SoundRate * ((uint64)1 << 32));


 memset(&frame_header, 0, sizeof(MM_Frame_Header));
 memcpy(frame_header.magic, "FRAME\0\0\0", 8);

 MDFN_enlsb(&frame_header.width, width);
 MDFN_enlsb(&frame_header.height, height);

 if(height > max_height)
  max_height = height;

 if(height < min_height)
  min_height = height;

 #ifdef MM_DELTA_FRAMES
 uint64 delta_count = 0;
 #endif

 for(int y = espec->DisplayRect.y; y < espec->DisplayRect.y + espec->DisplayRect.h; y++)
 {
  uint16 meow_width = (espec->LineWidths[0].w == ~0) ? espec->DisplayRect.w : espec->LineWidths[y].w;
  int meow_x = (espec->LineWidths[0].w == ~0) ? espec->DisplayRect.x : espec->LineWidths[y].x;
  uint32 *fb_line = espec->surface->pixels + y * espec->surface->pitch32 + meow_x;

  //if(!width)
  //{
  // fwrite(&meow_width, 1, 2, output_fp);
  //}

  if(meow_width > max_width)
   max_width = meow_width;

  if(meow_width < min_width)
   min_width = meow_width;

  for(int x = 0; x < meow_width; x++)
  {
   uint32 pixel = fb_line[x];
   int r, g, b;

   #ifdef MM_DELTA_FRAMES
   bool delta_pixel = FALSE;
   #endif

   espec->surface->DecodeColor(pixel, r, g, b);

   #ifdef MM_DELTA_FRAMES
   delta_pixel |= (bool)(*pb_delta_ptr++ = r - pb_ptr[0]);
   delta_pixel |= (bool)(*pb_delta_ptr++ = g - pb_ptr[1]);
   delta_pixel |= (bool)(*pb_delta_ptr++ = b - pb_ptr[2]);

   delta_count += delta_pixel;
   #endif

   *pb_ptr++ = r;
   *pb_ptr++ = g;
   *pb_ptr++ = b;
  }
 }

 #ifdef MM_DELTA_FRAMES
 bool use_delta = FALSE;

 if(delta_count <= ((pb_delta_ptr - pixel_delta_buffer) / (3 * 100)))
  use_delta = TRUE;

 if(use_delta)
 {
  uncompressed_size = pb_delta_ptr - pixel_delta_buffer;
  lzo1x_1_compress(pixel_delta_buffer, uncompressed_size, compress_buffer, &compressed_len, workmem);
 }
 else
 #endif
 {
  uncompressed_size = pb_ptr - pixel_buffer;
  lzo1x_1_compress(pixel_buffer, uncompressed_size, compress_buffer, &compressed_len, workmem);
 }

 compressed_size = compressed_len;

 MDFN_enlsb(&frame_header.compressed_size, compressed_size);
 MDFN_enlsb(&frame_header.uncompressed_size, uncompressed_size);
 MDFN_enlsb(&frame_header.audio_frame_count, frame_count);

 fwrite(&frame_header, 1, sizeof(MM_Frame_Header), output_fp);


 if(!width)
 {
  uint16 lws[espec->DisplayRect.h];
  
  for(int y = 0; y < espec->DisplayRect.h; y++)
   lws[y] = espec->LineWidths[espec->DisplayRect.y + y].w;

  Endian_A16_NE_to_LE(lws, espec->DisplayRect.h);

  fwrite(lws, 1, sizeof(lws), output_fp);
 }

 fwrite(compress_buffer, 1, compressed_len, output_fp);

 Endian_A16_NE_to_LE(espec->SoundBuf, MDFNGameInfo->soundchan * frame_count);
 fwrite(espec->SoundBuf, sizeof(int16) * MDFNGameInfo->soundchan, frame_count, output_fp);
 Endian_A16_LE_to_NE(espec->SoundBuf, MDFNGameInfo->soundchan * frame_count);

 if(max_aframes_per_vframe < frame_count)
  max_aframes_per_vframe = frame_count;

 audio_frame_counter += frame_count;

 video_frame_counter++;

 return(1);
}

void MMRecord_WriteFrame(const EmulateSpecStruct *espec)
{
 WriteChunk(espec);
}


bool MMRecord_End(void)
{
 if(output_fp)
 {
  uint64 FrameIndexTableOffset;
  const char *fi_header = "INDEX\0\0";

  // First, write the frame index chunk
  FrameIndexTableOffset = ftell(output_fp);
  fwrite(fi_header, 1, 8, output_fp);
  
  // FIXME:  Speedup
  for(uint64_t i = 0; i < FrameRelativeIndex.size(); i++)
  {
   uint32 qbuf[2];
   MDFN_enlsb(&qbuf[0], FrameRelativeIndex[i]);
   MDFN_enlsb(&qbuf[1], FrameRelativeTime[i]);
   fwrite(qbuf, 1, 4 * 2, output_fp);
  }

  // Finish our main header.
  MDFN_enlsb(&main_header.min_width, min_width);
  MDFN_enlsb(&main_header.max_width, max_width);
  MDFN_enlsb(&main_header.min_height, min_height);
  MDFN_enlsb(&main_header.max_height, max_height);

  MDFN_enlsb(&main_header.video_frame_counter, video_frame_counter);
  MDFN_enlsb(&main_header.audio_frame_counter, audio_frame_counter);
  MDFN_enlsb(&main_header.frame_index_offset, FrameIndexTableOffset);

  MDFN_enlsb(&main_header.max_aframes_per_vframe, max_aframes_per_vframe);

  // Now seek back and rewrite the completed header.
  fseek(output_fp, 0, SEEK_SET);
  fwrite(&main_header, 1, sizeof(MM_Main_Header), output_fp);

  fclose(output_fp);
  output_fp = NULL;
  LastFrameOffset = 0;
  LastFrameTime = 0;

  FrameRelativeIndex.clear();
  FrameRelativeTime.clear();
  return(1);
 }

 return(0);
}
