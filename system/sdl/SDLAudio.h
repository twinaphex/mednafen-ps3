#pragma once

#include "src/utility/AudioBuffer.h"

namespace	soundtouch
{
	class								SoundTouch;
}

class									ESAudio
{
	public:	
		static void						Initialize				();
		static void						Shutdown				();

		static void						AddSamples				(const uint32_t* aSamples, uint32_t aCount);
		static volatile int32_t			GetBufferAmount			() {return Buffer.GetBufferAmount();}
		static volatile int32_t			GetBufferFree			() {return Buffer.GetBufferFree();}
		static void						SetSpeed				(uint32_t aSpeed);

	protected:
		static void						ProcessAudioCallback	(void *userdata, Uint8 *stream, int len);

		static SDL_AudioSpec			Format;

		static AudioBuffer<>			Buffer;
		static ESSemaphore*				Semaphore;

		static soundtouch::SoundTouch	PitchShifter;
		static uint32_t					AuxBuffer[48000];
		static uint32_t					Speed;
};

