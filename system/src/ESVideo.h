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

#include "../opengl_common/Shaders.h"

class								ESVideo
{
	public:	
		static void					Initialize				(); //External
		static void					Shutdown				(); //External

		static void					EnableVsync				(bool aOn) {}
	
		static Texture*				CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) {return new Texture(aWidth, aHeight);};
	
		static void					SetScreenSize			(uint32_t aX, uint32_t aY); //External
		static uint32_t				GetScreenWidth			() {return ScreenWidth;}
		static uint32_t				GetScreenHeight			() {return ScreenHeight;}
		static bool					IsWideScreen			() {return WideScreen;}

		static inline void			SetClip					(const Area& aClip); //Below
		static const Area&			GetClip					() {return Clip;}
	
		static void					Flip					(); //External
		
		static void					PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor);
		static void					FillRectangle			(const Area& aArea, uint32_t aColor) {PlaceTexture(FillerTexture, aArea, Area(0, 0, 2, 2), aColor);}
		static void					AttachBorder			(Texture* aTexture) {Border = aTexture;};
		static void					PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)); //External

		static void					SetFilter				(const std::string& aName, uint32_t aPrescale) {delete Presenter; Presenter = GLShader::MakeChainFromPreset(ShaderContext, aName, aPrescale);};
		
	private:
		static void					SetVertex				(GLfloat* aBase, float aX, float aY, float aR, float aG, float aB, float aA, float aU, float aV);
		static void					InitializeState			();
		static void					EnterPresentState		();
		static void					ExitPresentState		();
		static const Area&			CalculatePresentArea	(int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine);


	private:
		static Texture*				FillerTexture;

		static CGcontext			ShaderContext;
		static GLShader*			Presenter;
		static const uint32_t		VertexSize = 9;
		static const uint32_t		VertexBufferCount = 4;
		static GLfloat*				VertexBuffer;

		static uint32_t				ScreenWidth;
		static uint32_t				ScreenHeight;
		static bool					WideScreen;
		static Area					Clip;
		static Texture*				Border;
};

//---Inlines
void								ESVideo::SetClip		(const Area& aClip)
{
	Clip = aClip.Valid(GetScreenWidth(), GetScreenHeight()) ? aClip : Area(0, 0, GetScreenWidth(), GetScreenHeight());
	glScissor(Clip.X, GetScreenHeight() - Clip.Bottom(), Clip.Width, Clip.Height);
}


