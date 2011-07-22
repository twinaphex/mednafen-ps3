/*  Copyright 2005-2006 Theo Berkau

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifdef MDFNPS3 //ROBO: Only for medanfen

#include <src/mednafen.h>
#include <include/Fir_Resampler.h>

extern "C"
{

#include "error.h"
#include "scsp.h"
#include "sndsdl.h"
#include "debug.h"

#include "sndmdfn.h"

#include <stdlib.h>

int SNDMDFNInit();
void SNDMDFNDeInit();
int SNDMDFNReset();
int SNDMDFNChangeVideoFormat(int vertfreq);
void SNDMDFNUpdateAudio(u32 *leftchanbuffer, u32 *rightchanbuffer, u32 num_samples);
u32 SNDMDFNGetAudioSpace();
void SNDMDFNMuteAudio();
void SNDMDFNUnMuteAudio();
void SNDMDFNSetVolume(int volume);

SoundInterface_struct SNDMDFN = {
SNDCORE_MDFN,
"Mednafen Sound Interface",
SNDMDFNInit,
SNDMDFNDeInit,
SNDMDFNReset,
SNDMDFNChangeVideoFormat,
SNDMDFNUpdateAudio,
SNDMDFNGetAudioSpace,
SNDMDFNMuteAudio,
SNDMDFNUnMuteAudio,
SNDMDFNSetVolume
};

#define NUMSOUNDBLOCKS  4

extern Fir_Resampler<8>* mdfnyab_resampler;

//////////////////////////////////////////////////////////////////////////////

int SNDMDFNInit()
{
   return 0;
}

//////////////////////////////////////////////////////////////////////////////

void SNDMDFNDeInit()
{
}

//////////////////////////////////////////////////////////////////////////////

int SNDMDFNReset()
{
   return 0;
}

//////////////////////////////////////////////////////////////////////////////

int SNDMDFNChangeVideoFormat(int vertfreq)
{
   return 0;
}

//////////////////////////////////////////////////////////////////////////////
static void sdlConvert32uto16s(s32 *srcL, s32 *srcR, s16 *dst, u32 len) {
   u32 i;

   for (i = 0; i < len; i++)
   {
      // Left Channel
      if (*srcL > 0x7FFF) *dst = 0x7FFF;
      else if (*srcL < -0x8000) *dst = -0x8000;
      else *dst = *srcL;
      srcL++;
      dst++;
      // Right Channel
      if (*srcR > 0x7FFF) *dst = 0x7FFF;
      else if (*srcR < -0x8000) *dst = -0x8000;
      else *dst = *srcR;
      srcR++;
      dst++;
   } 
}

void SNDMDFNUpdateAudio(u32 *leftchanbuffer, u32 *rightchanbuffer, u32 num_samples)
{
	if(mdfnyab_resampler && mdfnyab_resampler->max_write() > (num_samples * 2))
	{
		sdlConvert32uto16s((s32*)leftchanbuffer, (s32*)rightchanbuffer, mdfnyab_resampler->buffer(), num_samples);
		mdfnyab_resampler->write(num_samples * 2);
	}
}

//////////////////////////////////////////////////////////////////////////////

u32 SNDMDFNGetAudioSpace()
{
   return mdfnyab_resampler ? ((mdfnyab_resampler->written() > 1000) ? 0 : 735 * 2) : 0;
}

//////////////////////////////////////////////////////////////////////////////

void SNDMDFNMuteAudio()
{
}

//////////////////////////////////////////////////////////////////////////////

void SNDMDFNUnMuteAudio()
{
}

//////////////////////////////////////////////////////////////////////////////

void SNDMDFNSetVolume(int volume)
{
}

//////////////////////////////////////////////////////////////////////////////

}

#endif
