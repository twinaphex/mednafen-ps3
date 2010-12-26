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
	
		virtual uint32_t			GetScreenWidth			()
		{
			return esScreenWidth;
		}
		
		virtual uint32_t			GetScreenHeight			()
		{
			return esScreenHeight;
		}
		
		virtual bool				IsWideScreen			() = 0;
	
		virtual void				SetClip					(Area aClip)
		{
			if(aClip.Right() > GetScreenWidth() || aClip.Bottom() > GetScreenHeight())
			{
				esClip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
			}
			else
			{
				esClip = aClip;
			}
		}
		
		virtual Area				GetClip					()
		{
			return esClip;
		}
	
		virtual void				Flip					() = 0;
		
		virtual void				PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF) = 0;
		virtual void				FillRectangle			(Area aArea, uint32_t aColor) = 0;
		virtual void				PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, uint32_t aUnderscan) = 0;

	public: //Helpers
		Area						CalculatePresentArea	(bool aAspectOverride, uint32_t aUnderscan)
		{
			Area output(0, 0, GetScreenWidth(), GetScreenHeight());
		
			double underPercent = (double)aUnderscan / 100.0;
								
			double widthP = (aAspectOverride ? GetScreenWidth() : GetScreenWidth() - ((double)GetScreenWidth() * .125)) * (underPercent / 2);
			double heightP = GetScreenHeight() * (underPercent / 2);
				
			if(aAspectOverride || !IsWideScreen())
			{
				output = Area(widthP, heightP, GetScreenWidth() - widthP * 2, GetScreenHeight() - heightP * 2);
			}
			else
			{
				uint32_t barSize = ((double)GetScreenWidth()) * .125;
				output = Area(barSize + widthP, heightP, GetScreenWidth() - barSize * 2 - widthP * 2, GetScreenHeight() - heightP * 2);
			}
			
			return output;
		}

	protected:
		Area						esClip;
		uint32_t					esScreenWidth;
		uint32_t					esScreenHeight;
};

#endif
