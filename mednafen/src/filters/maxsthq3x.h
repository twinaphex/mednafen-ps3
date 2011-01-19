/***************************************************************************
 *   Copyright (C) 2007 by Sindre Aamås                                    *
 *   aamas@stud.ntnu.no                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 as     *
 *   published by the Free Software Foundation.                            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License version 2 for more details.                *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   version 2 along with this program; if not, write to the               *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MAXSTHQ3X_H
#define MAXSTHQ3X_H

#include "filter.h"

struct FilterInfo;

class MaxSt_Hq3x : public Filter {
	uint32_t *buffer;
	uint32_t *obuffer;
	uint32_t width, height;
	
public:
	MaxSt_Hq3x();
	~MaxSt_Hq3x();
	void init(uint32_t aWidth, uint32_t aHeight);
	void outit();
	const FilterInfo& info();
	void filter();
	uint32_t* inBuffer();
	unsigned inPitch();
	uint32_t* outBuffer();
	unsigned outPitch();
	uint32_t getWidth();
	uint32_t getHeight();
};

#endif
