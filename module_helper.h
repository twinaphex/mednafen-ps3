#ifndef _MODULE_HELPER_MDFN_H
#define _MODULE_HELPER_MDFN_H

#include <include/Fir_Resampler.h>
#include <algorithm>
#include <stdint.h>

#ifndef MODULENAMESPACE
# error	"You must define MODULENAMESPACE before including module_helper.h"
#endif

#ifdef SECONDARYINCLUDE
# define TYPEDEC extern
#else
# define TYPEDEC
#endif

namespace	MODULENAMESPACE
{
	namespace	Resampler
	{
		TYPEDEC Fir_Resampler<8>*						Resampler;

		inline void __attribute((always_inline))		Init						(EmulateSpecStruct* aSpec, double aSourceRate)
		{
			assert(aSpec);
			assert(aSourceRate > 3200);

			if(aSpec->SoundFormatChanged)
			{
				delete Resampler;
				Resampler = 0;

				if(aSpec->SoundRate > 1.0)
				{
					Resampler = new Fir_Resampler<8>();
					Resampler->buffer_size(4800 * 2);
					Resampler->time_ratio(aSourceRate / aSpec->SoundRate, 0.9965);
				}
			}
		}

		inline void __attribute((always_inline))		Kill						()
		{
			delete Resampler;
			Resampler = 0;
		}

		inline void __attribute((always_inline))		Fetch						(EmulateSpecStruct* aSpec)
		{
			assert(aSpec);

			if(Resampler && aSpec->SoundBuf && aSpec->SoundBufMaxSize)
			{
				uint32_t readsize = std::min(Resampler->avail() / 2, aSpec->SoundBufMaxSize);
				aSpec->SoundBufSize = Resampler->read(aSpec->SoundBuf, readsize) >> 1;
			}
		}

		inline void __attribute((always_inline))		Fill						(void* aSamples, uint32_t aCount)
		{
			if(Resampler && Resampler->max_write() >= aCount)
			{
				memcpy(Resampler->buffer(), aSamples, aCount * 2);
				Resampler->write(aCount);
			}
		}

		inline int16_t* __attribute((always_inline))	Buffer						(uint32_t aSamples)
		{
			if(Resampler)
			{
				if(aSamples <= Resampler->max_write())
				{
					int16_t* buffer = Resampler->buffer();
					Resampler->write(aSamples);
					return buffer;
				}
			}

			return 0;
		}

		inline uint32_t __attribute((always_inline))	Written						()
		{
			return Resampler ? Resampler->written() : 0;
		}
	}
}

using namespace MODULENAMESPACE;

#endif

