#pragma once

#include <malloc.h>
#include <unistd.h>
#include <assert.h>

#include <sys/socket.h>
#include <sys/stat.h>
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
}

#define	HAVE_ESSUB_ERROR

#endif

