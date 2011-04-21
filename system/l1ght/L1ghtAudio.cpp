#include <es_system.h>

extern "C" int audioAddData(u32 portNum, float *data, u32 frames, float volume);

						L1ghtAudio::L1ghtAudio			() : 
	ThreadID(0),
	ThreadDie(false),
	QueueID(0),
	QueueKey(0),
	Port(0)
{
	audioInit();
	
	audioPortParam portparam = {2, BlockCount, 0, 0};
	audioPortOpen(&portparam, &Port);

	audioGetPortConfig(Port, &Config);

	audioCreateNotifyEventQueue(&QueueID, &QueueKey);
	audioSetNotifyEventQueue(QueueKey);
	sysEventQueueDrain(QueueID);

//TODO: Fix this
	sys_lwmutex_attr_t MutexAttrs = {SYS_LWMUTEX_ATTR_PROTOCOL, SYS_LWMUTEX_ATTR_RECURSIVE, '\0'};
	sysLwMutexCreate(&BufferMutex, &MutexAttrs);

	audioPortStart(Port);
	
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

void					L1ghtAudio::AddSamples			(const uint32_t* aSamples, uint32_t aCount)
{
	sysLwMutexLock(&BufferMutex, 0);
	RingBuffer.WriteData(aSamples, aCount);
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

		sysLwMutexLock(&audio->BufferMutex, 0);
		audio->RingBuffer.ReadDataSilentUnderrun((uint32_t*)samples, 256);
		sysLwMutexUnlock(&audio->BufferMutex);

		for(uint32_t i = 0; i != 256 * 2; i ++)
		{
			outbuffer[i] = ((float)samples[i]) / 32768.0f;
		}

		audioAddData(audio->Port, outbuffer, 256, 1);
	}
	
	audio->ThreadDie = false;
	sysThreadExit(0);
}
