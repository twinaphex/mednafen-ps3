#include <assert.h>
#include <GL/gl.h>
#include <stdio.h>

#define NO_TRANSLATE
#include "opengl_state_machine.h"

//#define LOG(s) printf("%s\n", s);
#define LOG(s)

//glEnable, glDisable
static int 					CapState[SGL_CAP_MAX];
static const int			CapTranslate[SGL_CAP_MAX] = 
{
	GL_ALPHA_TEST, GL_AUTO_NORMAL, GL_BLEND, GL_COLOR_LOGIC_OP, GL_COLOR_MATERIAL, GL_COLOR_SUM,
	GL_COLOR_TABLE, GL_CONVOLUTION_1D, GL_CONVOLUTION_2D, GL_CULL_FACE, GL_DEPTH_TEST, GL_DITHER,
	GL_FOG, GL_HISTOGRAM, GL_INDEX_LOGIC_OP, GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHTING,
	GL_LINE_SMOOTH, GL_LINE_STIPPLE, GL_MAP1_COLOR_4, GL_MAP1_INDEX, GL_MAP1_NORMAL, GL_MAP1_TEXTURE_COORD_1,
	GL_MAP1_TEXTURE_COORD_2, GL_MAP1_TEXTURE_COORD_3, GL_MAP1_TEXTURE_COORD_4, GL_MAP1_VERTEX_3,
	GL_MAP1_VERTEX_4, GL_MAP2_COLOR_4, GL_MAP2_INDEX, GL_MAP2_NORMAL, GL_MAP2_TEXTURE_COORD_1,
	GL_MAP2_TEXTURE_COORD_2, GL_MAP2_TEXTURE_COORD_3, GL_MAP2_TEXTURE_COORD_4, GL_MAP2_VERTEX_3,
	GL_MAP2_VERTEX_4, GL_MINMAX, GL_MULTISAMPLE, GL_NORMALIZE, GL_POINT_SMOOTH, GL_POINT_SPRITE,
	GL_POLYGON_OFFSET_FILL, GL_POLYGON_OFFSET_LINE, GL_POLYGON_OFFSET_POINT, GL_POLYGON_SMOOTH,
	GL_POLYGON_STIPPLE, GL_POST_COLOR_MATRIX_COLOR_TABLE, GL_POST_CONVOLUTION_COLOR_TABLE,
	GL_RESCALE_NORMAL, GL_SAMPLE_ALPHA_TO_COVERAGE, GL_SAMPLE_ALPHA_TO_ONE, GL_SAMPLE_COVERAGE,
	GL_SEPARABLE_2D, GL_SCISSOR_TEST, GL_STENCIL_TEST, GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
	GL_TEXTURE_CUBE_MAP, GL_TEXTURE_GEN_Q, GL_TEXTURE_GEN_R, GL_TEXTURE_GEN_S, GL_TEXTURE_GEN_T,
	GL_VERTEX_PROGRAM_POINT_SIZE, GL_VERTEX_PROGRAM_TWO_SIDE
};

void						sglEnable				(GLenum cap)
{
	LOG(__func__);

	if(cap >= SGL_CAP_MAX)
	{
		printf("%08X\n", cap);
	}

	assert(cap < SGL_CAP_MAX);

	if(!CapState[cap])
	{
		glEnable(CapTranslate[cap]);
		CapState[cap] = 1;
	}
}

void						sglDisable				(GLenum cap)
{
	LOG(__func__);

	if(cap >= SGL_CAP_MAX)
	{
		printf("%08X\n", cap);
	}

	assert(cap < SGL_CAP_MAX);

	if(CapState[cap])
	{
		glDisable(CapTranslate[cap]);
		CapState[cap] = 0;
	}
}

GLboolean					sglIsEnabled			(GLenum cap)
{
	LOG(__func__);

	if(cap >= SGL_CAP_MAX)
	{
		printf("%08X\n", cap);
	}

	assert(cap < SGL_CAP_MAX);
	return CapState[cap] ? GL_TRUE : GL_FALSE;
}

//CLIENT STATE
static int 					ClientCapState[SGL_CLIENT_CAP_MAX];
static const int			ClientCapTranslate[SGL_CLIENT_CAP_MAX] = 
{
	GL_COLOR_ARRAY, GL_EDGE_FLAG_ARRAY, GL_FOG_COORD_ARRAY, GL_INDEX_ARRAY, GL_NORMAL_ARRAY, GL_SECONDARY_COLOR_ARRAY,
	GL_TEXTURE_COORD_ARRAY, GL_VERTEX_ARRAY
};

void sglEnableClientState(GLenum cap)
{
	LOG(__func__);

	if(cap >= SGL_CLIENT_CAP_MAX)
	{
		printf("%08X\n", cap);
	}

	assert(cap < SGL_CLIENT_CAP_MAX);

	if(!ClientCapState[cap])
	{
		glEnableClientState(ClientCapTranslate[cap]);
		ClientCapState[cap] = 1;
	}
}

void sglDisableClientState(GLenum cap)
{
	LOG(__func__);

	if(cap >= SGL_CLIENT_CAP_MAX)
	{
		printf("%08X\n", cap);
	}

	assert(cap < SGL_CLIENT_CAP_MAX);

	if(ClientCapState[cap])
	{
		glDisableClientState(ClientCapTranslate[cap]);
		ClientCapState[cap] = 0;
	}
}

//VERTEX POINTER
static GLint VertexPointer_size;
static GLenum VertexPointer_type;
static GLsizei VertexPointer_stride;
static const GLvoid* VertexPointer_pointer;
void sglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	LOG(__func__);

	VertexPointer_size = size;
	VertexPointer_type = type;
	VertexPointer_stride = stride;
	VertexPointer_pointer = pointer;
	glVertexPointer(size, type, stride, pointer);
}

//COLOR POINTER
static GLint ColorPointer_size;
static GLenum ColorPointer_type;
static GLsizei ColorPointer_stride;
static const GLvoid* ColorPointer_pointer;
void sglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	LOG(__func__);

	ColorPointer_size = size;
	ColorPointer_type = type;
	ColorPointer_stride = stride;
	ColorPointer_pointer = pointer;
	glColorPointer(size, type, stride, pointer);
}

//SECONDARY COLOR POINTER
static GLint SecondaryColorPointer_size;
static GLenum SecondaryColorPointer_type;
static GLsizei SecondaryColorPointer_stride;
static const GLvoid* SecondaryColorPointer_pointer;
void sglSecondaryColorPointerEXTs(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	LOG(__func__);

	SecondaryColorPointer_size = size;
	SecondaryColorPointer_type = type;
	SecondaryColorPointer_stride = stride;
	SecondaryColorPointer_pointer = pointer;
	glSecondaryColorPointerEXT(size, type, stride, pointer);
}

//TEXCOORD POINTER
static GLint TexCoordPointer_size;
static GLenum TexCoordPointer_type;
static GLsizei TexCoordPointer_stride;
static const GLvoid* TexCoordPointer_pointer;
void sglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	LOG(__func__);

	TexCoordPointer_size = size;
	TexCoordPointer_type = type;
	TexCoordPointer_stride = stride;
	TexCoordPointer_pointer = pointer;
	glTexCoordPointer(size, type, stride, pointer);
}

//ALPHA FUNC
static GLenum AlphaFunc_func;
static GLclampf AlphaFunc_ref;
void sglAlphaFunc(GLenum func, GLclampf ref)
{
	LOG(__func__);

	AlphaFunc_func = func;
	AlphaFunc_ref = ref;
	glAlphaFunc(func, ref);
}

//BLEND FUNC
static GLenum BlendFunc_sfactor, BlendFunc_dfactor;
void sglBlendFunc(GLenum sfactor, GLenum dfactor)
{
	LOG(__func__);

	BlendFunc_sfactor = sfactor;
	BlendFunc_dfactor = dfactor;
	glBlendFunc(sfactor, dfactor);
}

//POLYGON MODE
static GLenum PolygonMode_ModeList[2];
void sglPolygonMode(GLenum face, GLenum mode)
{
	LOG(__func__);

	if(face == GL_FRONT || face == GL_FRONT_AND_BACK)
	{
		PolygonMode_ModeList[0] = mode;
	}

	if(face == GL_BACK || face == GL_FRONT_AND_BACK)
	{
		PolygonMode_ModeList[1] = mode;
	}

	glPolygonMode(face, mode);
}

//VIEWPORT
static GLint Viewport_x, Viewport_y;
static GLsizei Viewport_width, Viewport_height;
void sglViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	LOG(__func__);

	Viewport_x = x;
	Viewport_y = y;
	Viewport_width = width;
	Viewport_height = height;
	glViewport(x, y, width, height);
}

//CLEAR COLOR
static GLclampf ClearColor_red, ClearColor_green, ClearColor_blue, ClearColor_alpha;
void sglClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	LOG(__func__);

	ClearColor_red = red;
	ClearColor_green = green;
	ClearColor_blue = blue;
	ClearColor_alpha = alpha;

	glClearColor(red, green, blue, alpha);
}

//CULL FACE
static GLenum CullFace_mode;
void sglCullFace(GLenum mode)
{
	LOG(__func__);

	CullFace_mode = mode;
	glCullFace(mode);
}

//DEPTH FUNC
static GLenum DepthFunc_func;
void sglDepthFunc(GLenum func)
{
	LOG(__func__);

	DepthFunc_func = func;
	glDepthFunc(func);
}

//DEPTH MASK
static GLboolean DepthMask_flag;
void sglDepthMask(GLboolean flag)
{
	LOG(__func__);

	DepthMask_flag = flag;
	glDepthMask(flag);
}

//DEPTH RANGE
static GLclampd DepthRange_nearVal, DepthRange_farVal;
void sglDepthRange(GLclampd nearVal, GLclampd farVal)
{
	LOG(__func__);

	DepthRange_nearVal = nearVal;
	DepthRange_farVal = farVal;
	glDepthRange(nearVal, farVal);
}

//SCISSOR
static GLint Scissor_x, Scissor_y;
static GLsizei Scissor_width, Scissor_height;
void sglScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	LOG(__func__);

	Scissor_x = x;
	Scissor_y = y;
	Scissor_width = width;
	Scissor_height = height;
	glScissor(x, y, width, height);
}

//ACTIVE TEXTURE
static GLenum ActiveTexture_texture;
void sglActiveTextureARBs(GLenum texture)
{
	LOG(__func__);

	ActiveTexture_texture = texture;
	glActiveTextureARB(texture);
}

//CLIENT ACTIVE TEXTURE
static GLenum ClientActiveTexture_texture;
void sglClientActiveTextureARBs(GLenum texture)
{
	LOG(__func__);

	ClientActiveTexture_texture = texture;
	glClientActiveTextureARB(texture);
}

//BIND TEXTURE
static GLuint BindTexture_ids[8];
void sglBindTexture(GLenum target, GLuint texture)
{
	LOG(__func__);

	assert(target == GL_TEXTURE_2D);

	BindTexture_ids[ActiveTexture_texture] = texture;
	glBindTexture(target, texture);
}

//MATRIX MODE
static GLenum MatrixMode_mode;
void sglMatrixMode(GLenum mode)
{
	LOG(__func__);

	MatrixMode_mode = mode;
	glMatrixMode(mode);
}

//ENTER/EXIT
float ModelViewMatrix[16];
float ProjectionMatrix[16];

void sglEnter()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(ProjectionMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(ModelViewMatrix);

	glVertexPointer(VertexPointer_size, VertexPointer_type, VertexPointer_stride, VertexPointer_pointer);
	glColorPointer(ColorPointer_size, ColorPointer_type, ColorPointer_stride, ColorPointer_pointer);
	glSecondaryColorPointerEXT(SecondaryColorPointer_size, SecondaryColorPointer_type, SecondaryColorPointer_stride, SecondaryColorPointer_pointer);
	glTexCoordPointer(TexCoordPointer_size, TexCoordPointer_type, TexCoordPointer_stride, TexCoordPointer_pointer);

	glAlphaFunc(AlphaFunc_func, AlphaFunc_ref);
	glBlendFunc(BlendFunc_sfactor, BlendFunc_dfactor);
	glPolygonMode(GL_FRONT, PolygonMode_ModeList[0]);
	glPolygonMode(GL_BACK, PolygonMode_ModeList[1]);
	glViewport(Viewport_x, Viewport_y, Viewport_width, Viewport_height);
	glClearColor(ClearColor_red, ClearColor_green, ClearColor_blue, ClearColor_alpha);
	glCullFace(CullFace_mode);
	glDepthFunc(DepthFunc_func);
	glDepthMask(DepthMask_flag);
	glDepthRange(DepthRange_nearVal, DepthRange_farVal);
	glScissor(Scissor_x, Scissor_y, Scissor_width, Scissor_height);
	glMatrixMode(MatrixMode_mode);

	for(int i = 0; i != SGL_CAP_MAX; i ++)
	{
		if(CapState[i])
		{
			glEnable(CapTranslate[i]);
		}
		else
		{
			glDisable(CapTranslate[i]);
		}
	}

	for(int i = 0; i != SGL_CLIENT_CAP_MAX; i ++)
	{
		if(ClientCapState[i])
		{
			glEnableClientState(ClientCapTranslate[i]);
		}
		else
		{
			glDisableClientState(ClientCapTranslate[i]);
		}
	}

	glActiveTextureARB(ActiveTexture_texture);
	glClientActiveTextureARB(ClientActiveTexture_texture);
}

void sglExit()
{
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrix);
}

