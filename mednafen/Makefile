.SUFFIXES:

ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

##Package Details
SFOXML		=	package/sfo.xml
ICON0		=	package/ICON0.png
TITLE		:=	Mednafen
APPID		:=	MDFN90002
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000

##Mednafen config
MEDNAFENFLAGS=-DWANT_NES_EMU -DWANT_SMS_EMU -DWANT_GB_EMU -DWANT_GBA_EMU -DWANT_MD_EMU -DWANT_WSWAN_EMU -DWANT_NGP_EMU -DWANT_PCE_EMU -DWANT_PCE_FAST_EMU -DWANT_VB_EMU -DWANT_LYNX_EMU -DNEED_Z80_EMULATOR -DNETWORK
MEDNAFENCONFIG=-DPSS_STYLE=1 -DHAVE_MKDIR -DMEDNAFEN_VERSION=\"0.9.15\" -DMEDNAFEN_VERSION_NUMERIC=0x915 -DSIZEOF_DOUBLE=8
MEDNAFENINCLUDES=. src/src/hw_misc src/src/hw_video src/include src/src/hw_cpu src/src/sms src/src/hw_sound src/include/blip src/src/ngp src/src/ngp/TLCS-900h src/src/snes/src/lib

##Build info
TARGET		:=	mednafen
TYPE		:=	elf
PACKAGE		:=	mednafen.pkg
BUILD		:=	build
SOURCE		:=	src src/src src/filters src/src/cdrom src/src/compress src/src/gb src/src/gba src/src/hw_cpu/c68k src/src/hw_cpu/huc6280 src/src/hw_cpu/v810 \
				src/src/hw_cpu/v810/fpu-new src/src/hw_cpu/z80-fuse src/src/hw_misc/arcade_card src/src/hw_sound/gb_apu src/src/hw_sound/pce_psg \
				src/src/hw_sound/sms_apu src/src/hw_sound/ym2413 src/src/hw_sound/ym2612 src/src/hw_video/huc6270 src/src/lynx src/src/md \
				src/src/md/cart src/src/md/input src/src/mpcdec src/src/nes src/src/nes/boards src/src/nes/input src/src/nes/ntsc src/src/nes/ppu \
				src/src/nes/ppu/palettes src/src/ngp src/src/ngp/TLCS-900h src/src/pce src/src/pce/input src/src/pce_fast \
				src/src/sms src/src/sound src/src/string src/src/tremor src/src/trio src/src/vb src/src/video src/src/wswan src/src/md/cd 

#SDL Port gets SNES, lucky
#ifeq ($(strip $(PLATFORM)),sdl)
#MEDNAFENFLAGS+=-DWANT_SNES_EMU 

#CXXFILES	:=	src/src/snes/interface.cpp src/src/snes/src/cartridge/cartridge.cpp src/src/snes/src/cheat/cheat.cpp \
#				src/src/snes/src/memory/memory.cpp src/src/snes/src/memory/smemory/smemory.cpp src/src/snes/src/cpu/cpu.cpp src/src/snes/src/cpu/core/core.cpp src/src/snes/src/cpu/scpu/scpu.cpp \
#				src/src/snes/src/smp/smp.cpp src/src/snes/src/smp/core/core.cpp src/src/snes/src/smp/ssmp/ssmp.cpp src/src/snes/src/dsp/sdsp/sdsp.cpp src/src/snes/src/ppu/ppu.cpp \
#				src/src/snes/src/ppu/bppu/bppu.cpp src/src/snes/src/system/system.cpp src/src/snes/src/chip/sa1/sa1.cpp src/src/snes/src/chip/bsx/bsx.cpp src/src/snes/src/chip/srtc/srtc.cpp \
#				src/src/snes/src/chip/sdd1/sdd1.cpp src/src/snes/src/chip/spc7110/spc7110.cpp src/src/snes/src/chip/cx4/cx4.cpp src/src/snes/src/chip/dsp1/dsp1.cpp src/src/snes/src/chip/dsp2/dsp2.cpp \
#				src/src/snes/src/chip/dsp3/dsp3.cpp src/src/snes/src/chip/dsp4/dsp4.cpp src/src/snes/src/chip/obc1/obc1.cpp src/src/snes/src/chip/st010/st010.cpp src/src/snes/src/chip/st011/st011.cpp \
#				src/src/snes/src/chip/st018/st018.cpp src/src/snes/src/chip/superfx/superfx.cpp src/src/snes/src/chip/21fx/21fx.cpp
				
#CFILES		:=	src/src/snes/src/lib/libco/libco.c
#endif

INCLUDE		:=	src include ../system $(MEDNAFENINCLUDES) 

LIBDIRS		:=	../system ../nestopia ../gambatte ../vbam

ifeq ($(strip $(PLATFORM)),l1ght)
	LIBS		:= -lemusys -lnestopia -lgambatte -lvbam -laudio -lgcm_sys -lreality -lnet -lsysutil -lio -lz -lm -lsysmodule 
else
	LIBS		:= -lemusys -lnestopia -lgambatte -lz -lm
endif

#Not yet
#LIBDIRS		+= ../pcsxr
#LIBS		+= -lpcsxr

CFLAGS		+= -O2 --std=gnu99 $(MEDNAFENFLAGS) $(MEDNAFENCONFIG)
CXXFLAGS	+= -O2 $(MEDNAFENCFLAGS) $(MEDNAFENFLAGS) $(MEDNAFENCONFIG)

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif


