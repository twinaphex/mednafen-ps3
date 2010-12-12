.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "PSL1GHT must be set in the environment.")
endif

include $(PSL1GHT)/Makefile.base

SFOXML		=	package/sfo.xml
ICON0		=	package/ICON0.png

MEDNAFENFLAGS=-DWANT_NES_EMU -DWANT_SMS_EMU -DWANT_GB_EMU -DWANT_GBA_EMU -DWANT_MD_EMU -DWANT_WSWAN_EMU -DWANT_NGP_EMU -DWANT_PCE_EMU -DWANT_PCE_FAST_EMU -DWANT_VB_EMU -DWANT_LYNX_EMU -DNEED_Z80_EMULATOR -DNETWORK
MEDNAFENCONFIG=-DPSS_STYLE=1 -DMSB_FIRST -DWORDS_BIGENDIAN -DHAVE_MKDIR -DMEDNAFEN_VERSION=\"0.9.15\" -DMEDNAFEN_VERSION_NUMERIC=0x915 -DSIZEOF_DOUBLE=8
MEDNAFENINCLUDES=. src/src/hw_misc src/src/hw_video src/include src/src/hw_cpu src/src/sms src/src/hw_sound src/include/blip src/src/ngp src/src/ngp/TLCS-900h
MEDNAFENCFLAGS=-c -w


LD=$(CXX)
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCE		:=	src src/src src/src/cdrom src/src/compress src/src/gb src/src/gba src/src/hw_cpu/c68k src/src/hw_cpu/huc6280 src/src/hw_cpu/v810 \
				src/src/hw_cpu/v810/fpu-new src/src/hw_cpu/z80-fuse src/src/hw_misc/arcade_card src/src/hw_sound/gb_apu src/src/hw_sound/pce_psg \
				src/src/hw_sound/sms_apu src/src/hw_sound/ym2413 src/src/hw_sound/ym2612 src/src/hw_video/huc6270 src/src/lynx src/src/md \
				src/src/md/cart src/src/md/input src/src/mpcdec src/src/nes src/src/nes/boards src/src/nes/input src/src/nes/ntsc src/src/nes/ppu \
				src/src/nes/ppu/palettes src/src/ngp src/src/ngp/TLCS-900h src/src/pce src/src/pce/input src/src/pce_fast \
				src/src/sms src/src/sound src/src/string src/src/tremor src/src/trio src/src/vb src/src/video src/src/wswan src/src/md/cd

INCLUDE		:=	src include ../system-l1ght ../system-l1ght/src/fex  $(MEDNAFENINCLUDES) 
DATA		:=	data
LIBS		:=	-lemusys -lnestopia -lgambatte -lvbam -laudio -lgcm_sys -lreality -lsysutil -lio -lz -lm -lfreetype -lsysmodule -lpngdec
LIBDIRS		+=	../system-l1ght ../nestopia ../gambatte ../vbam

TITLE		:=	Mednafen
APPID		:=	MDFN90002
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000

CFLAGS		+= -O2 -mcpu=cell --std=gnu99 $(MEDNAFENCFLAGS) $(MEDNAFENFLAGS) $(MEDNAFENCONFIG) -I$(PSL1GHT)/include/freetype2 
CXXFLAGS	+= -O2 -mcpu=cell $(MEDNAFENCFLAGS) $(MEDNAFENFLAGS) $(MEDNAFENCONFIG) -I$(PSL1GHT)/include/freetype2

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCE),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export BUILDDIR	:=	$(CURDIR)/$(BUILD)
export DEPSDIR	:=	$(BUILDDIR)

CFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES	:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:= $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.bin)))

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
