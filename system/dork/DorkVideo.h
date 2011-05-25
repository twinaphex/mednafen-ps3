#pragma once

#include "../opengl_common/Shaders.h"

class								DorkVideo : public ESVideo
{
	public:	
									DorkVideo				();
									~DorkVideo				();
	
		void						EnableVsync				(bool aOn);

		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic) {return new DorkTexture(aWidth, aHeight);};
	
		virtual void				SetClip					(const Area& aClip);
	
		void						Flip					();
		
		virtual void				PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor); //External
		virtual void				FillRectangle			(const Area& aArea, uint32_t aColor); //External
		virtual void				PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)); //External
		
		virtual void				SetFilter				(const std::string& aName, uint32_t aPrescale) {delete Presenter; Presenter = GLShader::MakeChainFromPreset(ShaderContext, aName, aPrescale);};

	protected:
		static const uint32_t		VertexSize = 9;
		static const uint32_t		VertexBufferCount = 4;

		PSGLdevice*					Device;
		PSGLcontext*				Context;
		CGcontext					ShaderContext;

		GLfloat*					VertexBuffer;

		Texture*					FillerTexture;
		GLShader*					Presenter;
};

