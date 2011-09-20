/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - dummy_audio.h                                           *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2008-2009 Richard Goedeken                              *
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

#if !defined(DUMMY_AUDIO_H)
#define DUMMY_AUDIO_H

#include "api/m64p_plugin.h"

#ifndef MDFNPS3 //Audio plugin
extern void dummyaudio_AiDacrateChanged(int SystemType);
extern void dummyaudio_AiLenChanged(void);
extern int  dummyaudio_InitiateAudio(AUDIO_INFO Audio_Info);
extern void dummyaudio_ProcessAList(void);
extern int  dummyaudio_RomOpen(void);
extern void dummyaudio_RomClosed(void);
extern void dummyaudio_SetSpeedFactor(int percent);
extern void dummyaudio_VolumeUp(void);
extern void dummyaudio_VolumeDown(void);
extern int dummyaudio_VolumeGetLevel(void);
extern void dummyaudio_VolumeSetLevel(int level);
extern void dummyaudio_VolumeMute(void);
extern const char * dummyaudio_VolumeGetString(void);
#else
extern void AiDacrateChanged(int SystemType);
extern void AiLenChanged(void);
extern int  InitiateAudio(AUDIO_INFO Audio_Info);
extern void ProcessAList(void);
extern int  RomOpen(void);
extern void RomClosed(void);
extern void SetSpeedFactor(int percent);
extern void VolumeUp(void);
extern void VolumeDown(void);
extern int VolumeGetLevel(void);
extern void VolumeSetLevel(int level);
extern void VolumeMute(void);
extern const char * VolumeGetString(void);

#define dummyaudio_AiDacrateChanged AiDacrateChanged
#define dummyaudio_AiLenChanged AiLenChanged
#define dummyaudio_InitiateAudio InitiateAudio
#define dummyaudio_ProcessAList ProcessAList
#define dummyaudio_RomOpen RomOpen
#define dummyaudio_RomClosed RomClosed
#define dummyaudio_SetSpeedFactor SetSpeedFactor
#define dummyaudio_VolumeUp VolumeUp
#define dummyaudio_VolumeDown VolumeDown
#define dummyaudio_VolumeGetLevel VolumeGetLevel
#define dummyaudio_VolumeSetLevel VolumeSetLevel
#define dummyaudio_VolumeMute VolumeMute
#define dummyaudio_VolumeGetString VolumeGetString
#endif

#endif /* DUMMY_AUDIO_H */


