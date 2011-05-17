#include <es_system.h>

						DorkAudio::DorkAudio			() : 
	Thread(0),
	Semaphore(0),
	ThreadDie(false),
	QueueID(0),
	QueueKey(0),
	Port(0)
{
	cellAudioInit();

	CellAudioPortParam portparam = {2, BlockCount, CELL_AUDIO_PORTATTR_BGM, 1};
	cellAudioPortOpen(&portparam, &Port);

	cellAudioGetPortConfig(Port, &Config);

	cellAudioCreateNotifyEventQueue(&QueueID, &QueueKey);
	cellAudioSetNotifyEventQueue(QueueKey);
	sys_event_queue_drain(QueueID);

	cellAudioPortStart(Port);
	
	Thread = es_threads->MakeThread(ProcessAudioThread, this);
	Semaphore = es_threads->MakeSemaphore(1);
}


						DorkAudio::~DorkAudio			()
{
	cellAudioPortStop(Port);

	ThreadDie = true;
	delete Thread;
	delete Semaphore;

	cellAudioPortClose(Port);
	cellAudioRemoveNotifyEventQueue(QueueKey);
	sys_event_queue_destroy(QueueID, SYS_EVENT_QUEUE_DESTROY_FORCE);
	cellAudioQuit();
}

void					DorkAudio::AddSamples			(const uint32_t* aSamples, uint32_t aCount)
{
	while(RingBuffer.GetBufferFree() < aCount)
	{
		Semaphore->Wait();
	}

	RingBuffer.WriteData(aSamples, aCount);
}

int						DorkAudio::ProcessAudioThread	(void* aAudio)
{
	DorkAudio* audio = (DorkAudio*)aAudio;

	int16_t samples[1024];
	float outbuffer[512];

	sys_event_t event;

	while(!audio->ThreadDie)
	{
		if(0 != sys_event_queue_receive(audio->QueueID, &event, 100 * 1000))
		{
			break;
		}

		audio->RingBuffer.ReadDataSilentUnderrun((uint32_t*)samples, 256);

		if(!audio->Semaphore->GetValue())
		{
			audio->Semaphore->Post();
		}

		for(uint32_t i = 0; i != 256 * 2; i ++)
		{
			outbuffer[i] = ((float)samples[i]) / 32768.0f;
		}

		cellAudioAddData(audio->Port, outbuffer, 256, 1);
	}
	
	audio->ThreadDie = false;
	return 0;
}
