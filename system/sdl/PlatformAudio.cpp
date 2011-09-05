#include <es_system.h>
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

void						ESAudio::Initialize				()
{
	SDL_InitSubSystem(SDL_INIT_AUDIO);

	SDL_AudioSpec spec;
	spec.freq = 48000;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 2048;
	spec.callback = ProcessAudioCallback;

	SDL_OpenAudio(&spec, &Format);
	SDL_PauseAudio(0);

	Semaphore = ESThreads::MakeSemaphore(1);
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

