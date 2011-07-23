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
	namespace	Video
	{
		inline void __attribute((always_inline))		SetDisplayRect				(EmulateSpecStruct* aSpec, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight)
		{
			assert(aX + aWidth <= aSpec->surface->w && aY + aHeight <= aSpec->surface->h);

			aSpec->DisplayRect.x = aX;
			aSpec->DisplayRect.y = aY;
			aSpec->DisplayRect.w = aWidth;
			aSpec->DisplayRect.h = aHeight;
		}

		template<int rS, int gS, int bS, int rD, int gD, int bD, int sMul, int sMask, int dMul, int dAdd, typename sType>
		inline void __attribute((always_inline))		BlitSwiz					(EmulateSpecStruct* aSpec, const sType* aSource, uint32_t aWidth, uint32_t aHeight, uint32_t aPixelPitch)
		{
			for(int i = 0; i != aHeight; i ++)
			{
				for(int j = 0; j != aWidth; j ++)
				{
					sType source = aSource[i * aPixelPitch + j];

					uint32_t r = ((source >> (rS * sMul)) & sMask) << (rD * dMul + dAdd);
					uint32_t g = ((source >> (gS * sMul)) & sMask) << (gD * dMul + dAdd);
					uint32_t b = ((source >> (bS * sMul)) & sMask) << (bD * dMul + dAdd);
					aSpec->surface->pixels[i * aSpec->surface->pitchinpix + j] = r | g | b;
				}
			}
		}


		template<int rS, int gS, int bS, int rD, int gD, int bD>
		inline void __attribute((always_inline))		BlitRGB15					(EmulateSpecStruct* aSpec, const uint16_t* aSource, uint32_t aWidth, uint32_t aHeight, uint32_t aPixelPitch)
		{
			BlitSwiz<rS, gS, bS, rD, gD, bD, 5, 0x1F, 8, 3, uint16_t>(aSpec, aSource, aWidth, aHeight, aPixelPitch);
		}

		template<int rS, int gS, int bS, int rD, int gD, int bD>
		inline void __attribute((always_inline))		BlitRGB32					(EmulateSpecStruct* aSpec, const uint32_t* aSource, uint32_t aWidth, uint32_t aHeight, uint32_t aPixelPitch)
		{
			BlitSwiz<rS, gS, bS, rD, gD, bD, 8, 0xFF, 8, 0, uint32_t>(aSpec, aSource, aWidth, aHeight, aPixelPitch);
		}

		template<typename pixelType>
		inline void __attribute((always_inline))		BlitRGB						(EmulateSpecStruct* aSpec, const pixelType* aSource, uint32_t aWidth, uint32_t aHeight, uint32_t aPixelPitch)
		{
			for(int i = 0; i != aHeight; i ++)
			{
				memcpy(&aSpec->surface->pixels[i * aSpec->surface->pitchinpix], &aSource[i * aPixelPitch], aWidth * sizeof(pixelType));
			}
		}

		template<uint32_t pMask, typename pType, typename sType>
		inline void __attribute((always_inline))		BlitPalette					(EmulateSpecStruct* aSpec, const pType* aPalette, const sType* aSource, uint32_t aWidth, uint32_t aHeight, uint32_t aPixelPitch)
		{
			for(int i = 0; i != aHeight; i ++)
			{
				for(int j = 0; j != aWidth; j ++)
				{
					aSpec->surface->pixels[i * aSpec->surface->pitchinpix + j] = aPalette[aSource[i * aPixelPitch + j] & pMask];
				}
			}
		}
	}

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

#endif

