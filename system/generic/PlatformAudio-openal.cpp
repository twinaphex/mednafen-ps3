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

static void					AssertError2					(int32_t line)
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
	{
		printf("AL_ERROR: %d: %X\n", line, error);
		assert(false);
	}		
}

#define						AssertError() AssertError2(__LINE__)

static inline void			Stream							(uint32_t aBufferID)
{
	//Fetch data from audio buffer
	uint32_t stream[SegmentSize];
	Buffer.ReadDataSilentUnderrun(stream, SegmentSize);

	//Send to OpenAL
	alBufferData(aBufferID, AL_FORMAT_STEREO16, stream, SegmentSize * sizeof(uint32_t), 48000); AssertError();
	alSourceQueueBuffers(SourceID, 1, &aBufferID); AssertError();

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
	alGetError();

	//Create context
	Device = alcOpenDevice(0); assert(Device);
	Context = alcCreateContext(Device, 0); assert(Context);
	alcMakeContextCurrent(Context); AssertError();

	//Create source
	alGenSources(1, &SourceID); AssertError();

	//Create buffers
	alGenBuffers(BufferCount, Buffers); AssertError();

	//Setup listener
	alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
	alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
	ALfloat listener_orientation[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	alListenerfv(AL_ORIENTATION, listener_orientation);

	//Loop
	for(int i = 0; i != BufferCount; i ++)
	{
		Stream(Buffers[i]);
	}

	while(!Done)
	{
		int processed;
		alGetSourcei(SourceID, AL_BUFFERS_PROCESSED, &processed); AssertError();

		if(processed)
		{
			ALuint buffer;
			alSourceUnqueueBuffers(SourceID, 1, &buffer); AssertError();

			Stream(buffer);
		}

		Utility::Sleep(1);
	}

	//Kill source
	alDeleteSources(1, &SourceID); AssertError();

	//Kill buffers
	alDeleteBuffers(BufferCount, Buffers); AssertError();

	//Kill device and context
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
}

void						ESAudio::Initialize				()
{
	Done = false;
	Thread = ESThreads::MakeThread(AudioThread, 0);
	Semaphore = ESThreads::MakeSemaphore(1);
}

void						ESAudio::Shutdown				()
{
	Done = true;
	delete Thread;
	delete Semaphore;
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

