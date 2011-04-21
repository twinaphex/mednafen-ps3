#include <es_system.h>

						WiiAudio::WiiAudio				() :
	BufferIndex(0),
	BufferMutex(0)
{
	for(int i = 0; i != 2; i ++)
	{
		Buffer[i] = (uint32_t*)memalign(32, SegmentSize * 4);
		memset(Buffer[i], 0, SegmentSize * 4);
		DCFlushRange(Buffer[i], SegmentSize * 4);
	}

	BufferIndex = 0;

	//Create the mutex
	LWP_MutexInit(&BufferMutex, false);

	//Setup audio engine
	AUDIO_Init(0);
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
	AUDIO_RegisterDMACallback(ProcessAudioCallback);
}


						WiiAudio::~WiiAudio				()
{
	AUDIO_StopDMA();

	LWP_MutexDestroy(BufferMutex);

	free(Buffer[0]);
	free(Buffer[1]);
}

void					WiiAudio::AddSamples			(const uint32_t* aSamples, uint32_t aCount)
{
	LWP_MutexLock(BufferMutex);
	InitDMA();
	RingBuffer.WriteData(aSamples, aCount);
	LWP_MutexUnlock(BufferMutex);
}

void					WiiAudio::InitDMA				()
{
	if(!AUDIO_GetDMAEnableFlag())
	{
		BufferIndex = 0;

		AUDIO_InitDMA((uint32_t)Buffer[1], SegmentSize);
		AUDIO_StartDMA();
	}
}

void					WiiAudio::ProcessAudioCallback	()
{
	WiiAudio* audio = (WiiAudio*)es_audio;

	LWP_MutexLock(audio->BufferMutex);
	audio->RingBuffer.ReadDataSilentUnderrun(audio->Buffer[audio->BufferIndex], SegmentSize);
	LWP_MutexUnlock(audio->BufferMutex);

	DCFlushRange(audio->Buffer[audio->BufferIndex], SegmentSize * 4);
	AUDIO_InitDMA((uint32_t)audio->Buffer[audio->BufferIndex], SegmentSize * 4);

	audio->BufferIndex = audio->BufferIndex == 0 ? 1 : 0;
}
