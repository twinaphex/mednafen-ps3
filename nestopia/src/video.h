#ifndef NEST_VIDEO_H_MDFN
#define NEST_VIDEO_H_MDFN

#include <src/mednafen.h>

#include "core/api/NstApiVideo.hpp"
using namespace Nes;
using namespace Nes::Api;


namespace nestMDFN
{
	void						SetupVideo						(uint32_t aBits, uint32_t aRedShift, uint32_t aGreenShift, uint32_t aBlueShift);
	void						SetBuffer						(void* aBuffer, uint32_t aPitch);
	void						SetFrame						(MDFN_Rect* aFrame);
	Video::Output*				GetVideo						();
}

#endif


