#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void sglEnter();
void sglExit();

enum
{
	SGL_ALPHA_TEST, SGL_AUTO_NORMAL, SGL_BLEND, SGL_COLOR_LOGIC_OP, SGL_COLOR_MATERIAL, SGL_COLOR_SUM,
	SGL_COLOR_TABLE, SGL_CONVOLUTION_1D, SGL_CONVOLUTION_2D, SGL_CULL_FACE, SGL_DEPTH_TEST, SGL_DITHER,
	SGL_FOG, SGL_HISTOGRAM, SGL_INDEX_LOGIC_OP, SGL_LIGHT0, SGL_LIGHT1, SGL_LIGHT2, SGL_LIGHT3, SGL_LIGHTING,
	SGL_LINE_SMOOTH, SGL_LINE_STIPPLE, SGL_MAP1_COLOR_4, SGL_MAP1_INDEX, SGL_MAP1_NORMAL, SGL_MAP1_TEXTURE_COORD_1,
	SGL_MAP1_TEXTURE_COORD_2, SGL_MAP1_TEXTURE_COORD_3, SGL_MAP1_TEXTURE_COORD_4, SGL_MAP1_VERTEX_3,
	SGL_MAP1_VERTEX_4, SGL_MAP2_COLOR_4, SGL_MAP2_INDEX, SGL_MAP2_NORMAL, SGL_MAP2_TEXTURE_COORD_1,
	SGL_MAP2_TEXTURE_COORD_2, SGL_MAP2_TEXTURE_COORD_3, SGL_MAP2_TEXTURE_COORD_4, SGL_MAP2_VERTEX_3,
	SGL_MAP2_VERTEX_4, SGL_MINMAX, SGL_MULTISAMPLE, SGL_NORMALIZE, SGL_POINT_SMOOTH, SGL_POINT_SPRITE,
	SGL_POLYGON_OFFSET_FILL, SGL_POLYGON_OFFSET_LINE, SGL_POLYGON_OFFSET_POINT, SGL_POLYGON_SMOOTH,
	SGL_POLYGON_STIPPLE, SGL_POST_COLOR_MATRIX_COLOR_TABLE, SGL_POST_CONVOLUTION_COLOR_TABLE,
	SGL_RESCALE_NORMAL, SGL_SAMPLE_ALPHA_TO_COVERAGE, SGL_SAMPLE_ALPHA_TO_ONE, SGL_SAMPLE_COVERAGE,
	SGL_SEPARABLE_2D, SGL_SCISSOR_TEST, SGL_STENCIL_TEST, SGL_TEXTURE_1D, SGL_TEXTURE_2D, SGL_TEXTURE_3D,
	SGL_TEXTURE_CUBE_MAP, SGL_TEXTURE_GEN_Q, SGL_TEXTURE_GEN_R, SGL_TEXTURE_GEN_S, SGL_TEXTURE_GEN_T,
	SGL_VERTEX_PROGRAM_POINT_SIZE, SGL_VERTEX_PROGRAM_TWO_SIDE, SGL_CAP_MAX
};

enum
{
	SGL_COLOR_ARRAY, SGL_EDGE_FLAG_ARRAY, SGL_FOG_COORD_ARRAY, SGL_INDEX_ARRAY, SGL_NORMAL_ARRAY, SGL_SECONDARY_COLOR_ARRAY,
	SGL_TEXTURE_COORD_ARRAY, SGL_VERTEX_ARRAY, SGL_SECONDARY_COLOR_ARRAY_EXT, SGL_CLIENT_CAP_MAX
};

void sglEnable(GLenum cap);
void sglDisable(GLenum cap);
GLboolean sglIsEnabled(GLenum cap);

void sglEnableClientState(GLenum cap);
void sglDisableClientState(GLenum cap);
void sglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void sglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void sglSecondaryColorPointerEXTs(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void sglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);

void sglAlphaFunc(GLenum func, GLclampf ref);
void sglBlendFunc(GLenum sfactor, GLenum dfactor);
void sglPolygonMode(GLenum face, GLenum mode);
void sglViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void sglClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void sglCullFace(GLenum mode);
void sglDepthFunc(GLenum func);
void sglDepthMask(GLboolean flag);
void sglDepthRange(GLclampd nearVal, GLclampd farVal);
void sglScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void sglPolygonOffset(GLfloat factor, GLfloat units);

void sglActiveTextureARBs(GLenum texture);
void sglClientActiveTextureARBs(GLenum texture);
void sglBindTexture(GLenum target, GLuint texture);

void sglMatrixMode(GLenum mode);

#ifndef NO_TRANSLATE
#define glEnable(T) sglEnable(S##T)
#define glDisable(T) sglDisable(S##T)
#define glIsEnabled(T) sglIsEnabled(S##T)

#define glEnableClientState(T) sglEnableClientState(S##T)
#define glDisableClientState(T) sglDisableClientState(S##T)
#define glVertexPointer sglVertexPointer
#define glColorPointer sglColorPointer
#define glSecondaryColorPointerEXT sglSecondaryColorPointerEXTs
#define glTexCoordPointer sglTexCoordPointer

#define glAlphaFunc sglAlphaFunc
#define glBlendFunc sglBlendFunc
#define glPolygonMode sglPolygonMode
#define glViewport sglViewport
#define glClearColor sglClearColor
#define glCullFace sglCullFace
#define glDepthFunc sglDepthFunc
#define glDepthMask sglDepthMask
#define glDepthRange sglDepthRange
#define glScissor sglScissor
#define glPolygonOffset sglPolygonOffset

#define glActiveTextureARB sglActiveTextureARBs
#define glClientActiveTextureARB sglClientActiveTextureARBs
#define glBindTexture sglBindTexture

#define glMatrixMode sglMatrixMode
#endif

#ifdef __cplusplus
}
#endif

