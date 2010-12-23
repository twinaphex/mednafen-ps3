#include <ps3_system.h>

void					PS3Audio::Init					()
{
	SDL_AudioSpec spec;
	spec.freq = 48000;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 2048;
	spec.callback = ProcessAudioCallback;

	SDL_OpenAudio(&spec, &Format);
	SDL_PauseAudio(0);
}


void					PS3Audio::Quit					()
{
	SDL_CloseAudio();
}

void					PS3Audio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	SDL_LockAudio();

//	if(BufferSize - GetBufferAmount() < aCount)
//	{
//		for(int i = 0; i != 10 && BufferSize - GetBufferAmount() < aCount; i ++)
//		{
//			Utility::Sleep(1);
//		}
//	}
	
	if(BufferSize - GetBufferAmount() < aCount)
	{
//		throw "PS3Audio::AddSamples: Unresolved buffer overflow, did the audio thread hang?";
//		printf("PS3Audio::AddSamples: Dropped %d samples: POP\n", aCount);
		return;
	}

	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
	
	SDL_UnlockAudio();
}

void					PS3Audio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
//	if(GetBufferAmount() < aCount)
//	{
//		for(int i = 0; i != 10 && GetBufferAmount() < aCount; i ++)
//		{
//			Utility::Sleep(1);
//		}
//	}

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

void					PS3Audio::ProcessAudioCallback	(void *userdata, Uint8 *stream, int len)
{
	uint32_t* buffer = (uint32_t*)stream;
	
	uint32_t samps[8192];
	
	GetSamples(samps, len / 4);
	
	for(int i = 0; i != len / 4; i ++)
	{
		buffer[i] = samps[i];
	}
}

volatile int32_t 		PS3Audio::GetBufferAmount		()
{
	if(InitialFill == false)
	{
		if((WriteCount - ReadCount) < 2400)
		{
			return 0;
		}
		
		InitialFill = true;
	}

	return (WriteCount - ReadCount);
}

volatile int32_t 		PS3Audio::GetBufferFree			()
{
	return BufferSize - (WriteCount - ReadCount);
}


SDL_AudioSpec			PS3Audio::Format;
uint32_t 				PS3Audio::RingBuffer[BufferSize];
int32_t 				PS3Audio::ReadCount = 0;
int32_t 				PS3Audio::WriteCount = 0;
uint32_t				PS3Audio::NextBlock = 0;
bool					PS3Audio::InitialFill = false;