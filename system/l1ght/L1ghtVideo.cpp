#include <ps3_system.h>

extern "C"
{
	extern realityFragmentProgram nv30_fp;
}
#include "nv40_vp.vcg.h"

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

						L1ghtVideo::L1ghtVideo			()
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

	esScreenWidth = Resolution.width;
	esScreenHeight = Resolution.height;

	//Make vertex buffer
	Allocate(VertexBuffer[0], VertexBufferOffset[0], 1024 * 1024, 64);
	Allocate(VertexBuffer[1], VertexBufferOffset[1], 1024 * 1024, 64);

	//Make Filler Texture
	FillerTexture = CreateTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Make fragment program
	u32 *frag_mem = (u32*)rsxMemAlign(256, 256);
	realityInstallFragmentProgram(GCMContext, &nv30_fp, frag_mem);

	NextBuffer = 0;
	VertexBufferPosition = 0;
}

						L1ghtVideo::~L1ghtVideo			()
{
	//TODO: Clean up rsxMemory, not supported right now so not done right now

	if(FillerTexture)
	{
		delete FillerTexture;
	}
}

void					L1ghtVideo::Flip				()
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
	esClip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
}

void					L1ghtVideo::PlaceTexture		(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor, Area* aArea)
{
	aX += esClip.X;
	aY += esClip.Y;

	//TODO: Better clipping
	if(aX + aWidth >= esClip.Right() || aY + aHeight >= (esClip.Bottom() + 10))
	{
		return;
	}

	if(!aArea)
	{
		ApplyTexture(aTexture, Area(0, 0, aTexture->GetWidth(), aTexture->GetHeight()));
	}
	else
	{
		ApplyTexture(aTexture, *aArea);
	}

	DrawQuad(Area(aX, aY, aWidth, aHeight), aColor);
}

void					L1ghtVideo::FillRectangle		(Area aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea.X, aArea.Y, aArea.Width, aArea.Height, aColor);
}

void					L1ghtVideo::PresentFrame		(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan)
{
	ApplyTexture(aTexture, Area(aViewPort.X, aViewPort.Y, aViewPort.Width, aViewPort.Height));

	Area output = CalculatePresentArea(aAspectOverride, aUnderscan);
	
	realityBlendEnable(GCMContext, 0);	
	DrawQuad(output, 0xFFFFFFFF);
	realityBlendEnable(GCMContext, 1);		
}

void					L1ghtVideo::PrepareBuffer		()
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

void					L1ghtVideo::Allocate			(uint32_t*& aMemory, uint32_t& aOffset, uint32_t aSize, uint32_t aAlign)
{
	aMemory = (uint32_t*)rsxMemAlign(aAlign, aSize);
	
	if(!aMemory)
	{
		Abort("ESVideo::Allocate: Couldn't allocate rsx memory");
	}
	
	realityAddressToOffset(aMemory, &aOffset);
}

void					L1ghtVideo::ApplyTexture		(Texture* aTexture, Area aRegion)
{
	L1ghtTexture* l1ghtTexture = (L1ghtTexture*)aTexture;

	//TODO: Handle this better
	if(aRegion.Right() > l1ghtTexture->Width || aRegion.Bottom() > l1ghtTexture->Height)
	{
		Abort("ESVideo::ApplyTexture: Area out of range");
	}

	realityTexture tex;
	tex.swizzle = NV30_3D_TEX_SWIZZLE_S0_X_S1 | NV30_3D_TEX_SWIZZLE_S0_Y_S1 | NV30_3D_TEX_SWIZZLE_S0_Z_S1 | NV30_3D_TEX_SWIZZLE_S0_W_S1 |  NV30_3D_TEX_SWIZZLE_S1_X_X | NV30_3D_TEX_SWIZZLE_S1_Y_Y | NV30_3D_TEX_SWIZZLE_S1_Z_Z | NV30_3D_TEX_SWIZZLE_S1_W_W;
	tex.format = NV40_3D_TEX_FORMAT_FORMAT_A8R8G8B8 | NV40_3D_TEX_FORMAT_LINEAR | NV30_3D_TEX_FORMAT_DIMS_2D | NV30_3D_TEX_FORMAT_DMA0 | NV30_3D_TEX_FORMAT_NO_BORDER | (0x8000) | (1 << NV40_3D_TEX_FORMAT_MIPMAP_COUNT__SHIFT);
	tex.wrap =  NV30_3D_TEX_WRAP_S_CLAMP_TO_EDGE | NV30_3D_TEX_WRAP_T_CLAMP_TO_EDGE | NV30_3D_TEX_WRAP_R_CLAMP_TO_EDGE;
	tex.enable = NV40_3D_TEX_ENABLE_ENABLE;

	tex.filter = 0x3FD6 | (l1ghtTexture->Filter ? NV30_3D_TEX_FILTER_MIN_LINEAR | NV30_3D_TEX_FILTER_MAG_LINEAR : NV30_3D_TEX_FILTER_MIN_NEAREST | NV30_3D_TEX_FILTER_MAG_NEAREST);
	tex.offset = l1ghtTexture->Offset + 4 * ((aRegion.Y * l1ghtTexture->Width) + aRegion.X);
	tex.width = aRegion.Width;
	tex.height = aRegion.Height;
	tex.stride = l1ghtTexture->Width * 4;
	
	realitySetTexture(GCMContext, 0, &tex);
}

void					L1ghtVideo::ApplyVertexBuffer	(uint32_t aPosition)
{
	int off_position = realityVertexProgramGetInputAttribute((realityVertexProgram*) nv40_vp_bin, "inputvertex.vertex");
	int off_texture  = realityVertexProgramGetInputAttribute((realityVertexProgram*) nv40_vp_bin, "inputvertex.texcoord");
	int off_color    = realityVertexProgramGetInputAttribute((realityVertexProgram*) nv40_vp_bin, "inputvertex.color");
	uint32_t offset = VertexBufferOffset[NextBuffer] + sizeof(Vertex) * aPosition;

	realityBindVertexBufferAttribute(GCMContext, off_position, offset, sizeof(Vertex), 4, REALITY_BUFFER_DATATYPE_FLOAT, REALITY_RSX_MEMORY);
	realityBindVertexBufferAttribute(GCMContext, off_color, offset + 16, sizeof(Vertex), 4, REALITY_BUFFER_DATATYPE_BYTE, REALITY_RSX_MEMORY);
	realityBindVertexBufferAttribute(GCMContext, off_texture, offset + 20, sizeof(Vertex), 2, REALITY_BUFFER_DATATYPE_FLOAT, REALITY_RSX_MEMORY);
}

void					L1ghtVideo::DrawQuad			(Area aRegion, uint32_t aColor)
{
	ApplyVertexBuffer(VertexBufferPosition);

	if((VertexBufferPosition + 4) * sizeof(Vertex) > 1024 * 1024)
	{
		Abort("ESVideo::DrawQuad: Out of vertex buffer room.");
	}

	Vertex* vertices = (Vertex*)VertexBuffer[NextBuffer];
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.X, aRegion.Y, 1.0, aColor, 0.0, 0.0);
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.Right(), aRegion.Y, 1.0, aColor, 1.0, 0.0);	
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.Right(), aRegion.Bottom(), 1.0, aColor, 1.0, 1.0);	
	FillVertex(&vertices[VertexBufferPosition ++], aRegion.X, aRegion.Bottom(), 1.0, aColor, 0.0, 1.0);	

	realityDrawVertexBuffer(GCMContext, REALITY_QUADS, 0, 4);
}
