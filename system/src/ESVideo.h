#ifndef ESVIDEO_H
#define	ESVIDEO_H

class							ESVideo;
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
	public:
		virtual void				Clear					(uint32_t aColor) = 0;
		virtual uint32_t*			GetPixels				() = 0;
		
		virtual uint32_t			GetWidth				() = 0;
		virtual uint32_t			GetHeight				() = 0;

		virtual void				SetFilter				(uint32_t aOn) = 0;
};

class								ESVideo
{
	public:	
		virtual						~ESVideo				(){};
	
		virtual Texture*			CreateTexture			(uint32_t aWidth, uint32_t aHeight) = 0;
	
		virtual uint32_t			GetScreenWidth			() = 0;
		virtual uint32_t			GetScreenHeight			() = 0;
		virtual bool				IsWideScreen			() = 0;
	
		virtual void				SetClip					(Area aClip) = 0;
		virtual Area				GetClip					() = 0;
	
		virtual void				Flip					() = 0;
		
		virtual void				PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF) = 0;
		virtual void				FillRectangle			(Area aArea, uint32_t aColor) = 0;
		virtual void				PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, uint32_t aUnderscan) = 0;
};

#endif
