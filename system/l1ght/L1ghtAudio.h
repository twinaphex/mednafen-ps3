#ifndef L1GHTAUDIO_H
#define	L1GHTAUDIO_H

class								L1ghtAudio : public ESAudio
{
	public:	
									L1ghtAudio				();
									~L1ghtAudio				();

		void						AddSamples				(uint32_t* aSamples, uint32_t aCount);
		void						GetSamples				(uint32_t* aSamples, uint32_t aCount);

	protected:
		volatile int32_t			GetBufferAmount			();
		volatile int32_t			GetBufferFree			();
		
		static void					ProcessAudioThread		(uint64_t aBcD);
	
		static const int			BlockCount = 16;
		static const int			BufferSize = 8192;
		static const int			BufferMask = 0x1FFF;

		sys_ppu_thread_t			ThreadID;	
		bool						ThreadDie;
	
		uint32_t					Port;
		AudioPortConfig				Config;

		uint32_t 					RingBuffer[BufferSize];
		int32_t 					ReadCount;
		int32_t 					WriteCount;
		uint32_t					NextBlock;
		bool						InitialFill;
		
};

#endif
