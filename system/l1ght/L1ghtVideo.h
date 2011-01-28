#ifndef L1GHTVIDEO_H
#define	L1GHTVIDEO_H

class								L1ghtVideo;

class								L1ghtTexture : public Texture
{
	friend class					L1ghtVideo;

	public:
									L1ghtTexture			(uint32_t aWidth, uint32_t aHeight);
		virtual						~L1ghtTexture			();
		
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

class								L1ghtVideo : public ESVideo
{
	friend class					L1ghtTexture;

	public:	
									L1ghtVideo				();
									~L1ghtVideo				();
									
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight) {return new L1ghtTexture(aWidth, aHeight);};
							
		bool						IsWideScreen			()				{return true;};
	
		void						Flip					();
		
		void						PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF, Area* aArea = 0);
		void						FillRectangle			(Area aArea, uint32_t aColor);
		void						PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine);
		
	protected:
		void						PrepareBuffer			();
		static void					Allocate				(uint32_t*& aMemory, uint32_t& aOffset, uint32_t aSize, uint32_t aAlign);
		void						ApplyTexture			(Texture* aTexture, Area aRegion);
		void						ApplyVertexBuffer		(uint32_t aPosition);
		void						DrawQuad				(Area aRegion, uint32_t aColor);

		gcmContextData*				GCMContext;
		VideoResolution				Resolution;
		uint32_t					Aspect;
		
		uint32_t*					VertexBuffer[2];
		uint32_t					VertexBufferOffset[2];
		uint32_t					VertexBufferPosition;

		Texture*					FillerTexture;
		
		uint32_t*					Screen[3];
		uint32_t					ScreenOffset[3];
		uint32_t					NextBuffer;
};

#endif

