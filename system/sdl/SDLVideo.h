#pragma once

class	SDLShaderProgram;
typedef std::map<std::string, SDLShaderProgram*>	ShaderMap;
class								SDLShaderProgram
{
	public:
									SDLShaderProgram		(CGcontext& aContext, const std::string& aFileName);
		void						Apply					(uint32_t aInWidth, uint32_t aInHeight, uint32_t aOutWidth, uint32_t aOutHeight);

	public:
		static SDLShaderProgram*	Get						(CGcontext& aContext, const std::string& aFileName);

	private:
		static ShaderMap			Shaders;

		CGcontext&					Context;

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

class								SDLShader
{
	public:
									SDLShader				(CGcontext& aContext, const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor);
									~SDLShader				();

		void						Apply					();
		void						SetViewport				(float aLeft, float aRight, float aTop, float aBottom);
		void						Set						(const Area& aOutput, uint32_t aInWidth, uint32_t aInHeight);
		void						Present					(GLuint aSourceTexture);

		void						SetNext					(SDLShader* aNext) {Next = aNext;}
		void						AttachNext				(SDLShader* aNext) {if(Next) Next->AttachNext(aNext); else Next = aNext;};
		SDLShader*					GetNext					() {return Next;}

	public:
		static SDLShader*			MakeChainFromPreset		(CGcontext& aContext, const std::string& aFile, uint32_t aPrescale);

	private:
		CGcontext&					Context;
		SDLShader*					Next;
		SDLShaderProgram*			Program;

		Area						Output;

		uint32_t					InWidth;
		uint32_t					InHeight;

		GLuint						TextureID;
		GLuint						FrameBufferID;
		GLfloat						VertexBuffer[20];	//TODO: Use AttributeSets

		uint32_t					ScaleFactor;
		bool						Smooth;

		float						Viewport[4];
};

class								SDLVideo : public ESVideo
{
	public:	
									SDLVideo				();
									~SDLVideo				();
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic) {return new SDLTexture(aWidth, aHeight);};
	
		virtual void				SetClip					(const Area& aClip);
	
		void						Flip					();
		
		virtual void				PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor); //External
		virtual void				FillRectangle			(const Area& aArea, uint32_t aColor); //External
		virtual void				PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine = Area(0, 0, 0, 0)); //External

		virtual void				SetFilter				(const std::string& aName, uint32_t aPrescale) {delete Presenter; Presenter = SDLShader::MakeChainFromPreset(ShaderContext, aName, aPrescale);};

	public:
		inline static void			ApplyVertexBuffer		(GLfloat* aBuffer, bool aColors); //Implemented below
		
	protected:
		SDL_Surface*				Screen;
		Texture*					FillerTexture;

		CGcontext					ShaderContext;
		SDLShader*					Presenter;
		static const uint32_t		VertexSize = 9;
		static const uint32_t		VertexBufferCount = 4;
		GLfloat*					VertexBuffer;
};

//Inlines
inline void					SDLVideo::ApplyVertexBuffer	(GLfloat* aBuffer, bool aColors)
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

