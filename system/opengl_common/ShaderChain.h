#pragma once

#include "Vertex.h"
namespace LibESGL
{
	class							Program;

	class							ShaderChain
	{
		public:
									ShaderChain				(const std::string& aFileName, bool aSmooth, uint32_t aScaleFactor);
									~ShaderChain			();

			void					Apply					();
			void					SetViewport				(float aLeft, float aRight, float aTop, float aBottom);
			void					Set						(const Area& aOutput, uint32_t aInWidth, uint32_t aInHeight, uint32_t aTextureWidth, uint32_t aTextureHeight);
			void					Present					(GLuint aSourceTexture, GLuint aBorderTexture = 0);

			void					SetNext					(ShaderChain* aNext) {Next = aNext;}
			void					AttachNext				(ShaderChain* aNext) {if(Next) Next->AttachNext(aNext); else Next = aNext;};
			ShaderChain*			GetNext					() {return Next;}

		public:
			static ShaderChain*		MakeChainFromPreset		(const std::string& aFile, uint32_t aPrescale);

		private:
			ShaderChain*			Next;
			Program*				ShaderProgram;

			Area					Output;

			uint32_t				InWidth;
			uint32_t				InHeight;
			uint32_t				TextureWidth;
			uint32_t				TextureHeight;
			uint32_t				FrameCount;

			FrameBuffer*			RenderTarget;
			Vertex					VertexBuffer[4];

			uint32_t				ScaleFactor;
			bool					Smooth;

			float					Viewport[4];
	};
}

