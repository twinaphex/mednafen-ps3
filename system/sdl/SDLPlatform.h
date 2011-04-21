#pragma once

#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_net.h>
#include "SDLInputConfig.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#endif

