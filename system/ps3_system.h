#include <stdint.h>
#include <string>

#include "src/ESVideo.h"
#include "src/ESAudio.h"
#include "src/ESInput.h"

#ifdef L1GHT
# include "l1ght/cell.h"
# include "l1ght/L1ghtVideo.h"
# include "l1ght/L1ghtAudio.h"
# include "l1ght/L1ghtInput.h"
# include "l1ght/utility.h"
#else
# include "sdl/cell.h"
# include "sdl/SDLVideo.h"
# include "sdl/SDLAudio.h"
# include "sdl/SDLInput.h"
# include "sdl/utility.h"
#endif

#include "src/fex/fex/fex.h"

#include "src/utility/Font.h"
#include "src/utility/ImageManager.h"
#include "src/utility/GUI/Menu.h"
#include "src/utility/GUI/ListItem.h"
#include "src/utility/GUI/GridItem.h"
#include "src/utility/GUI/Winterface.h"
#include "src/utility/GUI/WinterfaceList.h"
#include "src/utility/GUI/WinterfaceMultiList.h"
#include "src/utility/GUI/WinterfaceIconGrid.h"
#include "src/utility/GUI/ErrorDialog.h"
#include "src/utility/Files/FileException.h"
#include "src/utility/Files/FileListItem.h"
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
extern PathBuild*		es_paths;
extern Logger*			es_log;

void				InitES					(void (*aExitFunction)() = 0);
void				QuitES					();
volatile bool		WantToDie				();
volatile bool		WantToSleep				();
void				Abort					(const char* aMessage);



