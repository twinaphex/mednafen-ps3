#include "src/cell.h"

#include "src/fex/fex/fex.h"

#include "src/ESVideo.h"
#include "src/ESInput.h"
#include "src/ESAudio.h"

#include "src/utility.h"
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



void				InitES					(void (*aExitFunction)() = 0);
void				QuitES					();
volatile bool		WantToDie				();
volatile bool		WantToSleep				();
void				Abort					(const char* aMessage);

extern				PathBuild				Paths;

extern Logger* es_log;


