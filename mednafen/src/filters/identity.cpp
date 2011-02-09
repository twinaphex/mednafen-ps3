#include "identity.h"
#include "filterinfo.h"
#include <cstring>

Identity::Identity() {
	buffer = NULL;
	obuffer = NULL;
	width = 0;
	height = 0;
}

Identity::~Identity() {
	delete []buffer;
	delete []obuffer;
}

void Identity::init(uint32_t aWidth, uint32_t aHeight) {
	delete []buffer;
	delete []obuffer;

	width = aWidth;
	height = aHeight;

	buffer = new uint32_t[(width) * (height)];
	obuffer = new uint32_t[(width) * (height)];
}

void Identity::outit() {
	delete []buffer;
	delete []obuffer;	
	buffer = NULL;
	obuffer = NULL;
}

const FilterInfo& Identity::info() {
	static FilterInfo fInfo = { "None", 1, 1 };
	
	return fInfo;
}

uint32_t* Identity::inBuffer() {
	return buffer;
}

unsigned Identity::inPitch() {
	return width;
}

void Identity::filter() {
	memcpy(obuffer, buffer, width * height * sizeof(uint32_t));
}

uint32_t* Identity::outBuffer() {
	return obuffer;
}

uint32_t Identity::outPitch() {
	return width;
}

uint32_t Identity::getWidth() {
	return width;
}

uint32_t Identity::getHeight() {
	return height;
}


Identity2x::Identity2x() {
	buffer = NULL;
	obuffer = NULL;
	width = 0;
	height = 0;
}

Identity2x::~Identity2x() {
	delete []buffer;
	delete []obuffer;
}

void Identity2x::init(uint32_t aWidth, uint32_t aHeight) {
	delete []buffer;
	delete []obuffer;

	width = aWidth;
	height = aHeight;

	buffer = new uint32_t[(width) * (height)];
	obuffer = new uint32_t[(width * 2) * (height * 2)];
}

void Identity2x::outit() {
	delete []buffer;
	delete []obuffer;	
	buffer = NULL;
	obuffer = NULL;
}

const FilterInfo& Identity2x::info() {
	static FilterInfo fInfo = { "None", 2, 2 };
	
	return fInfo;
}

uint32_t* Identity2x::inBuffer() {
	return buffer;
}

unsigned Identity2x::inPitch() {
	return width;
}

void Identity2x::filter() {
	for(int i = 0; i != height * 2; i ++)
	{
		for(int j = 0; j != width * 2; j ++)
		{
			obuffer[i * (width * 2) + j] = buffer[(i / 2) * width + (j / 2)];
		}
	}
}

uint32_t* Identity2x::outBuffer() {
	return obuffer;
}

uint32_t Identity2x::outPitch() {
	return width * 2;
}

uint32_t Identity2x::getWidth() {
	return width;
}

uint32_t Identity2x::getHeight() {
	return height;
}
