#include <es_system.h>
#include <SoundTouch.h>

//HACK: Put them IN esaudio later
static soundtouch::SoundTouch st;
static uint32_t auxStBuffer[48000];

void					ESAudio::Initialize				()
{
	SDL_AudioSpec spec;
	spec.freq = 48000;
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 2048;
	spec.callback = ProcessAudioCallback;

	SDL_OpenAudio(&spec, &Format);
	SDL_PauseAudio(0);

	Semaphore = es_threads->MakeSemaphore(1);

	//Soundtouch setup
	st.setSampleRate(48000);
	st.setChannels(2);
}

void					ESAudio::Shutdown				()
{
	SDL_CloseAudio();
	delete Semaphore;
}

void					ESAudio::ProcessAudioCallback	(void *userdata, Uint8 *stream, int len)
{
	Buffer.ReadDataSilentUnderrun((uint32_t*)stream, len / 4);

	if(!Semaphore->GetValue())
	{
		Semaphore->Post();
	}
}

void					ESAudio::AddSamples				(const uint32_t* aSamples, uint32_t aCount)
{
	//Add samples to sound touch
	st.putSamples((int16_t*)aSamples, aCount);

	//Process any available samples 
	int32_t sampleCount = st.numSamples();

	if(sampleCount)
	{
		//Don't process too large of a block
		sampleCount = st.receiveSamples((int16_t*)auxStBuffer, std::min(2048, sampleCount));

		//Put them in the ringbuffer as normal
		while(Buffer.GetBufferFree() < sampleCount)
			Semaphore->Wait();

		SDL_LockAudio();
		Buffer.WriteData((uint32_t*)auxStBuffer, sampleCount);
		SDL_UnlockAudio();
	}
}

volatile int32_t		ESAudio::GetBufferAmount		() {return Buffer.GetBufferAmount();}
volatile int32_t		ESAudio::GetBufferFree			() {return Buffer.GetBufferFree();}

void					ESAudio::SetSpeed				(uint32_t aSpeed)
{
	//Tell sound touch about the new tempo
	st.setTempo(aSpeed);
}

SDL_AudioSpec			ESAudio::Format;
AudioBuffer<>			ESAudio::Buffer;
ESSemaphore*			ESAudio::Semaphore;
