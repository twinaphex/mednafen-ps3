#ifndef FILTIDENTITY_H
#define FILTIDENTITY_H

#include "filter.h"

struct FilterInfo;

class Identity : public Filter {
	uint32_t *buffer;
	uint32_t *obuffer;
	uint32_t width, height;

public:
	Identity();
	~Identity();
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

class Identity2x : public Filter {
	uint32_t *buffer;
	uint32_t *obuffer;
	uint32_t width, height;

public:
	Identity2x();
	~Identity2x();
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
