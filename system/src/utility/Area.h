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


