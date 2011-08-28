#pragma once

//Forward declare CG types
#define CG_TYPE(x)					struct _##x; typedef struct _##x* x;
CG_TYPE(CGcontext)

class								CgProgram;

class								GLShader
{
	public:
									GLShader				(CGcontext& aContext, const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor);
									~GLShader				();

		void						Apply					();
		void						SetViewport				(float aLeft, float aRight, float aTop, float aBottom);
		void						Set						(const Area& aOutput, uint32_t aInWidth, uint32_t aInHeight, uint32_t aTextureWidth, uint32_t aTextureHeight);
		void						Present					(GLuint aSourceTexture, GLuint aBorderTexture = 0);

		void						SetNext					(GLShader* aNext) {Next = aNext;}
		void						AttachNext				(GLShader* aNext) {if(Next) Next->AttachNext(aNext); else Next = aNext;};
		GLShader*					GetNext					() {return Next;}

	public:
		static inline void			ApplyVertexBuffer		(GLfloat* aBuffer, bool aColors, bool aBorder = false);
		static GLShader*			MakeChainFromPreset		(CGcontext& aContext, const std::string& aFile, uint32_t aPrescale);

	private:
		GLShader*					Next;
		CgProgram*					Program;

		Area						Output;

		uint32_t					InWidth;
		uint32_t					InHeight;
		uint32_t					TextureWidth;
		uint32_t					TextureHeight;
		uint32_t					FrameCount;

		FrameBuffer*				RenderTarget;
		GLfloat						VertexBuffer[20];

		uint32_t					ScaleFactor;
		bool						Smooth;

		float						Viewport[4];
};

//Inlines
inline void							GLShader::ApplyVertexBuffer	(GLfloat* aBuffer, bool aColors, bool aBorder)
{
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, (aColors ? 9 : 5) * sizeof(GLfloat), &aBuffer[0]);
		glTexCoordPointer(2, GL_FLOAT, (aColors ? 9 : 5) * sizeof(GLfloat), &aBuffer[3]);

		if(aBorder)
		{
			glClientActiveTexture(GL_TEXTURE1);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			float tacos[] = {0, 0, 1, 0, 1, 1, 0, 1};
			glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(float), tacos);
			glClientActiveTexture(GL_TEXTURE0);
		}
		else
		{
			glClientActiveTexture(GL_TEXTURE1);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glClientActiveTexture(GL_TEXTURE0);
		}

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

