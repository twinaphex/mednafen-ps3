#ifndef PS3AUDIO_H
#define	PS3AUDIO_H

class							PS3Audio
{
	public:	
		static void					Init					();
		static void					Quit					();

		static void					AddSamples				(uint32_t* aSamples, uint32_t aCount);
		static void					GetSamples				(uint32_t* aSamples, uint32_t aCount);

		static volatile int32_t		GetBufferAmount			();
		static volatile int32_t		GetBufferFree			();
		
		static void					ProcessAudioCallback	(void *userdata, Uint8 *stream, int len);

	protected:
		static SDL_AudioSpec		Format;
	
		static const int			BlockCount = 16;
		static const int			BufferSize = 8192;
		static const int			BufferMask = 0x1FFF;

		static uint32_t 			RingBuffer[BufferSize];
		static int32_t 				ReadCount;
		static int32_t 				WriteCount;
		static uint32_t				NextBlock;
		static bool					InitialFill;
		
};

#endif
