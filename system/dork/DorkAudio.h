#pragma once

#include "src/utility/AudioBuffer.h"

class								DorkAudio : public ESAudio
{
	public:	
									DorkAudio				();
									~DorkAudio				();

		void						AddSamples				(const uint32_t* aSamples, uint32_t aCount);

		volatile int32_t			GetBufferFree			() const {return RingBuffer.GetBufferFree();};
		volatile int32_t			GetBufferAmount			() const {return RingBuffer.GetBufferAmount();};

	protected:
		static int					ProcessAudioThread		(void* aAudio);
	
		static const int			BlockCount = 16;

		ESThread*					Thread;
		ESMutex*					Mutex;
		volatile bool				ThreadDie;

		sys_event_queue_t			QueueID;
		sys_ipc_key_t				QueueKey;

		uint32_t					Port;
		CellAudioPortConfig			Config;

		AudioBuffer<>				RingBuffer;
};

