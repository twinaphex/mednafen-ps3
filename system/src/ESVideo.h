#pragma once

class							Area
{
	public:
		int32_t					X, Y, Width, Height;

		int32_t					Left					() const {return X;}		
		int32_t					Right					() const {return X + Width;}
		int32_t					Top						() const {return Y;}
		int32_t					Bottom					() const {return Y + Height;}

								Area					(int32_t aX = 0, int32_t aY = 0, int32_t aWidth = 0, int32_t aHeight = 0)
		{
			X = aX;
			Y = aY;
			Width = aWidth;
			Height = aHeight;
		};

		void					Inflate					(int32_t aAmount)
		{
			X -= aAmount;
			Y -= aAmount;
			Width += aAmount * 2;
			Height += aAmount * 2;
		}

		bool					ContainsPoint			(int32_t aX, int32_t aY) const
		{
			return (aX >= X && aX < Right() && aY >= Y && aY < Bottom());
		}

		bool					Intersects				(const Area& aArea) const
		{
			return !((Right() < aArea.Left()) || (Left() > aArea.Right()) || (Bottom() < aArea.Top()) || (Top() > aArea.Bottom()));
		}

		bool					Contains				(const Area& aArea) const
		{
			return (aArea.Left() >= Left()) && (aArea.Right() <= Right()) && (aArea.Top() >= Top()) && (aArea.Bottom() <= Bottom());
		}

		bool					Valid					(uint32_t aWidth, uint32_t aHeight) const
		{
			return (X >= 0) && (Y >= 0) && (Width > 0) && (Height > 0) && (Right() <= aWidth) && (Bottom() <= aHeight);
		}

		bool					operator==				(const Area& aB) const
		{
			return (X == aB.X && Y == aB.Y && Width == aB.Width && Height == aB.Height);
		}	

		bool					operator!=				(const Area& aB) const
		{
			return !(X == aB.X && Y == aB.Y && Width == aB.Width && Height == aB.Height);
		}	

		friend std::ostream&	operator<<				(std::ostream& aStream, const Area& aA)
		{
			aStream << aA.X << " " << aA.Y << " " << aA.Width << " " << aA.Height;
			return aStream;
		}

		friend std::istream&	operator>>				(std::istream& aStream, Area& aA)
		{
			aStream >> aA.X >> aA.Y >> aA.Width >> aA.Height;
			return aStream;
		}
};


#if 0

class								ESVideo
{
	public:	
									ESVideo								() : esBorder(0), LastAspect(0), LastUnderscan(0), LastUnderscanFine(0, 0, 0, 0), PresentArea(0, 0, 0, 0) {};
		virtual						~ESVideo							() {};

		virtual void				EnableVsync							(bool aOn) {};
	
		virtual Texture*			CreateTexture						(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) = 0; //Pure Virtual
	
		virtual uint32_t			GetScreenWidth						() const {return esScreenWidth;}
		virtual uint32_t			GetScreenHeight						() const {return esScreenHeight;}
		virtual bool				IsWideScreen						() const {return esWideScreen;}
	
		virtual inline void			SetClip								(const Area& aClip); //Defined Below
		virtual const Area&			GetClip								() const {return esClip;}
	
		virtual void				Flip								() = 0; //Pure Virtual
		
		virtual void				PlaceTexture						(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor) = 0; //Pure Virtual
		virtual void				FillRectangle						(const Area& aArea, uint32_t aColor) = 0; //Pure Virtual
		virtual void				AttachBorder						(Texture* aTexture) {esBorder = aTexture;};
		virtual void				PresentFrame						(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)) = 0; //Pure Virtual

		virtual const ESFilterList&	GetFilters							() const {return esFilters;}
		virtual void				SetFilter							(const std::string& aName, uint32_t aPrescale) {};


	protected:
		Area						esClip;
		uint32_t					esScreenWidth;
		uint32_t					esScreenHeight;
		bool						esWideScreen;

		ESFilterList				esFilters;

		Texture*					esBorder;
};

//---Inlines
void								ESVideo::SetClip					(const Area& aClip)
{
	esClip = aClip.Valid(GetScreenWidth(), GetScreenHeight()) ? aClip : Area(0, 0, GetScreenWidth(), GetScreenHeight());
}

#endif
