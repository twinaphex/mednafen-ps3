#ifndef NEST_SOUND_H_MDFN
#define NEST_SOUND_H_MDFN

#include "core/api/NstApiSound.hpp"
using namespace Nes;
using namespace Nes::Api;


namespace nestMDFN
{
	void						SetupAudio						(uint32_t aSampleRate);
	Sound::Output*				GetAudio						();
	void						CopyAudio						(void* aSoundBuffer, uint32_t aMaxSize, int32_t& aFrameCount);
}

#endif


