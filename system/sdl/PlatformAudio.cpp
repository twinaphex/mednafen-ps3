#include <es_system.h>
#include <SoundTouch.h>
#include "src/utility/AudioBuffer.h"

namespace
{
	SDL_AudioSpec			Format;
	AudioBuffer<>			Buffer;
	ESSemaphore*			Semaphore;

	soundtouch::SoundTouch	PitchShifter;
	uint32_t				AuxBuffer[48000];
	uint32_t				Speed = 1;

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
	SDL_AudioSpec spec;
	spec.freq = 48000;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 2048;
	spec.callback = ProcessAudioCallback;

	SDL_OpenAudio(&spec, &Format);
	SDL_PauseAudio(0);

	Semaphore = ESThreads::MakeSemaphore(1);

	//Soundtouch setup
	PitchShifter.setSampleRate(48000);
	PitchShifter.setChannels(2);
}

void						ESAudio::Shutdown				()
{
	SDL_CloseAudio();
	delete Semaphore;
}

void						ESAudio::AddSamples				(const uint32_t* aSamples, uint32_t aCount)
{
	//Pitch shifting
	if(Speed != 1)
	{
		//Add samples to sound touch
		PitchShifter.putSamples((int16_t*)aSamples, aCount);

		//Process any available samples 
		int32_t sampleCount = PitchShifter.numSamples();

		if(sampleCount)
		{
			//Don't process too large of a block
			sampleCount = PitchShifter.receiveSamples((int16_t*)AuxBuffer, std::min(2048, sampleCount));

			//Put them in the ringbuffer as normal
			while(Buffer.GetBufferFree() < sampleCount)
				Semaphore->Wait();

			SDL_LockAudio();
			Buffer.WriteData((uint32_t*)AuxBuffer, sampleCount);
			SDL_UnlockAudio();
		}
	}
	else
	{
		//Put them in the ringbuffer as normal
		while(Buffer.GetBufferFree() < aCount)
			Semaphore->Wait();

		SDL_LockAudio();
		Buffer.WriteData((uint32_t*)aSamples, aCount);
		SDL_UnlockAudio();
	}
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
	Speed = aSpeed;
	PitchShifter.setTempo(aSpeed);
}

