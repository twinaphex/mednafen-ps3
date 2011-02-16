//Collection of PS3 Headers

#ifndef CELL_H
#define	CELL_H

#include <map>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <exception>
#include <dirent.h>
#include <sys/stat.h>
#include <png.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_net.h>
#include "SDLInputConfig.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#endif


extern "C"
{
	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_BITMAP_H
	#include FT_GLYPH_H
}

#endif


