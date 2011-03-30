#pragma once

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

		bool					operator==				(const Area& aB) const
		{
			return (X == aB.X && Y == aB.Y && Width == aB.Width && Height == aB.Height);
		}	

		bool					operator!=				(const Area& aB) const
		{
			return !(X == aB.X && Y == aB.Y && Width == aB.Width && Height == aB.Height);
		}	
};

class								ESVideo
{
	public:	
									ESVideo								() : LastAspect(0), LastUnderscan(0), LastUnderscanFine(0, 0, 0, 0), PresentArea(0, 0, 0, 0) {};
		virtual						~ESVideo							() {};
	
		virtual Texture*			CreateTexture						(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) = 0; //Pure Virtual
	
		virtual uint32_t			GetScreenWidth						() const {return esScreenWidth;}
		virtual uint32_t			GetScreenHeight						() const {return esScreenHeight;}
		virtual bool				IsWideScreen						() const {return esWideScreen;}
	
		virtual inline void			SetClip								(Area aClip); //Defined Below
		virtual Area				GetClip								() const {return esClip;}
	
		virtual void				Flip								() = 0; //Pure Virtual
		
		virtual void				PlaceTexture						(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor) = 0; //Pure Virtual
		virtual void				FillRectangle						(const Area& aArea, uint32_t aColor) = 0; //Pure Virtual
		virtual void				PresentFrame						(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)) = 0; //Pure Virtual

	public: //Helpers
		inline Area					CalculatePresentArea				(int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine); //Defined below

	protected:
		Area						esClip;
		uint32_t					esScreenWidth;
		uint32_t					esScreenHeight;
		bool						esWideScreen;

	private:
		int32_t						LastAspect;
		int32_t						LastUnderscan;
		Area						LastUnderscanFine;
		Area						PresentArea;
};

//---Inlines
void								ESVideo::SetClip					(Area aClip)
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

Area								ESVideo::CalculatePresentArea		(int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	if(aAspectOverride != LastAspect || LastUnderscan != aUnderscan || aUnderscanFine != LastUnderscanFine)
	{
		LastAspect = aAspectOverride;
		LastUnderscan = aUnderscan;
		LastUnderscanFine = aUnderscanFine;

		int32_t xLeft = 0, xRight = GetScreenWidth(), yTop = 0, yBottom = GetScreenHeight();
		float fwidth = (float)GetScreenWidth();
		float fheight = (float)GetScreenHeight();

		if((LastAspect == 0 && IsWideScreen()) || (LastAspect < 0))
		{
			xLeft += fwidth * .125f;
			xRight -= fwidth * .125f;
			fwidth -= fwidth * .250f;
		}

		xLeft += (fwidth * ((float)(LastUnderscan + LastUnderscanFine.X) / 200.0f));
		xRight -= (fwidth * ((float)(LastUnderscan + LastUnderscanFine.Width) / 200.0f));
		yTop += (fheight * ((float)(LastUnderscan + LastUnderscanFine.Y) / 200.0f));
		yBottom -= (fheight * ((float)(LastUnderscan + LastUnderscanFine.Height) / 200.0f));

		PresentArea = Area(xLeft, yTop, xRight - xLeft, yBottom - yTop);
	}

	return PresentArea;
}
