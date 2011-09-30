ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET			:=	libvbam
TYPE			:=	lib
INCLUDE			:=	.. ../mednafen/src src/src

CPPFLAGS		+=	-DFINAL_VERSION -DSDL -DNO_PNG -DNO_DEBUGGER -DC_CORE -DMDFNPS3 -DNO_LINK
CPPFLAGS		+=	-DsystemDebug=systemDebugVBAM -DStereo_Buffer=Stereo_BufferVBAM -DBlip_Buffer=Blip_BufferVBAM -DBlip_Synth_Fast_=Blip_Synth_Fast_VBAM
CPPFLAGS		+=	-DBlip_Synth_=Blip_Synth_VBAM -DSilent_Blip_Buffer=Silent_Blip_BufferVBAM -Dblip_eq_t=blip_eq_tVBAM -Dbreakpoint=breakpointVBAM

CXXFILES		+=	src/mednafen.cpp									\
					src/system.cpp

CXXFILES		+=	src/src/Util.cpp

CFILES			+=	src/src/common/memgzio.c

CXXFILES		+=	src/src/apu/Blip_Buffer.cpp							\
					src/src/apu/Effects_Buffer.cpp						\
					src/src/apu/Gb_Apu.cpp								\
					src/src/apu/Gb_Apu_State.cpp						\
					src/src/apu/Gb_Oscs.cpp								\
					src/src/apu/Multi_Buffer.cpp

CXXFILES		+=	src/src/gb/gbCheats.cpp								\
					src/src/gb/GB.cpp									\
					src/src/gb/gbDis.cpp								\
					src/src/gb/gbGfx.cpp								\
					src/src/gb/gbGlobals.cpp							\
					src/src/gb/gbMemory.cpp								\
					src/src/gb/gbPrinter.cpp							\
					src/src/gb/gbSGB.cpp								\
					src/src/gb/gbSound.cpp

CXXFILES		+=	src/src/gba/agbprint.cpp							\
					src/src/gba/armdis.cpp								\
					src/src/gba/bios.cpp								\
					src/src/gba/Cheats.cpp								\
					src/src/gba/CheatSearch.cpp							\
					src/src/gba/EEprom.cpp								\
					src/src/gba/elf.cpp									\
					src/src/gba/Flash.cpp								\
					src/src/gba/GBA-arm.cpp								\
					src/src/gba/GBA.cpp									\
					src/src/gba/gbafilter.cpp							\
					src/src/gba/GBAGfx.cpp								\
					src/src/gba/GBALink.cpp								\
					src/src/gba/GBASockClient.cpp						\
					src/src/gba/GBA-thumb.cpp							\
					src/src/gba/Globals.cpp								\
					src/src/gba/Mode0.cpp								\
					src/src/gba/Mode1.cpp								\
					src/src/gba/Mode2.cpp								\
					src/src/gba/Mode3.cpp								\
					src/src/gba/Mode4.cpp								\
					src/src/gba/Mode5.cpp								\
					src/src/gba/RTC.cpp									\
					src/src/gba/Sound.cpp								\
					src/src/gba/Sram.cpp
#					src/src/gba/remote.cpp

include $(CURDIR)/../Makefile-$(PLATFORM).base

