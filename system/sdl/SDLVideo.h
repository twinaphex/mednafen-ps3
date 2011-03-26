#ifndef SDLVIDEO_H
#define	SDLVIDEO_H

class								SDLVideo;

class								SDLTexture : public Texture
{
	friend class					SDLVideo;

	public:
									SDLTexture				(uint32_t aWidth, uint32_t aHeight);
		virtual						~SDLTexture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		void						Invalidate				() {Valid = false;};
		
		uint32_t					GetWidth				() {return Width;};
		uint32_t					GetHeight				() {return Height;};
		uint32_t					GetPitch				() {return Width;};

		void						SetFilter				(uint32_t aOn) {Filter = aOn ? 1 : 0;};

		uint32_t					GetFlags				() const {return 0;};
		uint32_t					GetRedShift				() const {return 16;};
		uint32_t					GetGreenShift			() const {return 8;};
		uint32_t					GetBlueShift			() const {return 0;};
		uint32_t					GetAlphaShift			() const {return 24;};
		
	protected:
		void						Apply					();

		uint32_t*					Pixels;
		uint32_t					ID;
		bool						Valid;
	
		uint32_t					Width;
		uint32_t					Height;
		
		uint32_t					Filter;
};

class								SDLVideo : public ESVideo
{
	friend class					SDLTexture;

	public:	
									SDLVideo				();
									~SDLVideo				();
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic) {return new SDLTexture(aWidth, aHeight);};
	
		virtual void				SetClip					(Area aClip);
	
		void						Flip					();
		
		virtual void				PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor); //External
		virtual void				FillRectangle			(const Area& aArea, uint32_t aColor); //External
		virtual void				PresentFrame			(Texture* aTexture, const Area& aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)); //External
		
	protected:
		SDL_Surface*				Screen;
		Texture*					FillerTexture;
};

#endif
