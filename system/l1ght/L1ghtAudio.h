#pragma once

#include "src/utility/AudioBuffer.h"

class								L1ghtAudio : public ESAudio
{
	public:	
									L1ghtAudio				();
									~L1ghtAudio				();

		void						AddSamples				(const uint32_t* aSamples, uint32_t aCount);

		volatile int32_t			GetBufferFree			() const {return RingBuffer.GetBufferAmount();};
		volatile int32_t			GetBufferAmount			() const {return RingBuffer.GetBufferFree();};

	protected:
		static void					ProcessAudioThread		(void* aBcD);
	
		static const int			BlockCount = 16;

		sys_ppu_thread_t			ThreadID;
		volatile bool				ThreadDie;

		sys_event_queue_t			QueueID;
		uint64_t					QueueKey;

		sys_lwmutex_t				BufferMutex;
	
		uint32_t					Port;
		audioPortConfig				Config;

		AudioBuffer<>				RingBuffer;
};

