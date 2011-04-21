#pragma once

class								L1ghtVideo : public ESVideo
{
	friend class					L1ghtTexture;

	public:	
									L1ghtVideo				();
									~L1ghtVideo				();
									
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) {return new L1ghtTexture(aWidth, aHeight);};
							
		virtual void				SetClip					(Area aClip);
	
		void						Flip					();
		
		void						PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor);
		void						FillRectangle			(const Area& aArea, uint32_t aColor);
		void						PresentFrame			(Texture* aTexture, const Area& aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0));
		
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

		uint32_t*					FragmentMemory;
		uint32_t					FragmentOffset;

		Texture*					FillerTexture;
		
		uint32_t*					Screen[3];
		uint32_t					ScreenOffset[3];
		uint32_t					NextBuffer;
};

