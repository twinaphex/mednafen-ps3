#pragma once

#include "utility/Area.h"

class										FrameBuffer;

///Static class to be implemented by a port.
class										ESVideoPlatform
{
	public:
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
		///Determine if the platform supports the shader interface. If this returns false, it is an error to call the SetFilter,
		///AttachBorder and Present functions.
		static bool							SupportsShaders			();

		///Attach a Texture to pass to the presenter as TEXUNIT1. Must not be called if SupportsShaders returns false.
		///@param aTexture Pointer to the texture to attach.
		static void							AttachBorder			(Texture* aTexture);

		///Set a new cg shader to be used by PresentFrame. Must not be called if SupportsShaders returns false.
		///@param aName Name of shader preset file to use.
		///@param aPrescale Unused, set to 1.
		static void							SetFilter				(const std::string& aName, uint32_t aPrescale);

		static void							Present					(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort, const Area& aOutput);

		///Determine if the platform supports specifing the vertical sync setting. If this returns false, it is an error to call
		///the SetVSync function.
		///@return True if supported, false otherwise.
		static bool							SupportsVSyncSelect		();

		///Determine if the platform supports specifing the screen resolution. If this returns false, it is an error to call
		///the SetMode and GetModes functions.
		///@return True if supported, false otherwise.
		static bool							SupportsModeSwitch		();

		///Determine if the platform supports a second OpenGL context with shared display lists. If this returns false, it is an error
		///to call the MakePrimaryActive and MakeSecondaryActive functions.
		static bool							SupportsSecondaryContext();

		//Doc Note: All of these must assert if not supported
		static void							SetVSync				(bool aOn);
		static void							SetMode					(uint32_t aIndex);
		static const ModeList&				GetModes				();
		static void							MakePrimaryActive		();
		static void							MakeSecondaryActive		();
};


class								ESVideo : public ESVideoPlatform
{
	public:	
		static void					Initialize				(); //External
		static void					Shutdown				(); //External

		static Texture*				CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) {return new Texture(aWidth, aHeight);};

		static void					SetScreenSize			(uint32_t aX, uint32_t aY); //External

		///Get the width of the screen.
		///@return The width of the screen.
		static uint32_t				GetScreenWidth			() {return ScreenWidth;}

		///Get the height of the screen.
		///@return The height of the screen.
		static uint32_t				GetScreenHeight			() {return ScreenHeight;}

		static bool					IsWideScreen			() {return WideScreen;}

		///Set the current clip Area. The clip area is respected by the PlaceTexture and FillRectangle functions, but not by PresentFrame.
		///@param aClip The new clip area, if any portion of the Area is outside the bounds of the screen the clip Area is set to fill the
		///entire screen.
		static inline void			SetClip					(const Area& aClip); //Below

		///Retrive the current clip Area. The clip area is respected by the PlaceTexture and FillRectangle functions, but not by PresentFrame.
		///@return The current Area outside of which pixels will not be drawn.
		static const Area&			GetClip					() {return Clip;}

		///Flip the screen buffers and reset clipping to cover the entire screen.
		static void					Flip					();
		
		///Make a FrameBuffer object the render target.
		///@param aFrameBuffer Frame buffer to set as the render target. If null the default target will be restored.
		static void					SetRenderTarget			(FrameBuffer* aBuffer);

		///Blit a porition of a Texture to the screen. The Texture's alpha channel will be respected. The Texture may be stretched and color modulated.
		///@param aTexture Pointer to the Texture to blit.
		///@param aDestination Screen area to place the Texture.
		///@param aSource Source porition of the Texture to blit, in pixels.
		///@param aColor Color used to 'modulate' the textures color.
		static void					PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor);

		///Fill a given area of the screen with a solid color.
		///@param aArea Area of the screen to fill.
		///@param aColor Color to fill with.
		static void					FillRectangle			(const Area& aArea, uint32_t aColor) {PlaceTexture(FillerTexture, aArea, Area(0, 0, 2, 2), aColor);}



		///Present a Texture to the screen. The texture will fill the entire screen and may be passed through a Cg fragment shader.
		///@param aTexture Pointer to the texture to draw.
		///@param aViewPort Portion, in pixels, of the texture that should be drawn.
		static void					PresentFrame			(Texture* aTexture, const Area& aViewPort); //External

		///Present a FrameBuffer to the screen. The FrameBuffer will fill the entire screen and may be passed through a Cg fragment shader.
		///@param aFrameBuffer Pointer to the FrameBuffer to draw.
		///@param aViewPort Portion, in pixels, of the FrameBuffer that should be drawn.
		static void					PresentFrame			(FrameBuffer* aFrameBuffer, const Area& aViewPort); //External

		///Update the output Area that will be used by PresentFrame.
		///@param aAspectOverride Override aspect ratio correction. If -1 the image will be drawn to cover the entire screen, if 1 the image will have bars along the
		///side and 0 will auto-detect the method to use.
		///@param aUnderscan Amount in percent that the image will be shrunk to compensate for over scan on certain televisions.
		///@param aUnderscanFine Amount in percent that the image will be adjusted on each side in addition to the aUnderscan value.
		static void					UpdatePresentArea		(int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0));

		
	private:
		static void					PresentFrame			(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort);

		static void					SetVertex				(GLfloat* aBase, float aX, float aY, float aR, float aG, float aB, float aA, float aU, float aV);
		static void					InitializeState			();
		static void					EnterPresentState		();
		static void					ExitPresentState		();


	private:
		static Texture*				FillerTexture;					///<Small solid white Texture used by FillRectangle.
		static Area					Clip;							///<Area, outside of which PlaceTexture and FillRectangle will not draw.

		static Area					PresentArea;					///<Area that will be used for output by PresentFrame.

		static GLuint				FrameBufferID;					///<ID of the FrameBuffer object.

		static const uint32_t		VertexBufferCount = 4;			///<Number of vertices in the Vertex buffer.
		static const uint32_t		VertexSize = 9;					///<Number of GLfloat entries per vertex.
		static GLfloat*				VertexBuffer;					///<Buffer used to store OpenGL vertices. (VertexBufferCount * VertexSize * sizeof(GLfloat) bytes long)

		static uint32_t				ScreenWidth;					///<Width of the screen.
		static uint32_t				ScreenHeight;					///<Height of the screen.
		static bool					WideScreen;						///<True if the display has a wide aspect ratio.

		static bool					NoAspect;						///<Use a 1:1 scaling with no aspect correction.
};

//---Inlines
void								ESVideo::SetClip		(const Area& aClip)
{
	Clip = aClip.Valid(GetScreenWidth(), GetScreenHeight()) ? aClip : Area(0, 0, GetScreenWidth(), GetScreenHeight());
	glScissor(Clip.X, GetScreenHeight() - Clip.Bottom(), Clip.Width, Clip.Height);
}

