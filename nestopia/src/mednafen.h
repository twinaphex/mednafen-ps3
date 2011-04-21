#ifndef NEST_MEDNAFEN_H
#define NEST_MEDNAFEN_H

#include "core/api/NstApiEmulator.hpp"
#include "core/api/NstApiVideo.hpp"
#include "core/api/NstApiSound.hpp"

using namespace Nes;
using namespace Nes::Api;

#include "settings.h"

int				NestLoad				(const char *name, MDFNFILE *fp);
bool			NestTestMagic			(const char *name, MDFNFILE *fp);
void			NestCloseGame			(void);
bool			NestToggleLayer			(int which);
void			NestInstallReadPatch	(uint32 address);
void			NestRemoveReadPatches	(void);
uint8			NestMemRead				(uint32 addr);
int				NestStateAction			(StateMem *sm, int load, int data_only);
void			NestEmulate				(EmulateSpecStruct *espec);
void			NestSetInput			(int port, const char *type, void *ptr);
void			NestDoSimpleCommand		(int cmd);

namespace nestMDFN
{
	extern EmulateSpecStruct*			ESpec;

	extern bool							GameOpen;

	extern Nes::Api::Emulator			Nestopia;
	extern Video::Output 				EmuVideo;
	extern Sound::Output 				EmuSound;

	extern uint32_t						Samples[48000];
}


#endif
