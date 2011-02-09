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
#ifndef FILTER_H
#define FILTER_H

class Filter;
#include <mednafen_includes.h>

struct FilterInfo {
	std::string handle;
	unsigned int outWidth;
	unsigned int outHeight;
};

class Filter {
public:
	virtual ~Filter() {}
	virtual void init(uint32_t aWidth, uint32_t aHeight) {};
	virtual void outit() {};
	virtual const FilterInfo& info() = 0;
	virtual void filter() = 0;
	virtual uint32_t* inBuffer() = 0;
	virtual unsigned inPitch() = 0;
	virtual uint32_t* outBuffer() = 0;
	virtual unsigned outPitch() = 0;
	virtual uint32_t getWidth() = 0;
	virtual uint32_t getHeight() = 0;
	
};

#include "catrom2x.h"
#include "catrom3x.h"
#include "kreed2xsai.h"
#include "maxsthq2x.h"
#include "maxsthq3x.h"
#include "identity.h"

#endif
