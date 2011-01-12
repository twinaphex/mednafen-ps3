#ifndef L1GHTAUDIO_H
#define	L1GHTAUDIO_H

class								L1ghtAudio : public ESAudio
{
	public:	
									L1ghtAudio				();
									~L1ghtAudio				();

		void						AddSamples				(uint32_t* aSamples, uint32_t aCount);

	protected:
		uint32_t					GetSamples				(uint32_t* aSamples, uint32_t aCount);

		volatile int32_t			GetBufferAmount			();
		volatile int32_t			GetBufferFree			();
		
		static void					ProcessAudioThread		(uint64_t aBcD);
	
		static const int			BlockCount = 16;
		static const int			BufferSize = 8192;
		static const int			BufferMask = 0x1FFF;

		sys_ppu_thread_t			ThreadID;
		volatile bool				ThreadDie;

		sys_event_queue_t			QueueID;
		uint64_t					QueueKey;

		sys_mutex_t					BufferMutex;
	
		uint32_t					Port;
		AudioPortConfig				Config;

		uint32_t 					RingBuffer[BufferSize];
		volatile int32_t			ReadCount;
		volatile int32_t			WriteCount;
		bool						InitialFill;
		
};

#endif
