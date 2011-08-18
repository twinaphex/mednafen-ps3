ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

##Package Details
SFOXML				:=	package/sfo.xml
ICON0				:=	package/ICON0.png
TITLE				:=	Mednafen
APPID				:=	MDFN90002
CONTENTID			:=	UP0001-$(APPID)_00-0000000000000000
PACKAGE				:=	mednafen.pkg

##Source files for modules
#Note that some modules in the hw_* category may be used by more than one modules, fix issues as they arise
NESSRC				:=	src/src/nes src/src/nes/boards src/src/nes/input src/src/nes/ntsc src/src/nes/ppu src/src/nes/ppu/palettes
NESCPP				:=	-DWANT_NES_EMU

SMSSRC				:=	src/src/sms src/src/hw_sound/sms_apu src/src/hw_sound/ym2413 src/src/hw_cpu/z80-fuse
SMSCPP				:=	-DWANT_SMS_EMU -DNEED_Z80_EMU

MDSRC				:=	src/src/md src/src/md/cart src/src/md/input src/src/md/cd src/src/hw_cpu/c68k src/src/hw_sound/ym2612
MDCPP				:=	-DWANT_MD_EMU

WSWANSRC			:=	src/src/wswan
WSWANCPP			:=	-DWANT_WSWAN_EMU

NGPSRC				:=	src/src/ngp src/src/ngp/TLCS-900h
NGPCPP				:=	-DWANT_NGP_EMU

PCESRC				:=	src/src/hw_sound/pce_psg src/src/pce src/src/pce/input src/src/hw_misc/arcade_card src/src/hw_cpu/huc6280 src/src/hw_video/huc6270
PCECPP				:=	-DWANT_PCE_EMU

PCEFASTSRC			:=	src/src/pce_fast
PCEFASTCPP			:=	-DWANT_PCEFAST_EMU

VBSRC				:=	src/src/vb src/src/hw_cpu/v810 src/src/hw_cpu/v810/fpu-new
VBCPP				:=	-DWANT_VB_EMU

LYNXSRC				:=	src/src/lynx
LYNXCPP				:=	-DWANT_LYNX_EMU

PCFXSRC				:=	src/src/pcfx src/src/pcfx/input
PCFXCPP				:=	-DWANT_PCEFX_EMU

GBSRC				:=	src/src/gb src/src/hw_sound/gb_apu
GBCPP				:=	-DWANT_GB_EMU

GBASRC				:=	src/src/gba
GBACPP				:=	-DWANT_GBA_EMU

MAINSRC				:=	src/src src/filters src/src/cdrom src/src/compress src/src/mpcdec src/src/sound src/src/string src/src/tremor src/src/trio src/src/video
MAINCPP				:=	-DNETWORK -DPSS_STYLE=1 -DHAVE_MKDIR -DMEDNAFEN_VERSION=\"0.9.16\" -DMEDNAFEN_VERSION_NUMERIC=0x916 -DSIZEOF_DOUBLE=8

FRONTENDSRC			:=	src

##Mednafen config
CPPFLAGS			+=	$(MAINCPP) $(SMSCPP) $(MDCPP) $(WSWANCPP) $(NGPCPP) $(PCECPP) $(PCEFASTCPP) $(VBCPP) $(LYNXCPP) $(PCFXCPP)
SOURCE				+=	$(FRONTENDSRC) $(MAINSRC) $(SMSSRC) $(MDSRC) $(WSWANSRC) $(NGPSRC) $(PCESRC) $(PCEFASTSRC) $(VBSRC) $(LYNXSRC) $(PCFXSRC)
INCLUDE				:=	. src ../system ../system/$(PLATFORM) src/src/hw_misc src/src/hw_video src/include src/src/hw_cpu src/src/sms src/src/hw_sound src/include/blip src/src/ngp src/src/ngp/TLCS-900h src/src/snes/src/lib
LIBDIRS				:=	../system ../nestopia ../gambatte ../vbam ../pcsxr ../stella
LIBS				:=	-lemusys -lnestopia -lgambatte -lpcsxr -lvbam -lstella

#LIBDIRS			+=	../lsnes ../yabause
#LIBS				+=	-llsnes -lsnes -lyabause

##Build info
TARGET				:=	mednafen
TYPE				:=	bin

#Extra files for SNES
#CXXFILES			:=	src/src/snes/interface.cpp src/src/snes/src/cartridge/cartridge.cpp src/src/snes/src/cheat/cheat.cpp \
#						src/src/snes/src/memory/memory.cpp src/src/snes/src/memory/smemory/smemory.cpp src/src/snes/src/cpu/cpu.cpp src/src/snes/src/cpu/core/core.cpp src/src/snes/src/cpu/scpu/scpu.cpp \
#						src/src/snes/src/smp/smp.cpp src/src/snes/src/smp/core/core.cpp src/src/snes/src/smp/ssmp/ssmp.cpp src/src/snes/src/dsp/sdsp/sdsp.cpp src/src/snes/src/ppu/ppu.cpp \
#						src/src/snes/src/ppu/bppu/bppu.cpp src/src/snes/src/system/system.cpp src/src/snes/src/chip/sa1/sa1.cpp src/src/snes/src/chip/bsx/bsx.cpp src/src/snes/src/chip/srtc/srtc.cpp \
#						src/src/snes/src/chip/sdd1/sdd1.cpp src/src/snes/src/chip/spc7110/spc7110.cpp src/src/snes/src/chip/cx4/cx4.cpp src/src/snes/src/chip/dsp1/dsp1.cpp src/src/snes/src/chip/dsp2/dsp2.cpp \
#						src/src/snes/src/chip/dsp3/dsp3.cpp src/src/snes/src/chip/dsp4/dsp4.cpp src/src/snes/src/chip/obc1/obc1.cpp src/src/snes/src/chip/st010/st010.cpp src/src/snes/src/chip/st011/st011.cpp \
#						src/src/snes/src/chip/st018/st018.cpp src/src/snes/src/chip/superfx/superfx.cpp src/src/snes/src/chip/21fx/21fx.cpp
#CFILES				:=	src/src/snes/src/lib/libco/libco.c


include $(CURDIR)/../Makefile-$(PLATFORM).base

