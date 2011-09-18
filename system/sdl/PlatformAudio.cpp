#include <es_system.h>
#include <SDL.h>
#include "src/utility/AudioBuffer.h"

namespace
{
	SDL_AudioSpec			Format;
	SoundTouchAudioBuffer	Buffer;
	ESSemaphore*			Semaphore;

	void					ProcessAudioCallback			(void *userdata, Uint8 *stream, int len)
	{
		Buffer.ReadDataSilentUnderrun((uint32_t*)stream, len / 4);

		if(!Semaphore->GetValue())
		{
			Semaphore->Post();
		}
	}
}

#define Check(X)			if((X) != 0){printf("SDL_Audio Failed at %d\n", __LINE__); Shutdown(); return false;}

bool						ESAudio::Initialize				()
{
	//Setup audio format
	SDL_AudioSpec spec;
	spec.freq = 48000;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 2048;
	spec.callback = ProcessAudioCallback;

	//Start SDL audio
	Check(SDL_InitSubSystem(SDL_INIT_AUDIO));
	Check(SDL_OpenAudio(&spec, &Format));
	SDL_PauseAudio(0);

	Semaphore = ESThreads::MakeSemaphore(1);

	return true;
}

void						ESAudio::Shutdown				()
{
	SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	delete Semaphore;
}

void						ESAudio::AddSamples				(const uint32_t* aSamples, uint32_t aCount)
{
	while(Buffer.GetBufferFree() < aCount)
		Semaphore->Wait();

	SDL_LockAudio();
	Buffer.WriteData((uint32_t*)aSamples, aCount);
	SDL_UnlockAudio();
}

volatile int32_t			ESAudio::GetBufferAmount		()
{
	return Buffer.GetBufferAmount();
}

volatile int32_t			ESAudio::GetBufferFree			()
{
	return Buffer.GetBufferFree();
}

void						ESAudio::SetSpeed				(uint32_t aSpeed)
{
	Buffer.SetSpeed(aSpeed);
}

