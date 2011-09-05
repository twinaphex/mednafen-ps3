#pragma once

#include <assert.h>
#include <stdlib.h>
#include <algorithm>

//If Length is not a power of two you are in for a world of hurt (MODULOS GALORE)
template<int Length=8192>
class								AudioBuffer
{
	public:
									AudioBuffer						() : ReadCount(0), WriteCount(0), InputSpeed(1) {}
		virtual						~AudioBuffer					() {}

		void						SetSpeed						(uint32_t aSpeed)
		{
			assert(aSpeed && aSpeed <= 16);
			InputSpeed = aSpeed;
		}

		uint32_t					WriteData						(const uint32_t* aData, uint32_t aLength)
		{
			if(InputSpeed == 1)
			{
				//Clip to available space (if you wan't to block, do it in the caller.)
				uint32_t free = GetBufferFree();
				aLength = (aLength > free) ? free : aLength;

				//Get the size of the copies
				uint32_t untilEnd = Length - (WriteCount % Length);
				uint32_t firstBlock = std::min(untilEnd, aLength);
				uint32_t secondBlock = (firstBlock == aLength) ? 0 : aLength - firstBlock;

				//Copy 1
				if(firstBlock)
				{
					memcpy(&RingBuffer[WriteCount % Length], aData, firstBlock * 4);
				}

				//Copy 2
				if(secondBlock)
				{
					memcpy(&RingBuffer[0], &aData[firstBlock], secondBlock * 4);
				}

				//Done
				WriteCount += aLength;
			}
			else
			{
				assert(InputSpeed);

				//Clip to available space (if you wan't to block, do it in the caller.)
				uint32_t free = GetBufferFree();
				aLength = (aLength > free) ? free : aLength;

				//Copy
				for(int i = 0; i < aLength; i += InputSpeed, WriteCount ++)
				{
					RingBuffer[WriteCount % Length] = aData[i];
				}
			}

			return aLength;
		}

		uint32_t					ReadData						(uint32_t* aData, uint32_t aLength)
		{
			//Clip to available space (if you wan't to block, do it in the caller.)
			uint32_t available = GetBufferAmount();
			aLength = (aLength > available) ? available : aLength;

			//Get the size of the copies
			uint32_t untilEnd = Length - (ReadCount % Length);
			uint32_t firstBlock = std::min(untilEnd, aLength);
			uint32_t secondBlock = (firstBlock == aLength) ? 0 : aLength - firstBlock;

			//Copy 1
			if(firstBlock)
			{
				memcpy(aData, &RingBuffer[ReadCount % Length], firstBlock * 4);
			}

			//Copy 2
			if(secondBlock)
			{
				memcpy(&aData[firstBlock], &RingBuffer[0], secondBlock * 4);
			}

			//Done
			ReadCount += aLength;

			return aLength;
		}

		uint32_t					ReadDataSilentUnderrun			(uint32_t* aData, uint32_t aLength)
		{
			uint32_t count = ReadData(aData, aLength);

			if(count < aLength)
			{
				memset(&aData[count], 0, (aLength - count) * 4);
			}

			return count;
		}

		volatile int32_t		 	GetBufferAmount					() const {return (WriteCount - ReadCount);}
		volatile int32_t		 	GetBufferFree					() const {return Length - (WriteCount - ReadCount);}

	private:
		uint32_t					RingBuffer[Length];

		uint32_t					ReadCount;
		uint32_t					WriteCount;

		uint32_t					InputSpeed;
};

#ifdef ES_USESOUNDTOUCH
namespace							soundtouch
{
	class							SoundTouch;
};

//Note that the methods in this class are NOT virtual.
class								SoundTouchAudioBuffer : public AudioBuffer<8192>
{
	public:
									SoundTouchAudioBuffer			();
									~SoundTouchAudioBuffer			();

		void						SetSpeed						(uint32_t aSpeed);
		uint32_t					WriteData						(const uint32_t* aData, uint32_t aLength);

	private:
		soundtouch::SoundTouch*		PitchShifter;
		uint32_t					AuxBuffer[48000];
		uint32_t					Speed;
};
#else
typedef								AudioBuffer<8192>				SoundTouchAudioBuffer;
#endif

