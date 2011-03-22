#include <ps3_system.h>

						WiiAudio::WiiAudio				()
{
	SDL_AudioSpec spec;
	spec.freq = 48000;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 2048;
	spec.callback = ProcessAudioCallback;

	ReadCount = 0;
	WriteCount = 0;

	SDL_OpenAudio(&spec, &Format);
	SDL_PauseAudio(0);
}


						WiiAudio::~WiiAudio				()
{
	SDL_CloseAudio();
}

void					WiiAudio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	while(GetBufferFree() < aCount)
	{
		Utility::Sleep(0);
	}

	SDL_LockAudio();
	
	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
	
	SDL_UnlockAudio();
}

void					WiiAudio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	//TODO: Fix this
	if(GetBufferAmount() < aCount)
	{
		//Would report, but inside menu this is hit all of the time
		memset(aSamples, 0, aCount * 4);
	}
	else
	{
		for(int i = 0; i != aCount; i ++, ReadCount ++)
		{
			aSamples[i] = RingBuffer[ReadCount & BufferMask];
		}
	}
}

void					WiiAudio::ProcessAudioCallback	(void *userdata, Uint8 *stream, int len)
{
	if(es_audio)
	{
		uint32_t* buffer = (uint32_t*)stream;
	
		uint32_t samps[8192];
	
		((WiiAudio*)es_audio)->GetSamples(samps, len / 4);
	
		for(int i = 0; i != len / 4; i ++)
		{
			buffer[i] = samps[i];
		}
	}
}

volatile int32_t 		WiiAudio::GetBufferAmount		()
{
	return (WriteCount - ReadCount);
}

volatile int32_t 		WiiAudio::GetBufferFree			()
{
	return BufferSize - (WriteCount - ReadCount);
}

