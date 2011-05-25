#pragma once

#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

#include <malloc.h>

#include <fat.h>
#include <network.h>
#include <wiiuse/wpad.h>
#include <ogc/mutex.h>
#include <ogc/audio.h>
#include <ogc/lwp_watchdog.h>

class	PlatformHelpers
{
		static uint32_t					GetTicks					()
		{
			//Stolen from SDL wii
			const uint64_t ticks      = gettime();
			const uint64_t ms         = ticks / TB_TIMER_CLOCK;
			return ms;
		}

		static void						Sleep						(uint32_t aMilliseconds)
		{
			usleep(aMilliseconds * 1000);
		}
};

