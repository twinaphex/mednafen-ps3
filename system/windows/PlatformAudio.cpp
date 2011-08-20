#include <es_system.h>
#include "XAudio2-compat.h"
#include <SoundTouch.h>
#include "src/utility/AudioBuffer.h"

//TODO: Audio buffer is not mutex locked.

namespace
{
	//XAudio2 Interfaces
	IXAudio2*						Device;
	IXAudio2MasteringVoice*			MasterVoice;
	IXAudio2SourceVoice*			Voice;

	//Audio Buffer
	SoundTouchAudioBuffer			Buffer;

	//XAudio Sound buffering
	HANDLE							BufferReadyEvent;
	uint32_t						Buffers[8][256];
	uint32_t						NextBuffer = 0;

	int								BufferFeed						(void* aData)
	{
		Buffer.ReadDataSilentUnderrun(Buffers[NextBuffer], 256);

		XAUDIO2_BUFFER buf = {0, 256 * 4, (BYTE*)Buffers[NextBuffer], 0, 0, 0, 0, 0, 0};
		Voice->SubmitSourceBuffer(&buf);

		NextBuffer = (NextBuffer == 7) ? 0 : (NextBuffer + 1);

		SetEvent(BufferReadyEvent);

		return 0;
	}

	class							VoiceCallback : public IXAudio2VoiceCallback
	{
		public:
			//TODO: XAudio docs say I should signal a thread here to do the lifting
			void __stdcall			OnBufferEnd						(void * pBufferContext) {BufferFeed(0);}
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
	BufferReadyEvent = CreateEvent(0, 0, 0, 0);

	//Setup XAudio2
	WAVEFORMATEX waveFormat = {WAVE_FORMAT_PCM, 2, 48000, 48000 * 2, 4, 16, 0};

	XAudio2Create(&Device);
	Device->CreateMasteringVoice(&MasterVoice, 2, 48000);
	Device->CreateSourceVoice(&Voice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &AudioCallbacks);

	//Setup voice
	XAUDIO2_BUFFER buf = {0, 256 * 4, (BYTE*)Buffers[7], 0, 0, 0, 0, 0, 0};
	Voice->SubmitSourceBuffer(&buf);
	Voice->Start();
}

void								ESAudio::Shutdown				()
{
	Device->Release();
	Device = 0;
	Voice = 0;
	MasterVoice = 0;

	CloseHandle(BufferReadyEvent);
}


void								ESAudio::AddSamples				(const uint32_t* aSamples, uint32_t aCount)
{
	while(Buffer.GetBufferFree() < aCount)
		WaitForSingleObject(BufferReadyEvent, INFINITE);

	Buffer.WriteData((uint32_t*)aSamples, aCount);
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
	Buffer.SetSpeed(aSpeed);
}

