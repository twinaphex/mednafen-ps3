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
#include <malloc.h>
#include <unistd.h>
#include <assert.h>
#include <exception>
#include <png.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/net.h>

#include <psl1ght/lv2.h>
#include <psl1ght/lv2/filesystem.h>
#include <sysutil/video.h>
#include <sysutil/events.h>
#include <io/pad.h>
#include <io/msg.h>
#include <audio/audio.h>
#include <sys/thread.h>
#include <lv2/process.h>
#include <sysmodule/sysmodule.h>
#include <pngdec/loadpng.h>

extern "C"
{
	#include <rsx/gcm.h>
	#include <rsx/commands.h>
	#include <rsx/nv40.h>
	#include <rsx/reality.h>
	#include <psl1ght/lv2/thread.h>
	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_BITMAP_H
	#include FT_GLYPH_H
}

#define	HAVE_ESSUB_ERROR

#endif

