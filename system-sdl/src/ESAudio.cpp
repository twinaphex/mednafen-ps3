#include <ps3_system.h>

void					ESAudio::Init					()
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


void					ESAudio::Quit					()
{
	SDL_CloseAudio();
}

void					ESAudio::AddSamples				(uint32_t* aSamples, uint32_t aCount)
{
	SDL_LockAudio();
	
	if(BufferSize - GetBufferAmount() < aCount)
	{
		return;
	}

	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
	
	SDL_UnlockAudio();
}

void					ESAudio::GetSamples				(uint32_t* aSamples, uint32_t aCount)
{
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

void					ESAudio::ProcessAudioCallback	(void *userdata, Uint8 *stream, int len)
{
	uint32_t* buffer = (uint32_t*)stream;
	
	uint32_t samps[8192];
	
	GetSamples(samps, len / 4);
	
	for(int i = 0; i != len / 4; i ++)
	{
		buffer[i] = samps[i];
	}
}

volatile int32_t 		ESAudio::GetBufferAmount		()
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

volatile int32_t 		ESAudio::GetBufferFree			()
{
	return BufferSize - (WriteCount - ReadCount);
}


SDL_AudioSpec			ESAudio::Format;
uint32_t 				ESAudio::RingBuffer[BufferSize];
int32_t 				ESAudio::ReadCount = 0;
int32_t 				ESAudio::WriteCount = 0;
uint32_t				ESAudio::NextBlock = 0;
bool					ESAudio::InitialFill = false;