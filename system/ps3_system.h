#ifndef PS3_SYSTEM_H____
#define PS3_SYSTEM_H____

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>

#include "src/ESException.h"
inline void				ErrorCheck					(bool aCondition, const char* aMessage, ...)
{
	if(!aCondition)
	{
		char buffer[2048];

		va_list args;
		va_start (args, aMessage);
		vsnprintf(buffer, 2048, aMessage, args);
		va_end (args);

		throw ESException(buffer);		
	}
}

#include "src/ESVideo.h"
#include "src/ESAudio.h"
#include "src/ESInput.h"
#include "src/ESNetwork.h"

#ifdef L1GHT
# include "l1ght/cell.h"
# include "l1ght/L1ghtVideo.h"
# include "l1ght/L1ghtAudio.h"
# include "l1ght/L1ghtInput.h"
# include "l1ght/L1ghtNetwork.h"
# include "l1ght/utility.h"
#elif defined(MDWII)
# include "wii/cell.h"
# include "wii/WiiVideo.h"
# include "wii/WiiAudio.h"
# include "wii/WiiInput.h"
# include "wii/WiiNetwork.h"
# include "wii/utility.h"
#else
# include "sdl/cell.h"
# include "sdl/SDLVideo.h"
# include "sdl/SDLAudio.h"
# include "sdl/SDLInput.h"
# include "sdl/SDLNetwork.h"
# include "sdl/utility.h"
#endif

#include "src/fex/fex/fex.h"

#include "src/utility/Font.h"
#include "src/utility/ImageManager.h"
#include "src/utility/GUI/Menu.h"
#include "src/utility/GUI/ListItem.h"
#include "src/utility/GUI/Summerface.h"
#include "src/utility/Files/FileEnumerator.h"
#include "src/utility/Files/FileList.h"
#include "src/utility/Files/FileSelect.h"
#include "src/utility/Files/ArchiveList.h"
#include "src/utility/Keyboard.h"
#include "src/utility/TextViewer.h"
#include "src/utility/Logger.h"

extern ESVideo*			es_video;
extern ESAudio*			es_audio;
extern ESInput*			es_input;
extern ESNetwork*		es_network;
extern PathBuild*		es_paths;
extern Logger*			es_log;

void				InitES					(void (*aExitFunction)() = 0);
void				QuitES					();
volatile bool		WantToDie				();
volatile bool		WantToSleep				();
void				Abort					(const char* aMessage);
void				ESSUB_Error				(const char* aMessage);
std::string			ESSUB_GetString			(const std::string& aHeader, const std::string& aMessage);

#endif
