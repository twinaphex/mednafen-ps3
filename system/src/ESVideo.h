#pragma once

#include "utility/Area.h"
#include "../opengl_common/Shaders.h"

///Static class to be implemented by a port.
class										ESVideoPlatform
{
	public:
		//Doc Note: Set Name to zero to terminate the list.
		struct								Mode
		{
			const char*						Name;
			uint32_t						ID;
		};

		typedef std::list<Mode>				ModeList;

	protected:	
		static void							Initialize				(uint32_t& aWidth, uint32_t& aHeight); //External
		static void							Shutdown				(); //External
		static void							Flip					();

	public:
		///Determine if the platform supports specifing the vertical sync setting. If this returns false, it is an error to call
		///the SetVSync function.
		///@return True if supported, false otherwise.
		static bool							SupportsVSyncSelect		();

		///Determine if the platform supports specifing the screen resolution. If this returns false, it is an error to call
		///the SetMode and GetModes functions.
		///@return True if supported, false otherwise.
		static bool							SupportsModeSwitch		();

		//Doc Note: All of these must assert if not supported
		static void							SetVSync				(bool aOn);
		static void							SetMode					(uint32_t aIndex);
		static ModeList::const_iterator		GetModes				();
};


class								ESVideo : public ESVideoPlatform
{
	public:	
		static void					Initialize				(); //External
		static void					Shutdown				(); //External

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

