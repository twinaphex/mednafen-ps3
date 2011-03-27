#include <es_system.h>

extern "C" rsxFragmentProgram* vp_bin;
extern "C" int vp_bin_size;
extern "C" rsxVertexProgram* fp_bin;
extern "C" int fp_bin_size;


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
	GCMContext = rsxInit(0x80000, 8192 * 1024, host_addr);

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
	esWideScreen = true;

	//Make vertex buffer
	Allocate(VertexBuffer[0], VertexBufferOffset[0], 1024 * 1024, 64);
	Allocate(VertexBuffer[1], VertexBufferOffset[1], 1024 * 1024, 64);

	//Make Filler Texture
	FillerTexture = CreateTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Make fragment program
	Allocate(FragmentMemory, FragmentOffset, 256, 256);

	NextBuffer = 0;
	VertexBufferPosition = 0;
}

						L1ghtVideo::~L1ghtVideo			()
{
	delete FillerTexture;

	//TODO: rsxFinish
	rsxFree(Screen[0]);
	rsxFree(Screen[1]);
	rsxFree(Screen[2]);
	rsxFree(VertexBuffer[0]);
	rsxFree(VertexBuffer[1]);
	rsxFree(FragmentMemory);

	rsxFinish(GCMContext, 0);
}

void					L1ghtVideo::SetClip					(Area aClip)
{
	ESVideo::SetClip(aClip);

	Area clap = GetClip();

	f32 scale[4] = {1.0f, 1.0f, 1.0f, 0.0f};
	f32 trans[4] = {clap.X, clap.Y, 0.0f, 0.0f};
	rsxSetViewport(GCMContext, 0, 0, Resolution.width, Resolution.height, -1, 1, scale, trans);

	for(int i = 0; i != 8; i ++)
	{
		rsxSetViewportClip(GCMContext, i, clap.Right(), clap.Bottom());
	}
}

void					L1ghtVideo::Flip				()
{
	gcmSetFlip(GCMContext, NextBuffer);
	rsxFlushBuffer(GCMContext);
	gcmSetWaitFlip(GCMContext);

	while(gcmGetFlipStatus() != 0)
	{
		usleep(200);
	}
	gcmResetFlipStatus();
	
	PrepareBuffer();
	esClip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
}



void					L1ghtVideo::PlaceTexture		(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor)
{
	ApplyTexture(aTexture, aSource);
	DrawQuad(aDestination, aColor);
}

void					L1ghtVideo::FillRectangle		(const Area& aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea, Area(0, 0, 2, 2), aColor);
}

void					L1ghtVideo::PresentFrame		(Texture* aTexture, const Area& aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	ApplyTexture(aTexture, aViewPort);
	Area output = CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);
	
	rsxSetBlendEnable(GCMContext, 0);
	DrawQuad(output, 0xFFFFFFFF);
	rsxSetBlendEnable(GCMContext, 1);
}

void					L1ghtVideo::PrepareBuffer		()
{
	NextBuffer = (NextBuffer + 1) & 1;

	gcmSurface surface =
	{
//TODO: What is proper for these values?
		0, //?
		0, //?
		GCM_TF_COLOR_X8R8G8B8,
		GCM_TF_TARGET_NONE,
		{GCM_LOCATION_RSX, GCM_LOCATION_RSX, GCM_LOCATION_RSX, GCM_LOCATION_RSX},
		{ScreenOffset[NextBuffer], 0, 0, 0},
		{Resolution.width * 4, 0, 0, 0},
		GCM_TF_ZETA_Z24S8,
		GCM_LOCATION_RSX,
		{0, 0},
		ScreenOffset[2],
		Resolution.width * 4,
		Resolution.width,
		Resolution.height,
		0,
		0
	};

	rsxSetSurface(GCMContext, &surface);

	rsxSetClearColor(GCMContext, 0);
	rsxSetClearDepthValue(GCMContext, 0xffff);
	rsxClearSurface(GCMContext, GCM_CLEAR_R | GCM_CLEAR_G | GCM_CLEAR_B | GCM_CLEAR_A | GCM_CLEAR_S | GCM_CLEAR_Z);

	f32 scale[4] = {1.0f, 1.0f, 1.0f, 0.0f};
	f32 trans[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	rsxSetViewport(GCMContext, 0, 0, Resolution.width, Resolution.height, -1, 1, scale, trans);
	rsxZControl(GCMContext, 0, 1, 1);
//TODO: Is this right?
	rsxSetBlendFunc(GCMContext, GCM_SRC_COLOR, GCM_DST_COLOR, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA);
	rsxSetBlendEquation(GCMContext, GCM_FUNC_ADD, GCM_FUNC_ADD);
	rsxSetBlendEnable(GCMContext, 1);

//TODO: Shaders!
	rsxLoadVertexProgram(GCMContext, fp_bin, rsxVertexProgramGetUCode(fp_bin));
	rsxLoadFragmentProgramLocation(GCMContext, vp_bin, FragmentOffset, GCM_LOCATION_RSX);
	
	VertexBufferPosition = 0;
}

void					L1ghtVideo::Allocate			(uint32_t*& aMemory, uint32_t& aOffset, uint32_t aSize, uint32_t aAlign)
{
	aMemory = (uint32_t*)rsxMemalign(aAlign, aSize);

	if(!aMemory)
	{
		Abort("ESVideo::Allocate: Couldn't allocate rsx memory");
	}
	
	rsxAddressToOffset(aMemory, &aOffset);
}

void					L1ghtVideo::ApplyTexture		(Texture* aTexture, Area aRegion)
{
	L1ghtTexture* l1ghtTexture = (L1ghtTexture*)aTexture;

	//TODO: Handle this better
	if(aRegion.Right() > l1ghtTexture->esWidth || aRegion.Bottom() > l1ghtTexture->esHeight)
	{
		Abort("ESVideo::ApplyTexture: Area out of range");
	}

	//TODO: What is proper for theses values?
	gcmTexture tex;
	tex.format = GCM_TEXTURE_FORMAT_LIN | GCM_TEXTURE_FORMAT_A8R8G8B8; //?
	tex.mipmap = 1; //?
	tex.dimension = GCM_TEXTURE_DIMS_2D; //?
	tex.cubemap = 0; //?
	tex.remap = 0; //?
	tex.width = aRegion.Width;
	tex.height = aRegion.Height;
	tex.depth = 32; //?
	tex.location = GCM_LOCATION_RSX;
	tex.pitch = l1ghtTexture->esWidth * 4;
	tex.offset = l1ghtTexture->Offset + 4 * ((aRegion.Y * l1ghtTexture->esWidth) + aRegion.X);

	rsxLoadTexture(GCMContext, 0, &tex);
	rsxTextureFilter(GCMContext, 0, l1ghtTexture->esFilter ? GCM_TEXTURE_LINEAR : GCM_TEXTURE_NEAREST, l1ghtTexture->esFilter ? GCM_TEXTURE_LINEAR : GCM_TEXTURE_NEAREST, GCM_TEXTURE_CONVOLUTION_QUINCUNX); //Convolution?
	rsxTextureWrapMode(GCMContext, 0, GCM_TEXTURE_CLAMP, GCM_TEXTURE_CLAMP, GCM_TEXTURE_CLAMP, 0, 0, 128); //Gamma?
	rsxTextureControl(GCMContext, 0, GCM_TRUE, 0, 0, 0); //LOD?
}

void					L1ghtVideo::ApplyVertexBuffer	(uint32_t aPosition)
{
	//TODO: Fucking shaders!
	int off_position = rsxVertexProgramGetAttrib((rsxVertexProgram*)vp_bin, "inputvertex.vertex");
	int off_texture  = rsxVertexProgramGetAttrib((rsxVertexProgram*)vp_bin, "inputvertex.texcoord");
	int off_color    = rsxVertexProgramGetAttrib((rsxVertexProgram*)vp_bin, "inputvertex.color");
	uint32_t offset = VertexBufferOffset[NextBuffer] + sizeof(Vertex) * aPosition;

	rsxBindVertexArrayAttrib(GCMContext, off_position, offset, sizeof(Vertex), 4, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
	rsxBindVertexArrayAttrib(GCMContext, off_color, offset + 16, sizeof(Vertex), 4, GCM_VERTEX_DATA_TYPE_U8, GCM_LOCATION_RSX);
	rsxBindVertexArrayAttrib(GCMContext, off_texture, offset + 20, sizeof(Vertex), 2, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
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

	rsxDrawVertexArray(GCMContext, GCM_TYPE_QUADS, 0, 4);
}
