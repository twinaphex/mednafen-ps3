#include <ps3_system.h>

						L1ghtAudio::L1ghtAudio			()
{
	ReadCount = 0;
	WriteCount = 0;
	NextBlock = 0;
	InitialFill = false;

	audioInit();
	
	AudioPortParam portparam = {2, BlockCount, 0, 0};
	audioPortOpen(&portparam, &Port);

	audioGetPortConfig(Port, &Config);

	memset(RingBuffer, 0, sizeof(RingBuffer));

	audioPortStart(Port);
	
	ThreadDie = false;
	sys_ppu_thread_create(&ThreadID, ProcessAudioThread, 0, 0, 65536, 0, 0);
}


						L1ghtAudio::~L1ghtAudio			()
{
	audioPortStop(Port);
	audioPortClose(Port);
	audioQuit();
}

void					L1ghtAudio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	if(BufferSize - GetBufferAmount() < aCount)
	{
		for(int i = 0; i != 10 && BufferSize - GetBufferAmount() < aCount; i ++)
		{
			Utility::Sleep(1);
		}
	}
	
	if(BufferSize - GetBufferAmount() < aCount)
	{
		printf("ESAudio::AddSamples: Dropped %d samples: POP\n", aCount);
		return;
	}

	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
}

void					L1ghtAudio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	if(GetBufferAmount() < aCount)
	{
		for(int i = 0; i != 10 && GetBufferAmount() < aCount; i ++)
		{
			Utility::Sleep(1);
		}
	}

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

void					L1ghtAudio::ProcessAudioThread	(uint64_t aBcD)
{
	while(!es_audio)
	{
		Utility::Sleep(1);
	}

	L1ghtAudio* audio = (L1ghtAudio*)es_audio;

	int16_t samples[1024];
	uint32_t onblock = (*(volatile u64*)(u64)audio->Config.readIndex) + 1;

	while(!audio->ThreadDie)
	{
		onblock = (onblock + 1) % L1ghtAudio::BlockCount;
		while(onblock == *(volatile u64*)(u64)audio->Config.readIndex)
		{
			Utility::Sleep(1);
		}
	
		float* buf = (float*)(audio->Config.audioDataStart + (onblock * sizeof(float) * 256 * 2));
		
		audio->GetSamples((uint32_t*)samples, 256);

		for (uint32_t i = 0; i < 256 * 2; i ++)
		{
			buf[i] = ((float)samples[i]) / 32768.0f;
		}
	}
	
	audio->ThreadDie = false;
	sys_ppu_thread_exit(0);
}

volatile int32_t 		L1ghtAudio::GetBufferAmount		()
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

volatile int32_t 		L1ghtAudio::GetBufferFree		()
{
	return BufferSize - (WriteCount - ReadCount);
}
