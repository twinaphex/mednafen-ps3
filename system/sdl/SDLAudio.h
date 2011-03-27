#pragma once

#include "src/utility/AudioBuffer.h"

class								SDLAudio : public ESAudio
{
	public:	
									SDLAudio				();
									~SDLAudio				();
									
		void						AddSamples				(const uint32_t* aSamples, uint32_t aCount);
		volatile int32_t			GetBufferAmount			() const {return Buffer.GetBufferAmount();}
		volatile int32_t			GetBufferFree			() const {return Buffer.GetBufferFree();}

	protected:
		static void					ProcessAudioCallback	(void *userdata, Uint8 *stream, int len);

		static const int			BlockCount = 16;
		SDL_AudioSpec				Format;

		AudioBuffer<>				Buffer;
};
