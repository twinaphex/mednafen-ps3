#include <src/mednafen.h>

#include "mednafen.h"
#include "video.h"

namespace nestMDFN
{
	//Locals
	Video::Output 				EmuVideo;
	uint32_t					BitDepth;
	bool						UseNTSC = false;

	const int					NTSCSettingsArray[][3] = 
	{
		{Video::DEFAULT_SHARPNESS_COMP,			Video::DEFAULT_SHARPNESS_SVIDEO,		Video::DEFAULT_SHARPNESS_RGB},
		{Video::DEFAULT_COLOR_RESOLUTION_COMP,	Video::DEFAULT_COLOR_RESOLUTION_SVIDEO,	Video::DEFAULT_COLOR_RESOLUTION_RGB},
		{Video::DEFAULT_COLOR_BLEED_COMP,		Video::DEFAULT_COLOR_BLEED_SVIDEO,		Video::DEFAULT_COLOR_BLEED_RGB},
		{Video::DEFAULT_COLOR_ARTIFACTS_COMP,	Video::DEFAULT_COLOR_ARTIFACTS_SVIDEO,	Video::DEFAULT_COLOR_ARTIFACTS_RGB},
		{Video::DEFAULT_COLOR_FRINGING_COMP,	Video::DEFAULT_COLOR_FRINGING_SVIDEO,	Video::DEFAULT_COLOR_FRINGING_RGB}
	};

	void						SetupVideo							(uint32_t aBits, uint32_t aRedShift, uint32_t aGreenShift, uint32_t aBlueShift)
	{
		int ntscmode = NestopiaSettings.NTSCMode > 2 ? 0 : NestopiaSettings.NTSCMode;

		Video(Nestopia).SetSharpness(NTSCSettingsArray[0][ntscmode]);
		Video(Nestopia).SetColorResolution(NTSCSettingsArray[1][ntscmode]);
		Video(Nestopia).SetColorBleed(NTSCSettingsArray[2][ntscmode]);
		Video(Nestopia).SetColorArtifacts(NTSCSettingsArray[3][ntscmode]);
		Video(Nestopia).SetColorFringing(NTSCSettingsArray[4][ntscmode]);

		//TODO: Support 16-bit, and YUV
		//NTSC filter supports a specific color mode
		UseNTSC = NestopiaSettings.EnableNTSC && aRedShift == 16 && aGreenShift == 8 && aBlueShift == 0;

	    Video::RenderState renderState;
    	renderState.bits.count = 32;
	    renderState.bits.mask.r = 0xFF << aRedShift;
    	renderState.bits.mask.g = 0xFF << aGreenShift;
	    renderState.bits.mask.b = 0xFF << aBlueShift;
		renderState.filter = UseNTSC ? Video::RenderState::FILTER_NTSC : Video::RenderState::FILTER_NONE;
		renderState.width = UseNTSC ? Video::Output::NTSC_WIDTH : Video::Output::WIDTH;
		renderState.height = Video::Output::HEIGHT;

    	if(NES_FAILED(Video(Nestopia).SetRenderState(renderState)))
		{
			MDFND_PrintError("nest: Failed to set render state");
		}
	}

	void						SetBuffer							(void* aBuffer, uint32_t aPitch)
	{
		EmuVideo.pixels = aBuffer;
		EmuVideo.pitch = aPitch;

		Video(Nestopia).EnableUnlimSprites(NestopiaSettings.DisableSpriteLimit);
	}

	void						SetFrame							(MDFN_Rect* aFrame)
	{
		uint32_t widthhelp = UseNTSC ? 0 : 8;
		aFrame->x = NestopiaSettings.ClipSides ? widthhelp : 0;
		aFrame->y = NestopiaSettings.ScanLineStart;
		aFrame->w = (UseNTSC ? Video::Output::NTSC_WIDTH : Video::Output::WIDTH) - (NestopiaSettings.ClipSides ? widthhelp : 0);
		aFrame->h = NestopiaSettings.ScanLineEnd - NestopiaSettings.ScanLineStart;
	}

	Video::Output*				GetVideo							()
	{
		return &EmuVideo;
	}
}


