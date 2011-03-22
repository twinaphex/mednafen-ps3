#include <es_system.h>

						WiiAudio::WiiAudio				()
{
	assert(((SegmentSize * 4) % 32) == 0); //Segment size must be multiple of 32 bytes
	assert((((uint32_t)Buffer[0]) % 32) == 0); //Buffers must be 32 byte aligned
	assert((((uint32_t)Buffer[1]) % 32) == 0); //Buffers must be 32 byte aligned

	BufferIndex = 0;
	memset(Buffer[0], 0, sizeof(Buffer[0]));
	memset(Buffer[1], 0, sizeof(Buffer[1]));
	DCFlushRange(Buffer[0], sizeof(Buffer[0]));
	DCFlushRange(Buffer[1], sizeof(Buffer[1]));

	LWP_MutexInit(&BufferMutex, false);

	AUDIO_Init(0);
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
	AUDIO_RegisterDMACallback(ProcessAudioCallback);
	AUDIO_InitDMA((uint32_t)Buffer[1], SegmentSize);
	AUDIO_StartDMA();
}


						WiiAudio::~WiiAudio				()
{
	AUDIO_StopDMA();

	LWP_MutexDestroy(BufferMutex);
}

void					WiiAudio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	while(GetBufferFree() < aCount)
	{
		Utility::Sleep(0);
	}

	LWP_MutexLock(BufferMutex);
	
	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
	
	LWP_MutexUnlock(BufferMutex);
}

void					WiiAudio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	LWP_MutexLock(BufferMutex);

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

	LWP_MutexUnlock(BufferMutex);	
}

void					WiiAudio::ProcessAudioCallback	()
{
	if(es_audio)
	{
		WiiAudio* audio = (WiiAudio*)es_audio;

		audio->GetSamples(audio->Buffer[audio->BufferIndex], SegmentSize);
		DCFlushRange(audio->Buffer[audio->BufferIndex], SegmentSize * 4);

		AUDIO_InitDMA((uint32_t)audio->Buffer[audio->BufferIndex], SegmentSize * 4);
		audio->BufferIndex = audio->BufferIndex == 0 ? 1 : 0;
	}
}
