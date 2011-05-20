#pragma once

#include "src/utility/AudioBuffer.h"

class								DorkAudio : public ESAudio
{
	public:	
									DorkAudio				();
									~DorkAudio				();

		void						AddSamples				(const uint32_t* aSamples, uint32_t aCount) {while(RingBuffer.GetBufferFree() < aCount) Semaphore->Wait(); RingBuffer.WriteData(aSamples, aCount);};

		volatile int32_t			GetBufferFree			() const {return RingBuffer.GetBufferFree();};
		volatile int32_t			GetBufferAmount			() const {return RingBuffer.GetBufferAmount();};

	protected:
		static int					ProcessAudioThread		(void* aAudio);
		static void					MultiStreamCallback		(int streamNumber, void* userData, int cType, void * pWriteBuffer, int nBufferSize);
	
		static const int			BlockCount = 16;

		ESThread*					Thread;
		ESSemaphore*				Semaphore;
		volatile bool				ThreadDie;

		int32_t						MSChannel;
		void*						MSMemory;
		void*						MSBuffers[2];
		volatile bool				StreamDead;

		uint32_t					Port;
		CellAudioPortConfig			Config;

		AudioBuffer<>				RingBuffer;
};

