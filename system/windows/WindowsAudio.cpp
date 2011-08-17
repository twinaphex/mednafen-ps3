#include <es_system.h>
#include "XAudio2-compat.h"
#include <SoundTouch.h>
#include "src/utility/AudioBuffer.h"

//TODO: Audio buffer is not mutex locked.

namespace
{
	IXAudio2*						Device;
	IXAudio2MasteringVoice*			MasterVoice;
	IXAudio2SourceVoice*			Voice;

	AudioBuffer<8192>				Buffer;
	ESSemaphore*					Semaphore;

	soundtouch::SoundTouch			PitchShifter;
	uint32_t						AuxBuffer[48000];
	uint32_t						Speed = 1;

	uint32_t						Buffers[2][256];
	uint32_t						NextBuffer = 0;

	class							VoiceCallback : public IXAudio2VoiceCallback
	{
		public:
			void __stdcall			OnBufferEnd						(void * pBufferContext)
			{
				Buffer.ReadDataSilentUnderrun(Buffers[NextBuffer], 256);

				XAUDIO2_BUFFER buf = {0, 256 * 4, (BYTE*)Buffers[NextBuffer], 0, 0, 0, 0, 0, 0};
				Voice->SubmitSourceBuffer(&buf);

				NextBuffer ^= 1;

				if(!Semaphore->GetValue())
				{
					Semaphore->Post();
				}
			}

			void __stdcall			OnStreamEnd						() { }
			void __stdcall			OnVoiceProcessingPassEnd		() { }
			void __stdcall			OnVoiceProcessingPassStart		(UINT32 SamplesRequired) { }
			void __stdcall			OnBufferStart					(void * pBufferContext) { }
			void __stdcall			OnLoopEnd						(void * pBufferContext) { }
			void __stdcall			OnVoiceError					(void * pBufferContext, HRESULT Error){ }
	};

	VoiceCallback					AudioCallbacks;
}

void								ESAudio::Initialize				()
{
	//Make thread objects
	Semaphore = es_threads->MakeSemaphore(1);

	//Setup XAudio2
	WAVEFORMATEX waveFormat = {WAVE_FORMAT_PCM, 2, 48000, 48000 * 2, 4, 16, 0};

	XAudio2Create(&Device);
	Device->CreateMasteringVoice(&MasterVoice, 2, 48000);
	Device->CreateSourceVoice(&Voice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &AudioCallbacks);

	//Send first buffer to voice
	XAUDIO2_BUFFER buf = {0, 256 * 4, (BYTE*)Buffers[1], 0, 0, 0, 0, 0, 0};
	Voice->SubmitSourceBuffer(&buf);
	Voice->Start();

	//Soundtouch setup
	PitchShifter.setSampleRate(48000);
	PitchShifter.setChannels(2);
}

void								ESAudio::Shutdown				()
{
	Device->Release();
	Device = 0;
	Voice = 0;
	MasterVoice = 0;

	delete Semaphore;
}


void								ESAudio::AddSamples				(const uint32_t* aSamples, uint32_t aCount)
{
	//Pitch shifting
	if(Speed != 1)
	{
		//Add samples to sound touch
		PitchShifter.putSamples((int16_t*)aSamples, aCount);

		//Process any available samples 
		int32_t sampleCount = PitchShifter.numSamples();

		if(sampleCount)
		{
			//Don't process too large of a block
			sampleCount = PitchShifter.receiveSamples((int16_t*)AuxBuffer, std::min(2048, sampleCount));

			//Put them in the ringbuffer as normal
			while(Buffer.GetBufferFree() < sampleCount)
				Semaphore->Wait();

			Buffer.WriteData((uint32_t*)AuxBuffer, sampleCount);
		}
	}
	else
	{
		//Put them in the ringbuffer as normal
		while(Buffer.GetBufferFree() < aCount)
			Semaphore->Wait();

		Buffer.WriteData((uint32_t*)aSamples, aCount);
	}
}

volatile int32_t					ESAudio::GetBufferAmount		()
{
	return Buffer.GetBufferAmount();
}

volatile int32_t					ESAudio::GetBufferFree			()
{
	return Buffer.GetBufferFree();
}

void								ESAudio::SetSpeed				(uint32_t aSpeed)
{
	Speed = aSpeed;
	PitchShifter.setTempo(aSpeed);
}

