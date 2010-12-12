#include <ps3_system.h>

static int16_t samples[1024];

void					PS3Audio::Init					()
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


void					PS3Audio::Quit					()
{
	audioPortStop(Port);
	audioPortClose(Port);
	audioQuit();
}

void					PS3Audio::AddSamples			(uint32_t* aSamples, uint32_t aCount)
{
	for(int i = 0; i != aCount; i ++, WriteCount ++)
	{
		RingBuffer[WriteCount & BufferMask] = aSamples[i];
	}
}

void					PS3Audio::GetSamples			(uint32_t* aSamples, uint32_t aCount)
{
	for(int i = 0; i != aCount; i ++, ReadCount ++)
	{
		aSamples[i] = RingBuffer[ReadCount & BufferMask];
	}
}

bool					InitialFill = false;

void					PS3Audio::Block					()
{
}

void					PS3Audio::ProcessAudioThread	(uint64_t aBcD)
{
	while(!PS3Audio::ThreadDie)
	{
		uint32_t old_block = *(volatile u64*)(u64)Config.readIndex;
		uint32_t new_block = (old_block + 1) % BlockCount;
		float* buf = (float*)(Config.audioDataStart + (new_block * sizeof(float) * 256 * 2));
		
		if (GetBufferAmount() < 256)
		{
			for (uint32_t i = 0; i < 256 * 2; i ++)
			{
				buf[i] = 0.0f;
			}
		}
		else
		{
			GetSamples((uint32_t*)samples, 256);

			for (uint32_t i = 0; i < 256 * 2; i ++)
			{
				buf[i] = ((float)samples[i]) / 32768.0f;
			}
		}
		
		while(old_block == *(volatile u64*)(u64)Config.readIndex)
		{
			Utility::Sleep(1);
		}
	}
	
	PS3Audio::ThreadDie = false;
	sys_ppu_thread_exit(0);
}

volatile uint32_t 		PS3Audio::GetBufferAmount		()
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

uint32_t				PS3Audio::Port;
AudioPortConfig			PS3Audio::Config;



sys_ppu_thread_t		PS3Audio::ThreadID;	
bool					PS3Audio::ThreadDie;

uint32_t 				PS3Audio::RingBuffer[BufferSize];
uint32_t 				PS3Audio::ReadCount = 0;
uint32_t 				PS3Audio::WriteCount = 0;
uint32_t				PS3Audio::NextBlock = 0;
