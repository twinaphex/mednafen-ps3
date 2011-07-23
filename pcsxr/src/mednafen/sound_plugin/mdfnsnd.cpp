//Base includes
extern "C"
{
	#include "stdafx.h"
	#define _IN_OSS
	#include "externals.h"
}

#include <include/Fir_Resampler.h>

namespace	pcsxr
{
	extern Fir_Resampler<8>*		Resampler;
}
using namespace pcsxr;

//Implement sound driver
extern "C"
{
	// SETUP SOUND
	void SetupSound(void)
	{
	}

	// REMOVE SOUND
	void RemoveSound(void)
	{
	}

	// GET BYTES BUFFERED
	unsigned long SoundGetBytesBuffered(void)
	{
		return Resampler ? Resampler->written() * 2 : 0;
	}

	// FEED SOUND DATA
	void SoundFeedStreamData(unsigned char* pSound,long lBytes)
	{
		if(Resampler && Resampler->max_write() >= lBytes / 2)
		{
			memcpy(Resampler->buffer(), pSound, lBytes);
			Resampler->write(lBytes / 2);
		}
	}
}
