#include <sys/sys_time.h>
#include <sys/timer.h>

#include "Platform.h"

uint32_t					PlatformHelpers::GetTicks					()
{
	return sys_time_get_system_time() / 1000;
}

void						PlatformHelpers::Sleep						(uint32_t aMilliseconds)
{
	sys_timer_usleep(aMilliseconds * 1000);
}

