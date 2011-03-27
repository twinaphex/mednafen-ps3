#include <es_system.h>

extern "C" int audioAddData(u32 portNum, float *data, u32 frames, float volume);

						L1ghtAudio::L1ghtAudio			()
{
	ReadCount = 0;
	WriteCount = 0;
	InitialFill = false;

	audioInit();
	
	audioPortParam portparam = {2, BlockCount, 0, 0};
	audioPortOpen(&portparam, &Port);

	audioGetPortConfig(Port, &Config);

	memset(RingBuffer, 0, sizeof(RingBuffer));

	audioCreateNotifyEventQueue(&QueueID, &QueueKey);
	audioSetNotifyEventQueue(QueueKey);
	sysEventQueueDrain(QueueID);

//TODO: Fix this
	sys_lwmutex_attr_t MutexAttrs = {SYS_LWMUTEX_ATTR_PROTOCOL, SYS_LWMUTEX_ATTR_RECURSIVE, '\0'};
	sysLwMutexCreate(&BufferMutex, &MutexAttrs);

	audioPortStart(Port);
	
	ThreadDie = false;
	sysThreadCreate(&ThreadID, ProcessAudioThread, 0, 0, 65536, 0, 0);
}


						L1ghtAudio::~L1ghtAudio			()
{
	ThreadDie = true;

	for(int i = 0; i != 10 && ThreadDie; i ++)
	{
		Utility::Sleep(100);
	}

	audioPortStop(Port);
	audioPortClose(Port);
	audioQuit();

	sysLwMutexDestroy(&BufferMutex);
}

void					L1ghtAudio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	while(GetBufferFree() < aCount)
	{
		Utility::Sleep(0);
	}

	sysLwMutexLock(&BufferMutex, 0);

	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}

	sysLwMutexUnlock(&BufferMutex);
}

void				L1ghtAudio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	sysLwMutexLock(&BufferMutex, 0);

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

	sysLwMutexUnlock(&BufferMutex);
}

void					L1ghtAudio::ProcessAudioThread	(void* aBcD)
{
	while(!es_audio)
	{
		Utility::Sleep(1);
	}

	L1ghtAudio* audio = (L1ghtAudio*)es_audio;

	int16_t samples[1024];
	float outbuffer[512];

	sys_event_t event;

	while(!audio->ThreadDie)
	{
		if(0 != sysEventQueueReceive(audio->QueueID, &event, 0))
		{
			break;
		}

		for(int i = 0; i != 512; i ++)
		{
			outbuffer[i] = 0.0f;
		}

		audio->GetSamples((uint32_t*)samples, 256);

		for(uint32_t i = 0; i != 256 * 2; i ++)
		{
			outbuffer[i] = ((float)samples[i]) / 32768.0f;
		}

		audioAddData(audio->Port, outbuffer, 256, 1);
	}
	
	audio->ThreadDie = false;
	sysThreadExit(0);
}

volatile int32_t 		L1ghtAudio::GetBufferAmount		()
{
	return (WriteCount - ReadCount);
}

volatile int32_t 		L1ghtAudio::GetBufferFree		()
{
	return BufferSize - (WriteCount - ReadCount);
}

