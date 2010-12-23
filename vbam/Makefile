.SUFFIXES:
ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

TARGET		:=	libvbam.a
BUILD		:=	build
SOURCE		:=	src src/src src/src/common src/src/apu src/src/gb src/src/gba 
INCLUDE		:=	../mednafen/src src/src src/fex

CFLAGS		:=-O2 --std=gnu99 -DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER 
CXXFLAGS	:=-O2 -DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER 

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif

vbam-objmangle: $(BUILD)
	$(OBJCOPY) --prefix-symbols vbam $(CURDIR)/libvbam.a
	$(OBJCOPY) --redefine-syms syms.txt $(CURDIR)/libvbam.a
