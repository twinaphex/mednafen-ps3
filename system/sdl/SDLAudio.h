#ifndef SDLAUDIO_H
#define	SDLAUDIO_H

class								SDLAudio : public ESAudio
{
	public:	
									SDLAudio				();
									~SDLAudio				();
									
		void						AddSamples				(uint32_t* aSamples, uint32_t aCount);
		volatile int32_t			GetBufferFree			();

	protected:
		void						GetSamples				(uint32_t* aSamples, uint32_t aCount);
		volatile int32_t			GetBufferAmount			();
		static void					ProcessAudioCallback	(void *userdata, Uint8 *stream, int len);

		static const int			BlockCount = 16;
		static const int			BufferSize = 8192;
		static const int			BufferMask = 0x1FFF;

		SDL_AudioSpec				Format;
	
		uint32_t 					RingBuffer[BufferSize];
		volatile int32_t			ReadCount;
		volatile int32_t			WriteCount;
};

#endif
