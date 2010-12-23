#include <ps3_system.h>

void					ESAudio::Init					()
{
	audioInit();
	
	AudioPortParam portparam = {2, BlockCount, 0, 0};
	audioPortOpen(&portparam, &Port);

	audioGetPortConfig(Port, &Config);

	memset(RingBuffer, 0, sizeof(RingBuffer));

	audioPortStart(Port);
	
	ThreadDie = false;
	sys_ppu_thread_create(&ThreadID, ProcessAudioThread, 0, 0, 65536, 0, 0);
}


void					ESAudio::Quit					()
{
	audioPortStop(Port);
	audioPortClose(Port);
	audioQuit();
}

void					ESAudio::AddSamples				(uint32_t* aSamples, uint32_t aCount)
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
//		throw "ESAudio::AddSamples: Unresolved buffer overflow, did the audio thread hang?";
		printf("ESAudio::AddSamples: Dropped %d samples: POP\n", aCount);
		return;
	}

	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
}

void					ESAudio::GetSamples				(uint32_t* aSamples, uint32_t aCount)
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

void					ESAudio::ProcessAudioThread		(uint64_t aBcD)
{
	int16_t samples[1024];
	uint32_t onblock = (*(volatile u64*)(u64)Config.readIndex) + 1;

	while(!ESAudio::ThreadDie)
	{
		onblock = (onblock + 1) % BlockCount;
		while(onblock == *(volatile u64*)(u64)Config.readIndex)
		{
			Utility::Sleep(1);
		}
	
		float* buf = (float*)(Config.audioDataStart + (onblock * sizeof(float) * 256 * 2));
		
		GetSamples((uint32_t*)samples, 256);

		for (uint32_t i = 0; i < 256 * 2; i ++)
		{
			buf[i] = ((float)samples[i]) / 32768.0f;
		}
	}
	
	ESAudio::ThreadDie = false;
	sys_ppu_thread_exit(0);
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

uint32_t				ESAudio::Port;
AudioPortConfig			ESAudio::Config;

sys_ppu_thread_t		ESAudio::ThreadID;	
bool					ESAudio::ThreadDie;

uint32_t 				ESAudio::RingBuffer[BufferSize];
int32_t 				ESAudio::ReadCount = 0;
int32_t 				ESAudio::WriteCount = 0;
uint32_t				ESAudio::NextBlock = 0;
bool					ESAudio::InitialFill = false;