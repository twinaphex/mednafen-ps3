#ifndef WiiVideo_H
#define	WiiVideo_H

class								WiiVideo;

class								WiiTexture : public Texture
{
	friend class					WiiVideo;

	public:
									WiiTexture				(uint32_t aWidth, uint32_t aHeight);
		virtual						~WiiTexture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		
		uint32_t					GetWidth				() {return Width;};
		uint32_t					GetHeight				() {return Height;};

		void						SetFilter				(uint32_t aOn) {Filter = aOn ? 1 : 0;}
		
	protected:
		SDL_Surface*				Surface;
	
		uint32_t					Width;
		uint32_t					Height;
		
		uint32_t					Filter;
};

class								WiiVideo : public ESVideo
{
	friend class					WiiTexture;

	public:	
									WiiVideo				();
									~WiiVideo				();
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight) {return new WiiTexture(aWidth, aHeight);};
	
		bool						IsWideScreen			() {return true;};

		virtual void				SetClip					(Area aClip);
	
		void						Flip					();
		
		void						PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF, Area* aArea = 0);
		void						FillRectangle			(Area aArea, uint32_t aColor);
		void						PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine);
		
	protected:
		SDL_Surface*				Screen;
};

#endif
