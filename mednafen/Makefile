.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "PSL1GHT must be set in the environment.")
endif

include $(CURDIR)/../Makefile.base

SFOXML		=	package/sfo.xml
ICON0		=	package/ICON0.png

MEDNAFENFLAGS=-DWANT_NES_EMU -DWANT_SMS_EMU -DWANT_GB_EMU -DWANT_GBA_EMU -DWANT_MD_EMU -DWANT_WSWAN_EMU -DWANT_NGP_EMU -DWANT_PCE_EMU -DWANT_PCE_FAST_EMU -DWANT_VB_EMU -DWANT_LYNX_EMU -DNEED_Z80_EMULATOR -DNETWORK
MEDNAFENCONFIG=-DPSS_STYLE=1 -DMSB_FIRST -DWORDS_BIGENDIAN -DHAVE_MKDIR -DMEDNAFEN_VERSION=\"0.9.15\" -DMEDNAFEN_VERSION_NUMERIC=0x915 -DSIZEOF_DOUBLE=8
MEDNAFENINCLUDES=. src/src/hw_misc src/src/hw_video src/include src/src/hw_cpu src/src/sms src/src/hw_sound src/include/blip src/src/ngp src/src/ngp/TLCS-900h src/src/snes/src/lib

TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCE		:=	src src/src src/filters src/src/cdrom src/src/compress src/src/gb src/src/gba src/src/hw_cpu/c68k src/src/hw_cpu/huc6280 src/src/hw_cpu/v810 \
				src/src/hw_cpu/v810/fpu-new src/src/hw_cpu/z80-fuse src/src/hw_misc/arcade_card src/src/hw_sound/gb_apu src/src/hw_sound/pce_psg \
				src/src/hw_sound/sms_apu src/src/hw_sound/ym2413 src/src/hw_sound/ym2612 src/src/hw_video/huc6270 src/src/lynx src/src/md \
				src/src/md/cart src/src/md/input src/src/mpcdec src/src/nes src/src/nes/boards src/src/nes/input src/src/nes/ntsc src/src/nes/ppu \
				src/src/nes/ppu/palettes src/src/ngp src/src/ngp/TLCS-900h src/src/pce src/src/pce/input src/src/pce_fast \
				src/src/sms src/src/sound src/src/string src/src/tremor src/src/trio src/src/vb src/src/video src/src/wswan src/src/md/cd 

INCLUDE		:=	src include ../system-l1ght ../system-l1ght/src/fex  $(MEDNAFENINCLUDES) 
DATA		:=	data
LIBS		:=	-lemusys -lnestopia -lgambatte -lvbam -laudio -lgcm_sys -lreality -lsysutil -lio -lz -lm -lfreetype -lsysmodule -lpngdec -lnet
LIBDIRS		+=	../system-l1ght ../nestopia ../gambatte ../vbam

TITLE		:=	Mednafen
APPID		:=	MDFN90002
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000

CFLAGS		+= -O2 -mcpu=cell --std=gnu99 $(MEDNAFENFLAGS) $(MEDNAFENCONFIG) -I$(PSL1GHT)/include/freetype2 
CXXFLAGS	+= -O2 -mcpu=cell $(MEDNAFENCFLAGS) $(MEDNAFENFLAGS) $(MEDNAFENCONFIG) -I$(PSL1GHT)/include/freetype2

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export BUILDDIR	:=	$(CURDIR)/$(BUILD)
export DEPSDIR	:=	$(BUILDDIR)
export VPATH	:= ../

CFILES		:= $(foreach dir,$(SOURCE),$(wildcard $(dir)/*.c))
CXXFILES	:= $(foreach dir,$(SOURCE),$(wildcard $(dir)/*.cpp))
SFILES		:= $(foreach dir,$(SOURCE),$(wildcard $(dir)/*.S))
BINFILES	:= $(foreach dir,$(DATA),$(wildcard $(dir)/*.bin))

#SNES Later yo
#-DWANT_SNES_EMU -DLIBCO_PPC_ASM 
#CFILES		+= src/src/snes/src/lib/libco/libco.c
#
#CXXFILES	+=	src/src/snes/interface.cpp src/src/snes/src/cartridge/cartridge.cpp src/src/snes/src/cheat/cheat.cpp \
#				src/src/snes/src/memory/memory.cpp src/src/snes/src/memory/smemory/smemory.cpp src/src/snes/src/cpu/cpu.cpp src/src/snes/src/cpu/core/core.cpp src/src/snes/src/cpu/scpu/scpu.cpp \
#				src/src/snes/src/smp/smp.cpp src/src/snes/src/smp/core/core.cpp src/src/snes/src/smp/ssmp/ssmp.cpp src/src/snes/src/dsp/sdsp/sdsp.cpp src/src/snes/src/ppu/ppu.cpp \
#				src/src/snes/src/ppu/bppu/bppu.cpp src/src/snes/src/system/system.cpp src/src/snes/src/chip/sa1/sa1.cpp src/src/snes/src/chip/bsx/bsx.cpp src/src/snes/src/chip/srtc/srtc.cpp \
#				src/src/snes/src/chip/sdd1/sdd1.cpp src/src/snes/src/chip/spc7110/spc7110.cpp src/src/snes/src/chip/cx4/cx4.cpp src/src/snes/src/chip/dsp1/dsp1.cpp src/src/snes/src/chip/dsp2/dsp2.cpp \
#				src/src/snes/src/chip/dsp3/dsp3.cpp src/src/snes/src/chip/dsp4/dsp4.cpp src/src/snes/src/chip/obc1/obc1.cpp src/src/snes/src/chip/st010/st010.cpp src/src/snes/src/chip/st011/st011.cpp \
#				src/src/snes/src/chip/st018/st018.cpp src/src/snes/src/chip/superfx/superfx.cpp src/src/snes/src/chip/21fx/21fx.cpp
#
#SFILES		+= src/src/snes/src/lib/libco/blargg_libco_ppc64-5/ppc.S

export OFILES	:=	$(CFILES:.c=.o) \
					$(CXXFILES:.cpp=.o) \
					$(SFILES:.S=.o)					

export BINFILES	:=	$(BINFILES:.bin=.bin.h)

export INCLUDES	:=	$(foreach dir,$(INCLUDE),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD)
export LIBPATHS :=      $(foreach dir,$(LIBDIRS),-L$(CURDIR)/$(dir))

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo Clean...
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).a pkg/USRDIR/EBOOT.BIN $(OUTPUT).pkg

pkg: $(BUILD)
	@echo Creating PKG...
	@$(FSELF) -n $(BUILD)/$(TARGET).elf pkg/USRDIR/EBOOT.BIN
	@$(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) pkg/PARAM.SFO
	@$(PKG) --contentid $(CONTENTID) pkg/ $(OUTPUT).pkg

run: $(BUILD)
	@$(PS3LOADAPP) $(OUTPUT).self

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
$(OFILES): $(BINFILES)

-include $(DEPENDS)

endif
