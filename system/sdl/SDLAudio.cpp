#include <ps3_system.h>

						SDLAudio::SDLAudio				()
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


						SDLAudio::~SDLAudio				()
{
	SDL_CloseAudio();
}

bool ini = 0;

void					SDLAudio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	ini = 1;

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

void					SDLAudio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	if(GetBufferAmount() < aCount)
	{
		//Would report, but inside menu this is hit all of the time
		if(ini) printf("underrun\n");
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

void					SDLAudio::ProcessAudioCallback	(void *userdata, Uint8 *stream, int len)
{
	if(es_audio)
	{
		uint32_t* buffer = (uint32_t*)stream;
	
		uint32_t samps[8192];
	
		((SDLAudio*)es_audio)->GetSamples(samps, len / 4);
	
		for(int i = 0; i != len / 4; i ++)
		{
			buffer[i] = samps[i];
		}
	}
}

volatile int32_t 		SDLAudio::GetBufferAmount		()
{
	return (WriteCount - ReadCount);
}

volatile int32_t 		SDLAudio::GetBufferFree			()
{
	return BufferSize - (WriteCount - ReadCount);
}

