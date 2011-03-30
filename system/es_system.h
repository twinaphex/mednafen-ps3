#ifndef PS3_SYSTEM_H____
#define PS3_SYSTEM_H____

#include "src/stdheaders.h"


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

#include "src/ESThreads.h"
#include "src/ESTexture.h"
#include "src/ESVideo.h"
#include "src/ESAudio.h"
#include "src/ESInput.h"
#include "src/ESNetwork.h"
#include "src/main.h"

#ifdef L1GHT
# include "l1ght/L1ghtPlatform.h"
# include "l1ght/L1ghtTexture.h"
# include "l1ght/L1ghtVideo.h"
# include "l1ght/L1ghtAudio.h"
# include "l1ght/L1ghtInput.h"
# include "l1ght/L1ghtNetwork.h"
# include "l1ght/utility.h"
#elif defined(MDDORK)
# include "dork/DorkPlatform.h"
# include "dork/DorkThreads.h"
# include "dork/DorkTexture.h"
# include "dork/DorkVideo.h"
# include "dork/DorkAudio.h"
# include "dork/DorkInput.h"
# include "dork/DorkNetwork.h"
# include "dork/utility.h"
#elif defined(MDWII)
# include "wii/WiiPlatform.h"
# include "wii/WiiThreads.h"
# include "wii/WiiTexture.h"
# include "wii/WiiVideo.h"
# include "wii/WiiAudio.h"
# include "wii/WiiInput.h"
# include "wii/WiiNetwork.h"
# include "wii/utility.h"
#else
# include "sdl/SDLPlatform.h"
# include "sdl/SDLThreads.h"
# include "sdl/SDLTexture.h"
# include "sdl/SDLVideo.h"
# include "sdl/SDLAudio.h"
# include "sdl/SDLInput.h"
# include "sdl/SDLNetwork.h"
# include "sdl/utility.h"
#endif

#include "src/fex/fex/fex.h"

#include "src/utility/FastCounter.h"
#include "src/utility/Font.h"
#include "src/utility/ImageManager.h"
#include "src/utility/GUI/Menu.h"
#include "src/utility/GUI/Summerface.h"
#include "src/utility/Files/FileSelect.h"
#include "src/utility/Files/ArchiveList.h"
#include "src/utility/Keyboard.h"
#include "src/utility/TextViewer.h"
#include "src/utility/Logger.h"

#endif
