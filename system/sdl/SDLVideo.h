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
		
		uint32_t					GetWidth				()				{return Width;};
		uint32_t					GetHeight				()				{return Height;};

		void						SetFilter				(uint32_t aOn);
		
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
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight) {return new SDLTexture(aWidth, aHeight);};
	
		bool						IsWideScreen			()				{return true;};

		virtual void				SetClip					(Area aClip);
	
		void						Flip					();
		
		void						PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF, Area* aArea = 0);
		void						FillRectangle			(Area aArea, uint32_t aColor);
		void						PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine);
		
	protected:
		SDL_Surface*				Screen;
		Texture*					FillerTexture;
};

#endif
