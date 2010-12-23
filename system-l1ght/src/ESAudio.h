#ifndef ESAUDIO_H
#define	ESAUDIO_H

class							ESAudio
{
	public:	
		static void					Init					();
		static void					Quit					();

		static void					AddSamples				(uint32_t* aSamples, uint32_t aCount);
		static void					GetSamples				(uint32_t* aSamples, uint32_t aCount);

		static volatile int32_t		GetBufferAmount			();
		static volatile int32_t		GetBufferFree			();
		
		static void					ProcessAudioThread		(uint64_t aBcD);

	protected:
		static const int			BlockCount = 16;
		static const int			BufferSize = 8192;
		static const int			BufferMask = 0x1FFF;

		static sys_ppu_thread_t		ThreadID;	
		static bool					ThreadDie;
	
		static uint32_t				Port;
		static AudioPortConfig		Config;

		static uint32_t 			RingBuffer[BufferSize];
		static int32_t 				ReadCount;
		static int32_t 				WriteCount;
		static uint32_t				NextBlock;
		static bool					InitialFill;
		
};

#endif
