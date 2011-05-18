#include <es_system.h>

						DorkAudio::DorkAudio			() : 
	Thread(0),
	ThreadDie(false),
	MSChannel(-1),
	MSMemory(0),
	StreamDead(false),
	Port(0)
{
	/* Init audio */
	CellAudioPortParam portparam = {CELL_AUDIO_PORT_8CH, CELL_AUDIO_BLOCK_16, CELL_AUDIO_PORTATTR_BGM, 1};
	cellAudioInit();
	cellAudioPortOpen(&portparam, &Port);
	cellAudioGetPortConfig(Port, &Config);

	/* Setup multistream */
	cellMSSystemConfigureSysUtil();

	CellMSSystemConfig MultiStreamConfig = {4, 1, 0, CELL_MS_NOFLAGS};
	MSMemory = memalign(128, cellMSSystemGetNeededMemorySize(&MultiStreamConfig));
	cellMSSystemInitSPUThread(MSMemory, &MultiStreamConfig, 100);
	cellMSSystemConfigureLibAudio(&portparam, &Config);

	/* Setup volume */
	float volumes[64];
	for(int i = 0; i != 64; i ++)
	{
		volumes[i] = ((i % 8) == (i / 8)) ? 1.0f : 0.0f;
	}

	cellMSCoreSetVolume64(CELL_MS_BUS_FLAG | 1, CELL_MS_WET, volumes);
	cellMSCoreSetVolume64(CELL_MS_MASTER_BUS, CELL_MS_DRY, volumes);

	/* Create thread */
	Thread = es_threads->MakeThread(ProcessAudioThread, this);

	/* Setup audio stream */
	for(int i = 0; i != 2; i ++)
	{
		MSBuffers[i] = memalign(128, 2048);
		memset(MSBuffers[i], 0, 2048);
	}

	CellMSInfo StreamInfo = {CELL_MS_MASTER_BUS, MSBuffers[0], 2048, MSBuffers[1], 2048, 48000, 2, 0,
							CELL_MS_16BIT_BIG, CELL_MS_STREAM_NOFLAGS};

    MSChannel = cellMSStreamOpen();

    cellMSStreamSetInfo(MSChannel, &StreamInfo);
	cellMSStreamSetCallbackData(MSChannel, this);
    cellMSStreamSetCallbackFunc(MSChannel, MultiStreamCallback);
	cellMSCoreSetVolume1(MSChannel, CELL_MS_DRY, CELL_MS_SPEAKER_FL, CELL_MS_CHANNEL_0, 1.0f);
	cellMSCoreSetVolume1(MSChannel, CELL_MS_DRY, CELL_MS_SPEAKER_FR, CELL_MS_CHANNEL_1, 1.0f);
	cellMSStreamPlay(MSChannel);
}


						DorkAudio::~DorkAudio			()
{
	cellMSStreamSetSecondRead(MSChannel, 0, 0);
	while(!StreamDead); //?
	cellMSStreamClose(MSChannel);
	while(!(cellMSStreamGetStatus(MSChannel) & CELL_MS_STREAM_CLOSED));

	free(MSBuffers[0]);
	free(MSBuffers[1]);

	/* Kill thread */
	ThreadDie = true;
	delete Thread;

	/* Stop Multistream */
	cellMSSystemClose();
	free(MSMemory);

	/* Stop libaudio */
	cellAudioQuit();
}

void					DorkAudio::MultiStreamCallback	(int streamNumber, void* userData, int cType, void * pWriteBuffer, int nBufferSize)
{
	DorkAudio* audio = (DorkAudio*)userData;

	if((cType == CELL_MS_CALLBACK_MOREDATA))
	{
		audio->RingBuffer.ReadDataSilentUnderrun((uint32_t*)pWriteBuffer, nBufferSize / 4);
	}
	else if(cType == CELL_MS_CALLBACK_FINISHSTREAM)
	{
		audio->StreamDead = 1;
	}
}

int						DorkAudio::ProcessAudioThread	(void* aAudio)
{
	DorkAudio* audio = (DorkAudio*)aAudio;

	cellAudioPortStart(audio->Port);

	while(!audio->ThreadDie)
	{
		sys_timer_usleep(1000000 / 60 / 2);

		if(!audio->StreamDead)
		{
			cellMSSystemSignalSPU();
			cellMSSystemGenerateCallbacks();
		}
	}

	cellAudioPortStop(audio->Port);

	return 0;
}
