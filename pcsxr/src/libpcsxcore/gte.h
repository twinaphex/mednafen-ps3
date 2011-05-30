/***************************************************************************
 *   PCSX-Revolution - PlayStation Emulator for Nintendo Wii               *
 *   Copyright (C) 2009-2010  PCSX-Revolution Dev Team                     *
 *   <http://code.google.com/p/pcsx-revolution/>                           *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

#ifndef __GTE_H__
#define __GTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psxcommon.h"
#include "r3000a.h"

void gteMFC2(uint32_t aCode);
void gteCFC2(uint32_t aCode);
void gteMTC2(uint32_t aCode);
void gteCTC2(uint32_t aCode);
void gteLWC2(uint32_t aCode);
void gteSWC2(uint32_t aCode);

void gteRTPS(uint32_t aCode);
void gteOP(uint32_t aCode);
void gteNCLIP(uint32_t aCode);
void gteDPCS(uint32_t aCode);
void gteINTPL(uint32_t aCode);
void gteMVMVA(uint32_t aCode);
void gteNCDS(uint32_t aCode);
void gteNCDT(uint32_t aCode);
void gteCDP(uint32_t aCode);
void gteNCCS(uint32_t aCode);
void gteCC(uint32_t aCode);
void gteNCS(uint32_t aCode);
void gteNCT(uint32_t aCode);
void gteSQR(uint32_t aCode);
void gteDCPL(uint32_t aCode);
void gteDPCT(uint32_t aCode);
void gteAVSZ3(uint32_t aCode);
void gteAVSZ4(uint32_t aCode);
void gteRTPT(uint32_t aCode);
void gteGPF(uint32_t aCode);
void gteGPL(uint32_t aCode);
void gteNCCT(uint32_t aCode);

#ifdef __cplusplus
}
#endif
#endif
