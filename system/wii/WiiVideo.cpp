#include <es_system.h>

GXColor bgc[2] = {{0, 0, 0, 0xFF}, {0xFF, 0, 0, 0xFF}};
	Mtx view;
	guVector viewVec[3] =
	{
        {0.0F, 0.0F, 0.0F},
        {0.0F, 0.5F, 0.0F},
        {0.0F, 0.0F, -0.5F}
	};

						WiiVideo::WiiVideo				()
{
	//Get Screen Mode
	ScreenMode = VIDEO_GetPreferredMode(0);

	//Setup Framebuffers
	for(int i = 0; i != 2; i ++)
	{
		FrameBuffer[i] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(ScreenMode));
		VIDEO_ClearFrameBuffer(ScreenMode, FrameBuffer[i], 0);
	}
	CurrentFrameBuffer = 0;

	//Setup Display
	VIDEO_Configure(ScreenMode);
	VIDEO_SetNextFramebuffer(FrameBuffer[0]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(ScreenMode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync(); //Is it needed?
	FirstFrame = true;

	CurrentFrameBuffer ^= 1;

	//Copy size
	esScreenWidth = ScreenMode->fbWidth;
	esScreenHeight = ScreenMode->efbHeight;

	//Setup GX
	FIFOBuffer = memalign(32, FIFOSize);
	memset(FIFOBuffer, 0, FIFOSize);
	GX_Init(FIFOBuffer, FIFOSize);

	GX_SetCopyClear(bgc[0], 0xFFFFFF);

	GX_SetViewport(0, 0, ScreenMode->fbWidth, ScreenMode->efbHeight, 0, 1);
	GX_SetDispCopyYScale(GX_GetYScaleFactor(ScreenMode->efbHeight, ScreenMode->xfbHeight));
	GX_SetScissor(0, 0, ScreenMode->fbWidth, ScreenMode->efbHeight);
	GX_SetDispCopySrc(0, 0, ScreenMode->fbWidth, ScreenMode->efbHeight);
	GX_SetDispCopyDst(ScreenMode->fbWidth, ScreenMode->xfbHeight);
	GX_SetCopyFilter(ScreenMode->aa, ScreenMode->sample_pattern, GX_TRUE, ScreenMode->vfilter);
	GX_SetFieldMode(ScreenMode->field_rendering, ((ScreenMode->viHeight == 2 * ScreenMode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	GX_SetPixelFmt(ScreenMode->aa ? GX_PF_RGB565_Z16 : GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	GX_SetDispCopyGamma(GX_GM_1_0);
	GX_SetCullMode(GX_CULL_NONE);

	//Seutp vertex format
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    GX_InvVtxCache();
	GX_InvalidateTexAll();

	//Setup View
	memset(&view, 0, sizeof(Mtx));
	guLookAt(view, &viewVec[0], &viewVec[1], &viewVec[2]);
	GX_LoadPosMtxImm(view, GX_PNMTX0);

	//Setup Projection
	Mtx44 p;
	guOrtho(p, 0, 480, 0, 680, 100, 1000); // matrix, t, b, l, r, n, f
	GX_LoadProjectionMtx (p, GX_ORTHOGRAPHIC);

	//Create the filler texture
	FillerTexture = CreateTexture(2, 2, true);
	FillerTexture->Clear(0xFFFFFFFF);
}

						WiiVideo::~WiiVideo				()
{
	delete FillerTexture;
}

void					WiiVideo::SetClip				(Area aClip)
{
	ESVideo::SetClip(aClip);

	Area clap = GetClip();
	GX_SetScissor(clap.X, clap.Y, clap.Width, clap.Height);
}

void					WiiVideo::Flip					()
{
	//Finish up GX drawing
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(FrameBuffer[CurrentFrameBuffer], GX_TRUE);

	GX_DrawDone();

	//Flip the frame
	VIDEO_SetNextFramebuffer(FrameBuffer[CurrentFrameBuffer]);
	if(FirstFrame)
	{
		FirstFrame = false;
		VIDEO_SetBlack(FALSE);
	}
	VIDEO_Flush();
	VIDEO_WaitVSync();
	CurrentFrameBuffer ^= 1;

	//Prepare for the next frame
	Mtx iden;
	guMtxIdentity(iden);
	GX_LoadPosMtxImm(iden, GX_PNMTX0);
	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
}

void					WiiVideo::PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor, Area* aArea)
{
	aX += esClip.X;
	aY += esClip.Y;

	Area texArea;
	if(aArea)
	{
		texArea = *aArea;
	}
	else
	{
		texArea = Area(0, 0, aTexture->GetWidth(), aTexture->GetHeight());
	}


	uint8_t r = ((aColor >> 24) & 0xFF);
	uint8_t g = ((aColor >> 16) & 0xFF);
	uint8_t b = ((aColor >> 8) & 0xFF);
	uint8_t a = ((aColor >> 0) & 0xFF);	

	GX_InvVtxCache();
	((WiiTexture*)aTexture)->Apply();
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(aX, aY, -250);
		GX_Color4u8(r, g, b, a);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(aX + aWidth, aY, -250);
		GX_Color4u8(r, g, b, a);
		GX_TexCoord2f32(1, 0);

		GX_Position3f32(aX + aWidth, aY + aHeight, -250);
		GX_Color4u8(r, g, b, a);
		GX_TexCoord2f32(1, 1);

		GX_Position3f32(aX, aY + aHeight, -250);
		GX_Color4u8(r, g, b, a);
		GX_TexCoord2f32(0, 1);
	GX_End();
}

void					WiiVideo::FillRectangle			(Area aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea.X, aArea.Y, aArea.Width, aArea.Height, aColor);
}

void					WiiVideo::PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
/*	Area output = CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);

	((WiiTexture*)aTexture)->Apply();
	GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3s16(output.X, output.Y, 0);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2s16(aViewPort.X, aViewPort.Y);

		GX_Position3s16(output.Right(), output.Y, 0);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2s16(aViewPort.Right(), aViewPort.Y);

		GX_Position3s16(output.Right(), output.Bottom(), 0);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2s16(aViewPort.Right(), aViewPort.Bottom());

		GX_Position3s16(output.X, output.Bottom(), 0);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2s16(aViewPort.X, aViewPort.Bottom());
	GX_End();
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);*/
}

void					WiiVideo::HandleRetrace			(uint32_t unused)
{
/*	WiiVideo* video = (WiiVideo*)es_video;

	if(video && video->ReadyForCopy)
	{
		GX_SetZMode(GX_TRUE, GX_LEQUAL,	GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(video->FrameBuffer[0], GX_TRUE);
		GX_Flush();
		video->ReadyForCopy = false;
	}*/
}