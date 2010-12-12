#ifndef PS3AUDIO_H
#define	PS3AUDIO_H

class							PS3Audio
{
	public:	
		static void					Init					();
		static void					Quit					();

		static void					AddSamples				(uint32_t* aSamples, uint32_t aCount);
		static void					GetSamples				(uint32_t* aSamples, uint32_t aCount);

		static void					Block					();

		static volatile uint32_t	GetBufferAmount			();
		
		static void					ProcessAudioThread		(uint64_t aBcD);

	protected:
		static const int			BlockCount = 16;
		static const int			BufferSize = 4096;
		static const int			BufferMask = 0xFFF;

		static sys_ppu_thread_t		ThreadID;	
		static bool					ThreadDie;
	
		static uint32_t				Port;
		static AudioPortConfig		Config;

		static uint32_t 			RingBuffer[BufferSize];
		static uint32_t 			ReadCount;
		static uint32_t 			WriteCount;
		static uint32_t				NextBlock;
};

#endif
