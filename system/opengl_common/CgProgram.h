#pragma once

//Forward declare CG types
#define CG_TYPE(x)					struct _##x; typedef struct _##x* x;
CG_TYPE(CGcontext)
CG_TYPE(CGprogram)
CG_TYPE(CGparameter)

class								CgProgram
{
	public:
									CgProgram				(CGcontext& aContext, const std::string& aFileName);

		void						Apply					(uint32_t aInWidth, uint32_t aInHeight, uint32_t aTextureWidth, uint32_t aTextureHeight, uint32_t aOutWidth, uint32_t aOutHeight, uint32_t aFrameCount);

		//Doc note: Disables any states enabled by apply, yes the name is dumb
		void						Unapply					();

		bool						Valid					();

	public:
		static CgProgram*			Get						(CGcontext& aContext, const std::string& aFileName);

	private:
		CGcontext&					Context;

		CGprogram					VertexProgram;
		CGprogram					FragmentProgram;

		CGparameter					Projection;
		CGparameter					FragmentVideoSize;
		CGparameter					FragmentTextureSize;
		CGparameter					FragmentOutputSize;
		CGparameter					FragmentFrameCount;
		CGparameter					VertexVideoSize;
		CGparameter					VertexTextureSize;
		CGparameter					VertexOutputSize;
		CGparameter					VertexFrameCount;
};

