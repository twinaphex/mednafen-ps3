#pragma once

class								DorkShader
{
	public:
									DorkShader				(CGcontext& aContext, const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor);
									~DorkShader				();

		void						Apply					();
		void						Set						(const Area& aOutput, uint32_t aInWidth, uint32_t aInHeight);
		void						Present					(GLuint aSourceTexture);

		void						SetNext					(DorkShader* aNext) {Next = aNext;}
		DorkShader*					GetNext					() {return Next;}

	public:
		static DorkShader*			MakeChainFromPreset		(CGcontext& aContext, const std::string& aFile);

	private:
		CGcontext&					Context;
		DorkShader*					Next;

		Area						Output;

		uint32_t					InWidth;
		uint32_t					InHeight;

		GLuint						TextureID;
		GLuint						FrameBufferID;
		GLfloat						VertexBuffer[20];	//TODO: Use AttributeSets

		uint32_t					ScaleFactor;
		bool						Smooth;

		CGprogram					VertexProgram;
		CGprogram					FragmentProgram;

		CGparameter					Projection;
		CGparameter					FragmentVideoSize;
		CGparameter					FragmentTextureSize;
		CGparameter					FragmentOutputSize;
		CGparameter					VertexVideoSize;
		CGparameter					VertexTextureSize;
		CGparameter					VertexOutputSize;
};

class								DorkVideo : public ESVideo
{
	public:	
									DorkVideo				();
									~DorkVideo				();
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic) {return new DorkTexture(aWidth, aHeight);};
	
		virtual void				SetClip					(const Area& aClip);
	
		void						Flip					();
		
		virtual void				PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor); //External
		virtual void				FillRectangle			(const Area& aArea, uint32_t aColor); //External
		virtual void				PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)); //External
		
	public:
		inline static void			ApplyVertexBuffer		(GLfloat* aBuffer, bool aColors); //Implemented below

	protected:
		static const uint32_t		VertexSize = 9;
		static const uint32_t		VertexBufferCount = 4;

		PSGLdevice*					Device;
		PSGLcontext*				Context;
		CGcontext					ShaderContext;

		GLfloat*					VertexBuffer;

		Texture*					FillerTexture;
};

//Inlines
inline void					DorkVideo::ApplyVertexBuffer	(GLfloat* aBuffer, bool aColors)
{
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, (aColors ? 9 : 5) * sizeof(GLfloat), &aBuffer[0]);
		glTexCoordPointer(2, GL_FLOAT, (aColors ? 9 : 5) * sizeof(GLfloat), &aBuffer[3]);

		if(aColors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 9 * sizeof(GLfloat), &aBuffer[5]);
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}
	}
}
