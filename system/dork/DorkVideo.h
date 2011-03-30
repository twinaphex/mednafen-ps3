#pragma once

class								DorkVideo : public ESVideo
{
	public:	
									DorkVideo				();
									~DorkVideo				();
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic) {return new DorkTexture(aWidth, aHeight);};
	
		virtual void				SetClip					(Area aClip);
	
		void						Flip					();
		
		virtual void				PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor); //External
		virtual void				FillRectangle			(const Area& aArea, uint32_t aColor); //External
		virtual void				PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)); //External
		
	protected:
		PSGLdevice*					Device;
		PSGLcontext*				Context;

		Texture*					FillerTexture;
};
