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
#include "catrom2x.h"
#include "filterinfo.h"
#include <cstring>

struct Colorsum {
	uint32_t r, g, b;
};

static void merge_columns(uint32_t *dest, const Colorsum *sums, int32_t aWidth) {
	unsigned w = aWidth;
	
	while (w--) {
		{
			uint32_t rsum = sums[1].r;
			uint32_t gsum = sums[1].g;
			uint32_t bsum = sums[1].b;
			
			if (rsum & 0x80000000) rsum = 0;
			if (gsum & 0x80000000) gsum = 0;
			if (bsum & 0x80000000) bsum = 0;
			
			rsum <<= 12;
			rsum += 0x008000;
			gsum >>= 4;
			gsum += 0x0080;
			bsum += 0x0008;
			bsum >>= 4;
			
			if (rsum > 0xFF0000) rsum = 0xFF0000;
			if (gsum > 0x00FF00) gsum = 0x00FF00;
			if (bsum > 0x0000FF) bsum = 0x0000FF;
			
			*dest++ = (rsum & 0xFF0000) | (gsum & 0x00FF00) | bsum;
		}
		
		{
			uint32_t rsum = sums[1].r * 9;
			uint32_t gsum = sums[1].g * 9;
			uint32_t bsum = sums[1].b * 9;
			
			rsum -= sums[0].r;
			gsum -= sums[0].g;
			bsum -= sums[0].b;
			
			rsum += sums[2].r * 9;
			gsum += sums[2].g * 9;
			bsum += sums[2].b * 9;
			
			rsum -= sums[3].r;
			gsum -= sums[3].g;
			bsum -= sums[3].b;
			
			if (rsum & 0x80000000) rsum = 0;
			if (gsum & 0x80000000) gsum = 0;
			if (bsum & 0x80000000) bsum = 0;
			
			rsum <<= 8;
			rsum += 0x008000;
			gsum >>= 8;
			gsum += 0x000080;
			bsum += 0x000080;
			bsum >>= 8;
			
			if (rsum > 0xFF0000) rsum = 0xFF0000;
			if (gsum > 0x00FF00) gsum = 0x00FF00;
			if (bsum > 0x0000FF) bsum = 0x0000FF;
			
			*dest++ = (rsum & 0xFF0000) | (gsum & 0x00FF00) | bsum;
		}
		
		++sums;
	}
}

static Colorsum sums[1024];
static void filter(uint32_t *dline, const unsigned pitch, const uint32_t *sline, int32_t aWidth, int32_t aHeight) {
	for (unsigned h = aHeight; h--;) {
		{
			const uint32_t *s = sline;
			Colorsum *sum = sums;
			unsigned n = aWidth + 3;
			
			while (n--) {
				unsigned long pixel = *s;
				sum->r = pixel >> 12 & 0x000FF0 ;
				pixel <<= 4;
				sum->g = pixel & 0x0FF000;
				sum->b = pixel & 0x000FF0;
				
				++s;
				++sum;
			}
		}
		
		merge_columns(dline, sums, aWidth);
		dline += pitch;
		
		{
			const uint32_t *s = sline;
			Colorsum *sum = sums;
			unsigned n = aWidth + 3;
			
			while (n--) {
				unsigned long pixel = *s;
				unsigned long rsum = (pixel >> 16) * 9;
				unsigned long gsum = (pixel & 0x00FF00) * 9;
				unsigned long bsum = (pixel & 0x0000FF) * 9;
				
				pixel = s[-1*(aWidth + 3)];
				rsum -= pixel >> 16;
				gsum -= pixel & 0x00FF00;
				bsum -= pixel & 0x0000FF;
				
				pixel = s[1*(aWidth + 3)];
				rsum += (pixel >> 16) * 9;
				gsum += (pixel & 0x00FF00) * 9;
				bsum += (pixel & 0x0000FF) * 9;
				
				pixel = s[2*(aWidth + 3)];
				rsum -= pixel >> 16;
				gsum -= pixel & 0x00FF00;
				bsum -= pixel & 0x0000FF;
				
				sum->r = rsum;
				sum->g = gsum;
				sum->b = bsum;
				
				++s;
				++sum;
			}
		}
		
		merge_columns(dline, sums, aWidth);
		dline += pitch;
		sline += aWidth + 3;
	}
}

Catrom2x::Catrom2x() {
	buffer = NULL;
	obuffer = NULL;
	width = 0;
	height = 0;
}

Catrom2x::~Catrom2x() {
	delete []buffer;
	delete []obuffer;
}

void Catrom2x::init(uint32_t aWidth, uint32_t aHeight) {
	delete []buffer;
	delete []obuffer;

	width = aWidth;
	height = aHeight;

	buffer = new uint32_t[(width + 3) * (height + 3)];
	std::memset(buffer, 0, (width + 3) * (height + 3) * sizeof(uint32_t));
	
	obuffer = new uint32_t[(width * 2) * (height * 2)];
}

void Catrom2x::outit() {
	delete []buffer;
	delete []obuffer;	
	buffer = NULL;
	obuffer = NULL;
}

const FilterInfo& Catrom2x::info() {
	static FilterInfo fInfo = { "Bicubic Catmull-Rom Spline 2x", 2, 2 };
	
	return fInfo;
}

uint32_t* Catrom2x::inBuffer() {
	return buffer + (width + 4);
}

unsigned Catrom2x::inPitch() {
	return width + 3;
}

void Catrom2x::filter() {
	::filter(obuffer, width * 2, buffer + (width + 3), width, height);
}

uint32_t* Catrom2x::outBuffer() {
	return obuffer;
}

uint32_t Catrom2x::outPitch() {
	return width * 2;
}

uint32_t Catrom2x::getWidth() {
	return width;
}

uint32_t Catrom2x::getHeight() {
	return height;
}
