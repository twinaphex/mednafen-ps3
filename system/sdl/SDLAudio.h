#pragma once

#include "src/utility/AudioBuffer.h"

class								ESAudio
{
	public:	
		static void					Initialize				();
		static void					Shutdown				();
									
		static void					AddSamples				(const uint32_t* aSamples, uint32_t aCount) {while(Buffer.GetBufferFree() < aCount) Semaphore->Wait(); SDL_LockAudio(); Buffer.WriteData(aSamples, aCount); SDL_UnlockAudio();};
		static volatile int32_t		GetBufferAmount			() {return Buffer.GetBufferAmount();}
		static volatile int32_t		GetBufferFree			() {return Buffer.GetBufferFree();}
		static void					SetSpeed				(uint32_t aSpeed) {Buffer.SetSpeed(aSpeed);}

	protected:
		static void					ProcessAudioCallback	(void *userdata, Uint8 *stream, int len);

		static const int			BlockCount = 16;
		static SDL_AudioSpec		Format;

		static AudioBuffer<>		Buffer;
		static ESSemaphore*			Semaphore;
};

