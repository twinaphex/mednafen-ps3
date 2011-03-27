#pragma once

#include "src/utility/AudioBuffer.h"

class								WiiAudio : public ESAudio
{
	public:	
									WiiAudio				();
									~WiiAudio				();

		void						AddSamples				(const uint32_t* aSamples, uint32_t aCount);
		volatile int32_t			GetBufferFree			() const {return RingBuffer.GetBufferAmount();};
		volatile int32_t			GetBufferAmount			() const {return RingBuffer.GetBufferFree();};

	protected:
		void						InitDMA					();
		static void					ProcessAudioCallback	();

		static const int			SegmentSize = 800;

		uint32_t*					Buffer[2];

		AudioBuffer<>				RingBuffer;
		uint32_t					BufferIndex;
		mutex_t						BufferMutex;

};
