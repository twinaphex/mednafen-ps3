#ifndef PS3VIDEO_H
#define	PS3VIDEO_H

class							PS3Video;
class							Font;

class							Area
{
	public:
		uint32_t				X, Y, Width, Height;
		
		uint32_t				Right					()		{return X + Width;}
		uint32_t				Bottom					()		{return Y + Height;}

								Area					(uint32_t aX = 0, uint32_t aY = 0, uint32_t aWidth = 0, uint32_t aHeight = 0)
		{
			X = aX;
			Y = aY;
			Width = aWidth;
			Height = aHeight;
		};
};

class								Texture
{
	friend class					PS3Video;

	public:
									Texture					(uint32_t aWidth, uint32_t aHeight);
		virtual						~Texture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		
		uint32_t					GetWidth				()				{return Width;};
		uint32_t					GetHeight				()				{return Height;};

		void						SetFilter				(uint32_t aOn);
		
	protected:
		uint32_t*					Pixels;
		uint32_t					Offset;
	
		uint32_t					Width;
		uint32_t					Height;
		
		uint32_t					Filter;
};

class								PS3Video
{
	friend class					Texture;

	public:	
		static void					Init					();
		static void					Quit					();
								
		static uint32_t				GetScreenWidth			()				{return Resolution.width;};
		static uint32_t				GetScreenHeight			()				{return Resolution.height;};
		static bool					IsWideScreen			()				{return true;};
	
		static void					SetClip					(Area aClip);
		static Area					GetClip					();
	
		static void					Flip					();
		
		static void					PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF);
		static void					FillRectangle			(Area aArea, uint32_t aColor);
		static void					PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, uint32_t aUnderscan);
		
	protected:
		static void					PrepareBuffer			();
		static void					Allocate				(uint32_t*& aMemory, uint32_t& aOffset, uint32_t aSize, uint32_t aAlign);
		static void					ApplyTexture			(Texture* aTexture, Area aRegion);
		static void					ApplyVertexBuffer		(uint32_t aPosition);
		static void					DrawQuad				(Area aRegion, uint32_t aColor);

		static gcmContextData*		GCMContext;
		static VideoResolution		Resolution;
		static uint32_t				Aspect;
		
		static Area					Clip;
		
		static uint32_t*			VertexBuffer[2];
		static uint32_t				VertexBufferOffset[2];
		static uint32_t				VertexBufferPosition;

		static Texture*				FillerTexture;
		
		static uint32_t*			Screen[3];
		static uint32_t				ScreenOffset[3];
		static uint32_t				NextBuffer;
};

#endif
