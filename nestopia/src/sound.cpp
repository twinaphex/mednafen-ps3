#include <src/mednafen.h>

#include "mednafen.h"
#include "sound.h"

namespace nestMDFN
{
	//Locals
	Sound::Output 				EmuAudio;
	uint32_t					Samples[48000];
	uint32_t					SampleRate;

	void						SetupAudio							(uint32_t aSampleRate)
	{
		SampleRate = aSampleRate;

		EmuAudio.samples[0] = (void*)Samples;
		//TODO: Real fps here
		EmuAudio.length[0] = aSampleRate / 60;
		EmuAudio.samples[1] = NULL;
		EmuAudio.length[1] = 0;

		Sound(Nestopia).SetSampleBits(16);
		Sound(Nestopia).SetSampleRate(aSampleRate);
		Sound(Nestopia).SetVolume(Sound::ALL_CHANNELS, 100);
		Sound(Nestopia).SetSpeaker(Sound::SPEAKER_MONO);
	}

	Sound::Output*				GetAudio							()
	{
		return &EmuAudio;
	}

	void						CopyAudio							(void* aSoundBuffer, uint32_t aMaxSize, int32_t& aFrameCount)
	{
		//TODO: Use real fps
		if(aSoundBuffer && (aMaxSize > SampleRate / 60))
		{
			aFrameCount = SampleRate / 60;
			memcpy(aSoundBuffer, Samples, aFrameCount * 2);
		}
		else
		{
			aFrameCount = 0;
		}
	}
}


