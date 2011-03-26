#ifndef ESVIDEO_H
#define	ESVIDEO_H

class							ESVideo;
class							Font;

class							Area
{
	public:
		int32_t					X, Y, Width, Height;
		
		int32_t					Right					() const {return X + Width;}
		int32_t					Bottom					() const {return Y + Height;}

								Area					(int32_t aX = 0, int32_t aY = 0, int32_t aWidth = 0, int32_t aHeight = 0)
		{
			X = aX;
			Y = aY;
			Width = aWidth;
			Height = aHeight;
		};
};

class								ESVideo
{
	public:	
		virtual						~ESVideo				(){};
	
		virtual Texture*			CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) = 0;
	
		virtual uint32_t			GetScreenWidth			() const {return esScreenWidth;}
		virtual uint32_t			GetScreenHeight			() const {return esScreenHeight;}
		
		virtual bool				IsWideScreen			() = 0;
	
		virtual void				SetClip					(Area aClip)
		{
			if(aClip.Right() > GetScreenWidth() || aClip.Bottom() > GetScreenHeight() || aClip.Width == 0 || aClip.Height == 0)
			{
				esClip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
			}
			else
			{
				esClip = aClip;
			}
		}
		
		virtual Area				GetClip					() const {return esClip;}
	
		virtual void				Flip					() = 0;
		
		virtual void				PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF, Area* aArea = 0) = 0;
		virtual void				FillRectangle			(Area aArea, uint32_t aColor) = 0;
		virtual void				PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)) = 0;

	public: //Helpers
		Area						CalculatePresentArea	(bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
		{
			int32_t xLeft = 0, xRight = GetScreenWidth(), yTop = 0, yBottom = GetScreenHeight();
			float fwidth = (float)GetScreenWidth();
			float fheight = (float)GetScreenHeight();


			if(IsWideScreen() && !aAspectOverride)
			{
				xLeft += fwidth * .125f;
				xRight -= fwidth * .125f;
				fwidth -= fwidth * .250f;
			}

			xLeft += (fwidth * ((float)(aUnderscan + aUnderscanFine.X) / 200.0f));
			xRight -= (fwidth * ((float)(aUnderscan + aUnderscanFine.Width) / 200.0f));
			yTop += (fheight * ((float)(aUnderscan + aUnderscanFine.Y) / 200.0f));
			yBottom -= (fheight * ((float)(aUnderscan + aUnderscanFine.Height) / 200.0f));

			return Area(xLeft, yTop, xRight - xLeft, yBottom - yTop);
		}

	protected:
		Area						esClip;
		uint32_t					esScreenWidth;
		uint32_t					esScreenHeight;
};

#endif
