#include <ps3_system.h>

extern "C"
{
	extern realityFragmentProgram nv30_fp;
}
#include <nv40_vp.vcg.h>

namespace
{
	struct				Vertex
	{
		float x, y, z, w;
		uint32_t color;
		float u, v;
	};
	
	void				FillVertex						(Vertex* aVertex, float aX, float aY, float aZ, uint32_t aColor, float aU, float aV)
	{
		aVertex->x = aX;
		aVertex->y = aY;
		aVertex->z = aZ;		
		aVertex->w = 1.0f;
		aVertex->color = aColor;
		aVertex->u = aU;
		aVertex->v = aV;
	}
};

void					PS3Video::Init					()
{
	//Initialize rsx
	void *host_addr = memalign(1024*1024, 8192*1024);
	GCMContext = realityInit(0x80000, 8192 * 1024, host_addr);

	//Get Resolution and aspect
	VideoState state;
	videoGetState(0, 0, &state);
	videoGetResolution(state.displayMode.resolution, &Resolution);
	Aspect = state.displayMode.aspect;

	//Configure rsx display mode
	VideoConfiguration vconfig;
	memset(&vconfig, 0, sizeof(VideoConfiguration));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = Resolution.width * 4;
	videoConfigure(0, &vconfig, NULL, 0);

	gcmSetFlipMode(GCM_FLIP_VSYNC);

	//Make screen and z buffer	
	Allocate(Screen[0], ScreenOffset[0], Resolution.width * Resolution.height * 4, 16);
	Allocate(Screen[1], ScreenOffset[1], Resolution.width * Resolution.height * 4, 16);	
	Allocate(Screen[2], ScreenOffset[2], Resolution.width * Resolution.height * 4, 16);	
	gcmSetDisplayBuffer(0, ScreenOffset[0], Resolution.width * 4, Resolution.width, Resolution.height);	
	gcmSetDisplayBuffer(1, ScreenOffset[1], Resolution.width * 4, Resolution.width, Resolution.height);

	//Make vertex buffer
	Allocate(VertexBuffer[0], VertexBufferOffset[0], 1024 * 1024, 64);
	Allocate(VertexBuffer[1], VertexBufferOffset[1], 1024 * 1024, 64);

	//Make Filler Texture
	FillerTexture = new Texture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Make fragment program
	u32 *frag_mem = (u32*)rsxMemAlign(256, 256);
	realityInstallFragmentProgram(GCMContext, &nv30_fp, frag_mem);
	
	//Load fonts
	FontManager::InitFonts();
}

void					PS3Video::Quit					()
{
	//TODO: Clean up rsxMemory, not supported right now so not done right now

	if(FillerTexture)
	{
		delete FillerTexture;
	}
	
	FontManager::QuitFonts();
}

void					PS3Video::SetClip				(Area aClip)
{
	//TODO: Just use the whole screen if aClip is invalid
	Clip = aClip;
}

Area					PS3Video::GetClip				()
{
	return Clip;
}

void					PS3Video::Flip					()
{
	gcmSetFlip(GCMContext, NextBuffer);
	realityFlushBuffer(GCMContext);
	gcmSetWaitFlip(GCMContext);

	while(gcmGetFlipStatus() != 0)
	{
		usleep(200);
	}
	gcmResetFlipStatus();
	
	PrepareBuffer();
	Clip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
}

void					PS3Video::PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor)
{
	aX += Clip.X;
	aY += Clip.Y;

	//TODO: Better clipping
	if(aX + aWidth >= Clip.Right() || aY + aHeight >= (Clip.Bottom() + 10))
	{
		return;
	}

	ApplyTexture(aTexture, Area(0, 0, aTexture->Width, aTexture->Height));
	DrawQuad(Area(aX, aY, aWidth, aHeight), aColor);
}

void					PS3Video::FillRectangle			(Area aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea.X, aArea.Y, aArea.Width, aArea.Height, aColor);
}

void					PS3Video::PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, uint32_t aUnderscan)
{
	ApplyTexture(aTexture, Area(aViewPort.X, aViewPort.Y, aViewPort.Width, aViewPort.Height));

	Area output(0, 0, Resolution.width, Resolution.height);
	
	aAspectOverride = (Aspect == VIDEO_ASPECT_4_3) ? true : aAspectOverride;

	double underPercent = (double)aUnderscan / 100.0;
						
	double widthP = (aAspectOverride ? Resolution.width : Resolution.width - ((double)Resolution.width * .125)) * (underPercent / 2);
	double heightP = Resolution.height * (underPercent / 2);
		
	if(aAspectOverride)
	{
		output = Area(widthP, heightP, Resolution.width - widthP * 2, Resolution.height - heightP * 2);
	}
	else
	{
		uint32_t barSize = ((double)Resolution.width) * .125;
		output = Area(barSize + widthP, heightP, Resolution.width - barSize * 2 - widthP * 2, Resolution.height - heightP * 2);
	}
	
	realityBlendEnable(GCMContext, 0);	
	DrawQuad(output, 0xFFFFFFFF);
	realityBlendEnable(GCMContext, 1);		
}

void					PS3Video::PrepareBuffer			()
{
	NextBuffer = (NextBuffer + 1) & 1;

	realitySetRenderSurface(GCMContext, REALITY_SURFACE_COLOR0, REALITY_RSX_MEMORY, ScreenOffset[NextBuffer], Resolution.width * 4);
	realitySetRenderSurface(GCMContext, REALITY_SURFACE_ZETA, REALITY_RSX_MEMORY, ScreenOffset[2], Resolution.width * 4);
	realitySelectRenderTarget(GCMContext, REALITY_TARGET_0, REALITY_TARGET_FORMAT_COLOR_X8R8G8B8 | REALITY_TARGET_FORMAT_ZETA_Z16 | REALITY_TARGET_FORMAT_TYPE_LINEAR, Resolution.width, Resolution.height, 0, 0);

	realitySetClearColor(GCMContext, 0);
	realitySetClearDepthValue(GCMContext, 0xffff);
	realityClearBuffers(GCMContext, REALITY_CLEAR_BUFFERS_COLOR_R | REALITY_CLEAR_BUFFERS_COLOR_G | REALITY_CLEAR_BUFFERS_COLOR_B | NV30_3D_CLEAR_BUFFERS_COLOR_A | NV30_3D_CLEAR_BUFFERS_STENCIL | REALITY_CLEAR_BUFFERS_DEPTH);

	realityViewport(GCMContext, Resolution.width, Resolution.height);
	realityViewportTranslate(GCMContext, 0.0, 0.0, 0.0, 0.0);
	realityViewportScale(GCMContext, 1.0, 1.0, 1.0, 0.0); 
	realityZControl(GCMContext, 0, 1, 1);
	realityBlendFunc(GCMContext, NV30_3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | NV30_3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA, NV30_3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | NV30_3D_BLEND_FUNC_DST_ALPHA_ZERO);
	realityBlendEquation(GCMContext, NV40_3D_BLEND_EQUATION_RGB_FUNC_ADD | NV40_3D_BLEND_EQUATION_ALPHA_FUNC_ADD);
	realityBlendEnable(GCMContext, 1);	
	realityLoadVertexProgram(GCMContext, (realityVertexProgram*)&nv40_vp_bin);
	realityLoadFragmentProgram(GCMContext, &nv30_fp);
	
	VertexBufferPosition = 0;
}

void					PS3Video::Allocate				(uint32_t*& aMemory, uint32_t& aOffset, uint32_t aSize, uint32_t aAlign)
{
	aMemory = (uint32_t*)rsxMemAlign(aAlign, aSize);
	
	if(!aMemory)
	{
		Abort("PS3Video::Allocate: Couldn't allocate rsx memory");
	}
	
	realityAddressToOffset(aMemory, &aOffset);
}

void					PS3Video::ApplyTexture			(Texture* aTexture, Area aRegion)
{
	//TODO: Handle this better
	if(aRegion.Right() > aTexture->Width || aRegion.Bottom() > aTexture->Height)
	{
		Abort("PS3Video::ApplyTexture: Area out of range");
	}

	realityTexture tex;
	tex.swizzle = NV30_3D_TEX_SWIZZLE_S0_X_S1 | NV30_3D_TEX_SWIZZLE_S0_Y_S1 | NV30_3D_TEX_SWIZZLE_S0_Z_S1 | NV30_3D_TEX_SWIZZLE_S0_W_S1 |  NV30_3D_TEX_SWIZZLE_S1_X_X | NV30_3D_TEX_SWIZZLE_S1_Y_Y | NV30_3D_TEX_SWIZZLE_S1_Z_Z | NV30_3D_TEX_SWIZZLE_S1_W_W;
	tex.format = NV40_3D_TEX_FORMAT_FORMAT_A8R8G8B8 | NV40_3D_TEX_FORMAT_LINEAR | NV30_3D_TEX_FORMAT_DIMS_2D | NV30_3D_TEX_FORMAT_DMA0 | NV30_3D_TEX_FORMAT_NO_BORDER | (0x8000) | (1 << NV40_3D_TEX_FORMAT_MIPMAP_COUNT__SHIFT);
	tex.wrap =  NV30_3D_TEX_WRAP_S_CLAMP_TO_EDGE | NV30_3D_TEX_WRAP_T_CLAMP_TO_EDGE | NV30_3D_TEX_WRAP_R_CLAMP_TO_EDGE;
	tex.enable = NV40_3D_TEX_ENABLE_ENABLE;

	tex.filter = 0x3FD6 | (aTexture->Filter ? NV30_3D_TEX_FILTER_MIN_LINEAR | NV30_3D_TEX_FILTER_MAG_LINEAR : NV30_3D_TEX_FILTER_MIN_NEAREST | NV30_3D_TEX_FILTER_MAG_NEAREST);
	tex.offset = aTexture->Offset + 4 * ((aRegion.Y * aTexture->Width) + aRegion.X);
	tex.width = aRegion.Width;
	tex.height = aRegion.Height;
	tex.stride = aTexture->Width * 4;
	
	realitySetTexture(GCMContext, 0, &tex);
}

void					PS3Video::ApplyVertexBuffer		(uint32_t aPosition)
{
	int off_position = realityVertexProgramGetInputAttribute((realityVertexProgram*) nv40_vp_bin, "inputvertex.vertex");
	int off_texture  = realityVertexProgramGetInputAttribute((realityVertexProgram*) nv40_vp_bin, "inputvertex.texcoord");
	int off_color    = realityVertexProgramGetInputAttribute((realityVertexProgram*) nv40_vp_bin, "inputvertex.color");
	uint32_t offset = VertexBufferOffset[NextBuffer] + sizeof(Vertex) * aPosition;

	realityBindVertexBufferAttribute(GCMContext, off_position, offset, sizeof(Vertex), 4, REALITY_BUFFER_DATATYPE_FLOAT, REALITY_RSX_MEMORY);
	realityBindVertexBufferAttribute(GCMContext, off_color, offset + 16, sizeof(Vertex), 4, REALITY_BUFFER_DATATYPE_BYTE, REALITY_RSX_MEMORY);
	realityBindVertexBufferAttribute(GCMContext, off_texture, offset + 20, sizeof(Vertex), 2, REALITY_BUFFER_DATATYPE_FLOAT, REALITY_RSX_MEMORY);
}

void					PS3Video::DrawQuad				(Area aRegion, uint32_t aColor)
{
	ApplyVertexBuffer(VertexBufferPosition);

	if((VertexBufferPosition + 4) * sizeof(Vertex) > 1024 * 1024)
	{
		Abort("PS3Video::DrawQuad: Out of vertex buffer room.");
	}

	Vertex* vertices = (Vertex*)VertexBuffer[NextBuffer];
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.X, aRegion.Y, 1.0, aColor, 0.0, 0.0);
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.Right(), aRegion.Y, 1.0, aColor, 1.0, 0.0);	
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.Right(), aRegion.Bottom(), 1.0, aColor, 1.0, 1.0);	
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.X, aRegion.Bottom(), 1.0, aColor, 0.0, 1.0);	

	realityDrawVertexBuffer(GCMContext, REALITY_QUADS, 0, 4);
}

gcmContextData*			PS3Video::GCMContext;
VideoResolution			PS3Video::Resolution;
uint32_t				PS3Video::Aspect = 0;

Area					PS3Video::Clip(0, 0, 100, 100);

uint32_t*				PS3Video::Screen[3];
uint32_t				PS3Video::ScreenOffset[3];
uint32_t				PS3Video::NextBuffer = 0;

Texture*				PS3Video::FillerTexture = 0;

uint32_t*				PS3Video::VertexBuffer[2];	
uint32_t				PS3Video::VertexBufferOffset[2];
uint32_t				PS3Video::VertexBufferPosition = 0;