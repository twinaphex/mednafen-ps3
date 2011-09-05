#include <es_system.h>
//#include <stdio.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include <string.h>

#include <AL/al.h>
#include <AL/alc.h>

//#include "src/ESAudio.h"
//#include "src/ESThreads.h"
#include "src/utility/AudioBuffer.h"
//#include "src/Helpers.h"

namespace
{
	const int				SegmentSize = 512; //In frames
	const int				BufferCount = 4;

	ALCdevice*				Device;
	ALCcontext*				Context;
	ALuint					SourceID;
	ALuint					Buffers[BufferCount];

	volatile bool			Done;

	SoundTouchAudioBuffer	Buffer;

	ESThread*				Thread;
	ESSemaphore*			Semaphore;
}

static inline void			Stream							(uint32_t aBufferID)
{
	//Fetch data from audio buffer
	uint32_t stream[SegmentSize];
	Buffer.ReadDataSilentUnderrun(stream, SegmentSize);

	//Send to OpenAL
	alBufferData(aBufferID, AL_FORMAT_STEREO16, stream, SegmentSize * sizeof(uint32_t), 48000);
	alSourceQueueBuffers(SourceID, 1, &aBufferID);

	//Start buffer if needed
    ALint playing;
    alGetSourcei(SourceID, AL_SOURCE_STATE, &playing);
    if(playing != AL_PLAYING) alSourcePlay(SourceID);

	//Signal feeder
	if(!Semaphore->GetValue())
	{
		Semaphore->Post();
	}
}

static int					AudioThread						(void* aData)
{
	//Add initial buffers
	for(int i = 0; i != BufferCount; i ++)
	{
		Stream(Buffers[i]);
	}

	//Loop
	while(!Done)
	{
		int processed;
		alGetSourcei(SourceID, AL_BUFFERS_PROCESSED, &processed);

		if(processed)
		{
			ALuint buffer;
			alSourceUnqueueBuffers(SourceID, 1, &buffer);

			Stream(buffer);
		}

		Utility::Sleep(1);
	}
}

#define						Check(X)	if(!(X)){printf("OpenAL Error at %d\n", __LINE__); Shutdown(); return false;}
#define						CheckAL(X)  X; if((AL_NO_ERROR != alGetError())){printf("OpenAL Error at %d\n", __LINE__); Shutdown(); return false;}

bool						ESAudio::Initialize				()
{
	alGetError();

	//Create context
	Check(Device = alcOpenDevice(0));
	Check(Context = alcCreateContext(Device, 0));
	Check(ALC_TRUE == alcMakeContextCurrent(Context));

	//Create source
	CheckAL(alGenSources(1, &SourceID));

	//Create buffers
	CheckAL(alGenBuffers(BufferCount, Buffers));

	//Setup listener
	alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
	alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
	ALfloat listener_orientation[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	alListenerfv(AL_ORIENTATION, listener_orientation);


	Done = false;
	Thread = ESThreads::MakeThread(AudioThread, 0);
	Semaphore = ESThreads::MakeSemaphore(1);

	return true;
}

void						ESAudio::Shutdown				()
{
	Done = true;
	delete Thread;
	delete Semaphore;

	//Kill source (ID = zero is OK)
	alDeleteSources(1, &SourceID);
	alDeleteBuffers(BufferCount, Buffers);

	//Kill device and context (should fail trivially on error)
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);

}

void						ESAudio::AddSamples				(const uint32_t* aSamples, uint32_t aCount)
{
	while(Buffer.GetBufferFree() < aCount)
		Semaphore->Wait();

	Buffer.WriteData((uint32_t*)aSamples, aCount);
}

volatile int32_t			ESAudio::GetBufferAmount		()
{
	return Buffer.GetBufferAmount();
}

volatile int32_t			ESAudio::GetBufferFree			()
{
	return Buffer.GetBufferFree();
}

void						ESAudio::SetSpeed				(uint32_t aSpeed)
{
	Buffer.SetSpeed(aSpeed);
}

