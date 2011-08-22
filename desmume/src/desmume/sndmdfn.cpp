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
#define MODULENAMESPACE desmume
#define SECONDARYINCLUDE
#include <module_helper.h>
using namespace desmume;

#include "SPU.h"
#include "types.h"

#define SNDCORE_MDFN	10010

static int	SNDMDFNInit			(int buffersize)				{return 0;}
static void SNDMDFNDeInit		()								{}
static void SNDMDFNUpdateAudio	(s16 *buffer, u32 num_samples)	{Resampler::Fill(buffer, num_samples);}
static u32	SNDMDFNGetAudioSpace()								{return (Resampler::Written() > 1000) ? 0 : 735 * 2;}
static void SNDMDFNMuteAudio	()								{}
static void SNDMDFNUnMuteAudio	()								{}
static void SNDMDFNSetVolume	(int volume)					{}

SoundInterface_struct SNDMDFN =
{
	SNDCORE_MDFN,
	"Mednafen Sound Interface",
	SNDMDFNInit,
	SNDMDFNDeInit,
	SNDMDFNUpdateAudio,
	SNDMDFNGetAudioSpace,
	SNDMDFNMuteAudio,
	SNDMDFNUnMuteAudio,
	SNDMDFNSetVolume
};


#endif

