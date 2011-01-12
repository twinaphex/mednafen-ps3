#include <ps3_system.h>

						L1ghtAudio::L1ghtAudio			()
{
	ReadCount = 0;
	WriteCount = 0;
	InitialFill = false;

	audioInit();
	
	AudioPortParam portparam = {2, BlockCount, 0, 0};
	audioPortOpen(&portparam, &Port);

	audioGetPortConfig(Port, &Config);

	memset(RingBuffer, 0, sizeof(RingBuffer));

	audioCreateNotifyEventQueue(&QueueID, &QueueKey);
	audioSetNotifyEventQueue(QueueKey);
	sys_event_queue_drain(QueueID);

	sys_mutex_attribute_t MutexAttrs = {MUTEX_PROTOCOL_FIFO, MUTEX_NOT_RECURSIVE, 0, 0, 0, 0, 0, '\0'};
	sys_mutex_create(&BufferMutex, &MutexAttrs);

	audioPortStart(Port);
	
	ThreadDie = false;
	sys_ppu_thread_create(&ThreadID, ProcessAudioThread, 0, 0, 65536, 0, 0);
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

	sys_mutex_destroy(BufferMutex);
}

void					L1ghtAudio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	sys_mutex_lock(BufferMutex, 0);

	uint32_t total = GetBufferFree();
	total = total > aCount ? aCount : total;

	for(int i = 0; i != total; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}

	if(total != aCount)
	{
		printf("AddSamples: OVERRUN: Dropped %d samples\n", aCount - total);
	}

	sys_mutex_unlock(BufferMutex);
}

uint32_t				L1ghtAudio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	sys_mutex_lock(BufferMutex, 0);

	uint32_t total = GetBufferAmount();
	total = total > aCount ? aCount : total;

	for(int i = 0; i != total; i ++, ReadCount ++)
	{
		aSamples[i] = RingBuffer[ReadCount & BufferMask];
	}

	if(total != aCount)
	{
//		printf("AddSamples: UNDERRUN: Missed %d samples\n", aCount - total);
	}

	sys_mutex_unlock(BufferMutex);

	return total;
}

void					L1ghtAudio::ProcessAudioThread	(uint64_t aBcD)
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
		if(0 != sys_event_queue_receive(audio->QueueID, &event, 0))
		{
			break;
		}

		for(int i = 0; i != 512; i ++)
		{
			outbuffer[i] = 0.0f;
		}

		uint32_t samplecount = audio->GetSamples((uint32_t*)samples, 256);

		for(uint32_t i = 0; i != samplecount * 2; i ++)
		{
			outbuffer[i] = ((float)samples[i]) / 32768.0f;
		}

		audioAddData(audio->Port, outbuffer, 256, 1);
	}
	
	audio->ThreadDie = false;
	sys_ppu_thread_exit(0);
}

volatile int32_t 		L1ghtAudio::GetBufferAmount		()
{
	return (WriteCount - ReadCount);
}

volatile int32_t 		L1ghtAudio::GetBufferFree		()
{
	return BufferSize - (WriteCount - ReadCount);
}

