#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "AudioBuffer.h"

#ifdef ES_USESOUNDTOUCH
#include <SoundTouch.h>

							SoundTouchAudioBuffer::SoundTouchAudioBuffer		() : PitchShifter(new soundtouch::SoundTouch())
{
	PitchShifter->setSampleRate(48000);
	PitchShifter->setChannels(2);
}

							SoundTouchAudioBuffer::~SoundTouchAudioBuffer		()
{
	delete PitchShifter;
}


void						SoundTouchAudioBuffer::SetSpeed						(uint32_t aSpeed)
{
	//Evacuate buffer
	if(aSpeed == 1)
	{
		PitchShifter->clear();
	}

	Speed = aSpeed;
	PitchShifter->setTempo(aSpeed);
}

uint32_t					SoundTouchAudioBuffer::WriteData					(const uint32_t* aData, uint32_t aLength)
{
	//Pitch shifting
	if(Speed != 1)
	{
		PitchShifter->putSamples((int16_t*)aData, aLength);
		int32_t sampleCount = PitchShifter->numSamples();

		if(sampleCount)
		{
			sampleCount = PitchShifter->receiveSamples((int16_t*)AuxBuffer, std::min(2048, sampleCount));
			AudioBuffer::WriteData((uint32_t*)AuxBuffer, sampleCount);
		}
	}
	else
	{
		AudioBuffer::WriteData(aData, aLength);
	}
}
#endif
