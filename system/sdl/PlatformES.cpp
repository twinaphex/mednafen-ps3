#include <es_system.h>
#include <SDL.h>

namespace
{
	bool			want_to_die = false;
};

void				SetExit							()
{
	want_to_die = true;
}

void				LibESPlatform::Initialize		()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER);
}

void				LibESPlatform::Shutdown			()
{
	SDL_Quit();
}

volatile bool		LibESPlatform::WantToDie		()
{
	return want_to_die;
}

volatile bool		LibESPlatform::WantToSleep		()
{
	return false;
}

std::string			LibESPlatform::BuildPath		(const std::string& aPath)
{
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#ifndef ES_HOME_PATH
#define ES_HOME_PATH mednafen
#endif
#define ES_DIR TOSTRING(ES_HOME_PATH)

#ifndef __WIN32__
	return std::string(getenv("HOME")) + "/." ES_DIR "/" + aPath;
#else
	char path[MAX_PATH];
	SHGetFolderPathA(0, CSIDL_APPDATA, 0, 0, path);
	return std::string(path) + "/" ES_DIR "/" + aPath;
#endif
}

