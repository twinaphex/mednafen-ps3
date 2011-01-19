#include <src/mednafen.h>
#include <src/git.h>
#include <src/driver.h>
#include <src/general.h>

namespace vbamMDFN{

#include "Util.h"
#include "common/Port.h"
#include "common/Patch.h"
#include "gba/Flash.h"
#include "gba/RTC.h"
#include "gba/Sound.h"
#include "gba/Cheats.h"
#include "gba/GBA.h"
#include "gba/agbprint.h"
#include "gb/gb.h"
#include "gb/gbGlobals.h"
#include "gb/gbCheats.h"
#include "gb/gbSound.h"
#include "common/SoundDriver.h"

static uint32_t		start_time = 0;
void				InitSystem								()
{
	start_time = MDFND_GetTime();

	utilUpdateSystemColorMaps();
}

namespace mdfn
{
	extern EmulateSpecStruct*		ESpec;
	extern uint8_t*					Ports[4];
	extern uint32_t					SoundFrame;
	extern uint32_t					FrameCount;
	extern uint32_t					SkipHack;
}
using namespace mdfn;

class soundy : public SoundDriver
{
	public:
								~soundy			()								{};
	
		bool					init			(long sampleRate)				{};
		void					pause			()								{};
		void					reset			()								{};
		void					resume			()								{};
		void					setThrottle		(unsigned short throttle)		{}
	
		void					write			(u16* finalWave, int length)
		{
			if(ESpec->SoundBufMaxSize > 0 && ESpec->SoundRate > 0)
			{
				memcpy(&ESpec->SoundBuf[SoundFrame * 2], finalWave, length);
				SoundFrame += length / 4;
			}	
		}
};


void 				sdlApplyPerImagePreferences				()
{
	FILE *f = fopen(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("vbam.vbaover").c_str()).c_str(), "r");
	if(!f)
	{
		return;
	}

	char buffer[7];
	buffer[0] = '[';
	buffer[1] = rom[0xac];
	buffer[2] = rom[0xad];
	buffer[3] = rom[0xae];
	buffer[4] = rom[0xaf];
	buffer[5] = ']';
	buffer[6] = 0;
	
	char readBuffer[2048];
	
	bool found = false;
	
	while(1)
	{
		char *s = fgets(readBuffer, 2048, f);
		
		if(s == NULL)
			break;

		char *p  = strchr(s, ';');

		if(p)
			*p = 0;

		char *token = strtok(s, " \t\n\r=");

		if(!token)
			continue;
		if(strlen(token) == 0)
			continue;

		if(!strcmp(token, buffer))
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		while(1)
		{
			char *s = fgets(readBuffer, 2048, f);
			
			if(s == NULL)
				break;

			char *p = strchr(s, ';');
			if(p)
				*p = 0;

			char *token = strtok(s, " \t\n\r=");
			if(!token)
				continue;
			if(strlen(token) == 0)
				continue;

			if(token[0] == '[') // starting another image settings
				break;
			char *value = strtok(NULL, "\t\n\r=");
			if(value == NULL)
				continue;

			if(!strcmp(token, "rtcEnabled"))
				rtcEnable(atoi(value) == 0 ? false : true);
			else if(!strcmp(token, "flashSize")) {
				int size = atoi(value);
				if(size == 0x10000 || size == 0x20000)
		        	flashSetSize(size);
				} else if(!strcmp(token, "saveType")) {
					int save = atoi(value);
					if(save >= 0 && save <= 5)
						cpuSaveType = save;
			} else if(!strcmp(token, "mirroringEnabled")) {
				mirroringEnable = (atoi(value) == 0 ? false : true);
			}
		}
	}
	fclose(f);
}


struct EmulatedSystem emulator = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, false, 0}; 


u16					systemColorMap16[0x10000];
u32					systemColorMap32[0x10000];
u16					systemGbPalette[24];
int					systemRedShift = 19;
int					systemGreenShift = 11;
int					systemBlueShift = 3;
int					systemColorDepth = 32;
int					systemDebug = 0;
int					systemVerbose = 0;
int					systemFrameSkip = 0;
int					systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
u32					RomIdCode = 0;
int					emulating = 0;

void				debugOutputStub							(const char*, u32)						{}
void				(*dbgOutput)							(const char *s, u32 addr) = debugOutputStub;



bool				systemPauseOnFrame						()										{return true;}
void				systemGbPrint							(u8 *,int,int,int,int)					{}
void				systemScreenCapture						(int)									{}
void				systemShowSpeed							(int)									{}
void				systemGbBorderOn						()										{}
void				systemFrame								()										{}
void				system10Frames							(int)									{}
void				systemMessage							(int, const char *, ...)				{}


//Timing
u32					systemGetClock							()										{return MDFND_GetTime() - start_time;}

//Input
bool				systemReadJoypads						()										{return true;}
u32					systemReadJoypad						(int)									{return Ports[0][0] | (Ports[0][1] << 8);}
void				systemCartridgeRumble					(bool)									{}
void				systemPossibleCartridgeRumble			(bool)									{}
void				updateRumbleFrame						()										{}
void				systemUpdateMotionSensor				()										{}
int					systemGetSensorX						()										{return 0;}
int					systemGetSensorY						()										{return 0;}
int					systemGetSensorZ						()										{return 0;}
u8					systemGetSensorDarkness					()										{return 0;}

//Visual
void				systemDrawScreen						()
{
	uint32_t* screenimage = (uint32_t*)pix;
	uint32_t* destimage = (uint32_t*)ESpec->surface->pixels;

	for(int i = 0; i != 160; i ++)
	{
		for(int j = 0; j != 240; j ++)
		{
			destimage[i * ESpec->surface->pitch32 + j] = screenimage[i * 241 + j];
		}
	}
}

//Sound
SoundDriver*		systemSoundInit							()
{
	soundShutdown();
	return new soundy();
}

void				systemOnWriteDataToSoundBuffer			(const u16 * finalWave, int length)		{}
void				systemOnSoundShutdown					()										{}
bool				systemCanChangeSoundQuality				()										{return true;}

}
