#ifndef ESVIDEO_H
#define	ESVIDEO_H

class							ESVideo;
class							Font;

class							Area
{
	public:
		int32_t					X, Y, Width, Height;
		
		int32_t					Right					()		{return X + Width;}
		int32_t					Bottom					()		{return Y + Height;}

								Area					(int32_t aX = 0, int32_t aY = 0, int32_t aWidth = 0, int32_t aHeight = 0)
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
			if(aClip.Right() > GetScreenWidth() || aClip.Bottom() > GetScreenHeight() || aClip.Width == 0 || aClip.Height == 0)
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
		
		virtual void				PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF, Area* aArea = 0) = 0;
		virtual void				FillRectangle			(Area aArea, uint32_t aColor) = 0;
		virtual void				PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)) = 0;

	public: //Helpers
		bool						CalculateClip			(Texture* aTexture, Area& aTextureArea, Area& aOutputArea)
		{
			Area clip = GetClip();
			float fwidth = (float)GetScreenWidth();
			float fheight = (float)GetScreenHeight();

			//Are we going to draw anything?
			if(aOutputArea.X > clip.Right() || aOutputArea.Right() < clip.X || aOutputArea.Y > clip.Bottom() || aOutputArea.Bottom() < clip.Y)
			{
				return false;
			}
			//Are we changing nothing?
			else if(aOutputArea.X > clip.X && aOutputArea.Right() < clip.Right() && aOutputArea.Y > clip.Y && aOutputArea.Bottom() < clip.Bottom())
			{
				return true;
			}
			else
			{
				Area clipPixels;
				clipPixels.X = (aOutputArea.X >= clip.X) ? 0 : aOutputArea.X - clip.X;
				clipPixels.Y = (aOutputArea.Y >= clip.Y) ? 0 : aOutputArea.Y - clip.Y;
				clipPixels.Width = (aOutputArea.Right() <= clip.Right()) ? 0 : clip.Right() - aOutputArea.Right();
				clipPixels.Height = (aOutputArea.Bottom() <= clip.Bottom()) ? 0 : clip.Bottom() - aOutputArea.Bottom();

				float xlP = (float)clipPixels.X / (float)aOutputArea.Width;
				float xrP = (float)clipPixels.Width / (float)aOutputArea.Width;
				float ytP = (float)clipPixels.Y / (float)aOutputArea.Height;
				float ybP = (float)clipPixels.Height / (float)aOutputArea.Height;

				float xlT = (float)aTexture->GetWidth() * xlP;
				float xrT = (float)aTexture->GetWidth() * xrP;
				float ytT = (float)aTexture->GetHeight() * ytP;
				float ybT = (float)aTexture->GetHeight() * ybP;

				aTextureArea = Area(aTextureArea.X + xlT , aTextureArea.Y + ytT, aTextureArea.Width + xrT, aTextureArea.Height + ybT);
				aOutputArea = Area(aOutputArea.X + clipPixels.X, aOutputArea.Y + clipPixels.Y, aOutputArea.Width + clipPixels.Width, aOutputArea.Height + clipPixels.Height);

				return true;
			}

		}

		Area						CalculatePresentArea	(bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
		{
			int32_t xLeft = 0, xRight = GetScreenWidth(), yTop = 0, yBottom = GetScreenHeight();
			float fwidth = (float)GetScreenWidth();
			float fheight = (float)GetScreenHeight();


			if(IsWideScreen() || !aAspectOverride)
			{
				xLeft += fwidth * .125f;
				xRight -= fwidth * .125f;
			}

			xLeft += fwidth * ((float)(aUnderscan + aUnderscanFine.X) / 200.0f);
			xRight -= fwidth * ((float)(aUnderscan + aUnderscanFine.Width) / 200.0f);
			yTop += fwidth * ((float)(aUnderscan + aUnderscanFine.Y) / 200.0f);
			yBottom -= fwidth * ((float)(aUnderscan + aUnderscanFine.Height) / 200.0f);

			return Area(xLeft, yTop, xRight - xLeft, yBottom - yTop);
		}

	protected:
		Area						esClip;
		uint32_t					esScreenWidth;
		uint32_t					esScreenHeight;
};

#endif
