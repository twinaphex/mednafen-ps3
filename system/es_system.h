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
#include "src/ESInput.h"
#include "src/ESAudio.h"
#include "src/main.h"

#include "Platform.h"
#include "PlatformThreads.h"
#include "opengl_common/Texture.h"
#include "src/ESVideo.h"
#include "PlatformVideo.h"
#include "PlatformNetwork.h"

#include "src/Helpers.h"
#include "src/utility/Font.h"
#include "src/utility/ImageManager.h"
#include "src/GUI/Menu.h"
#include "src/GUI/SummerfaceTypes.h"
#include "src/GUI/Summerface.h"
#include "src/GUI/SummerfaceWindow.h"
#include "src/GUI/SummerfaceList.h"
#include "src/GUI/SummerfaceItem.h"
#include "src/GUI/SummerfaceInputConduit.h"
#include "src/GUI/SimpleWidgets.h"

#endif
