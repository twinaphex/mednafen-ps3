#pragma once

namespace LibESGL
{
	///Public interface for Presenter
	class										Presenter
	{
		public:
			static void							Initialize				();
			static void							Shutdown				();

			///Attach a Texture to pass to the presenter as TEXUNIT1.
			///@param aTexture Pointer to the texture to attach.
			static void							AttachBorder			(Texture* aTexture);

			///Set a new shader to be used by PresentFrame.
			///@param aName Name of shader preset file to use.
			///@param aPrescale Unused, set to 1.
			static void							SetFilter				(const std::string& aName, uint32_t aPrescale);

			static void							Present					(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort, const Area& aOutput, bool aFlip);
	};
}
