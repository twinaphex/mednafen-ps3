#include "src/cell.h"

#include "src/fex/fex/fex.h"


#include "src/PS3Video.h"
#include "src/PS3Input.h"
#include "src/PS3Audio.h"

#include "src/utility/Font.h"
#include "src/utility/Menu.h"
#include "src/utility/ListItem.h"
#include "src/utility/Winterface.h"
#include "src/utility/WinterfaceList.h"
#include "src/utility/WinterfaceMultiList.h"
#include "src/utility/Keyboard.h"
#include "src/utility/FileSelect.h"
#include "src/utility/Archive.h"
#include "src/utility/TextViewer.h"
#include "src/utility.h"


void				InitPS3					();
void				QuitPS3					();
volatile bool		WantToDie				();
volatile bool		WantToSleep				();
void				Restart					(const char* aSelfFile);
void				TestError				(bool aCondition, const char* aMessage);

extern Logger* ps3_log;

