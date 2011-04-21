#ifndef MDFN_GMBT_H__
#define MDFN_GMBT_H__

#include <src/mednafen.h>
#include <src/snes/src/lib/libco/libco.h>
#include "gambatte.h"
#include "resample/resamplerinfo.h"
#include <sstream>
using namespace Gambatte;

namespace mdfngmbt
{
	extern EmulateSpecStruct*	ESpec;
	extern bool					GameLoaded;
	extern bool					NeedToClearFrameBuffer;

	class						GameBoy;

	extern cothread_t			MainThread;
	extern GameBoy				*SideA, *SideB;

	extern uint8_t*				InputPort[2];
	extern uint8_t*				ROMData;
	extern uint32_t				ROMSize;


	class gbblitter : public VideoBlitter
	{
		public:
								gbblitter				()			{buffer.pixels = 0;}
								~gbblitter				()			{delete[] (uint32_t*)buffer.pixels;}
			const PixelBuffer	inBuffer				()			{return buffer;}
			void				setSide					(uint32_t a){side = a;}


			void				setBufferDimensions		(unsigned aWidth, unsigned aHeight)
			{
				delete[] (uint32_t*)buffer.pixels;
				buffer.pixels = new uint32_t[aWidth * aHeight];

				width = aWidth;
				height = aHeight;
				buffer.pitch = aWidth;
			}


			void				blit					()
			{
				uint32_t* sourceptr = (uint32_t*)buffer.pixels;

				for(int i = 0; i != height; i ++)
				{
					for(int j = 0; j != width ; j ++)
					{
						if(!SideB)
						{
							ESpec->surface->pixels[i * ESpec->surface->pitch32 + j] = sourceptr[i * width + j];
						}
						else
						{
							ESpec->surface->pixels[(i + (144 / 2)) * ESpec->surface->pitch32 + j + (side ? 160 : 0)] = sourceptr[i * width + j];
						}
					}
				}
			}

		protected:
			uint32_t			width, height;
			uint32_t			side;
			PixelBuffer			buffer;
	};


	class gbinput : public InputStateGetter
	{
		public:
			const InputState& 		operator()			()	{return inputs;};

			InputState				inputs;
	};

	class							GameBoy
	{
		public:
									GameBoy				(const void* aROM, uint32_t aSize, uint32_t aSide)
			{
				Side = aSide;

				Gambatte = new GB();
				Resample = ResamplerInfo::get(0).create(2097152, 48000, 35112);

				SampleOverflow = 0;
				memset(Samples, 0, sizeof(Samples));
				memset(Resamples, 0, sizeof(Resamples));

				std::istringstream file(std::string((const char*)aROM, (size_t)aSize), std::ios_base::in | std::ios_base::binary);	
				if(Gambatte->load(file, MDFN_GetSettingB("gmbt.forcedmg")))
				{
					delete Resample;
					delete Gambatte;

					MDFND_PrintError("gambatte: Failed to load ROM");
					throw 0;
				}

				Blitter.setSide(aSide);

				Gambatte->setVideoBlitter(&Blitter);
				Gambatte->setInputStateGetter(&Input);
				Gambatte->setSide(aSide);

				CycleCounter = 0;

				Thread = co_create(65536 * sizeof(void*), aSide ? ThreadB : ThreadA);

				ShiftOutByte = 0xFF;
				ShiftOutClock = 0;
				ShiftOutValid = false;
				ShiftOutWait = false;
			}

									~GameBoy			()
			{
				delete Resample;
				delete Gambatte;

				co_delete(Thread);
			}

			static void				ThreadA				();
			static void				ThreadB				();

			unsigned long			CycleCounter;

			unsigned char			ShiftOutByte;
			unsigned long			ShiftOutClock;
			bool					ShiftOutValid;
			bool					ShiftOutWait;

			GB*						Gambatte;
			Resampler*				Resample;
			gbblitter				Blitter;
			gbinput					Input;
			uint32_t				Side;
			bool					Done;

			cothread_t				Thread;

			uint32_t				Samples[48000];
			uint32_t				Resamples[48000];
			int32_t					SampleOverflow;
	};
}

#endif
