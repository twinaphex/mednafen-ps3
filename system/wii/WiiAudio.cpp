#include <es_system.h>

						WiiAudio::WiiAudio				()
{
	//Allocate 32byte aligned audio buffers
	for(int i = 0; i != 2; i ++)
	{
		Buffer[i] = (uint32_t*)memalign(32, SegmentSize * 4);
		memset(Buffer[i], 0, SegmentSize * 4);
		DCFlushRange(Buffer[i], SegmentSize * 4);
	}

	//Init some values
	ReadCount = 0;
	WriteCount = 0;
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
	//Stop audio DMA
	AUDIO_StopDMA();

	//Kill the mutex
	LWP_MutexDestroy(BufferMutex);

	//Free audio buffers
	free(Buffer[0]);
	free(Buffer[1]);
}

void					WiiAudio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	//HACK: Wait untill enough buffer is free to add the samples
	while(GetBufferFree() < aCount)
	{
		Utility::Sleep(0);
	}

	//Lock Ring Buffer
	LWP_MutexLock(BufferMutex);

	//Start Audio DMA, if nessicarry
	InitDMA();

	//Add samples
	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
	
	//Unlock Buffer
	LWP_MutexUnlock(BufferMutex);
}

void					WiiAudio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	//Lock audio buffer
	LWP_MutexLock(BufferMutex);

	//TODO: Fix this
	if(GetBufferAmount() < aCount)
	{
		//Would report, but inside menu this is hit all of the time
		memset(aSamples, 0, aCount * 4);
	}
	else
	{
		//Fetch Samples
		for(int i = 0; i != aCount; i ++, ReadCount ++)
		{
			aSamples[i] = RingBuffer[ReadCount & BufferMask];
		}
	}

	//Unlock audio buffer
	LWP_MutexUnlock(BufferMutex);	
}

void					WiiAudio::InitDMA				()
{
	//Do nothing if DMA is running
	if(!AUDIO_GetDMAEnableFlag())
	{
		//Set next buffer to zero
		BufferIndex = 0;

		//Start DMA
		AUDIO_InitDMA((uint32_t)Buffer[1], SegmentSize);
		AUDIO_StartDMA();
	}
}

void					WiiAudio::ProcessAudioCallback	()
{
	//Convert to WiiAudio*
	WiiAudio* audio = (WiiAudio*)es_audio;

	//Fetch samples into the next buffer
	audio->GetSamples(audio->Buffer[audio->BufferIndex], SegmentSize);
	DCFlushRange(audio->Buffer[audio->BufferIndex], SegmentSize * 4);

	//Send them to the DMA interface
	AUDIO_InitDMA((uint32_t)audio->Buffer[audio->BufferIndex], SegmentSize * 4);

	//Toggle buffer index
	audio->BufferIndex = audio->BufferIndex == 0 ? 1 : 0;
}
