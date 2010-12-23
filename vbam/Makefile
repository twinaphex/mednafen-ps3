.SUFFIXES:
TARGET		:=	libvbam.a
BUILD		:=	build
SOURCE		:=	src src/src src/src/common src/src/apu src/src/gb src/src/gba 
INCLUDE		:=	../mednafen/src src/src src/fex

CFLAGS		:=-O2 --std=gnu99 -DWORDS_BIGENDIAN -D__ppc__ -DC_CORE -D__POWERPC__ -DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER -DSIZEOF_DOUBLE=8 -DMSB_FIRST -DHAVE_MKDIR
CXXFLAGS	:=-O2 -DWORDS_BIGENDIAN -D__ppc__ -DC_CORE -D__POWERPC__ -DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER -DSIZEOF_DOUBLE=8 -DMSB_FIRST -DHAVE_MKDIR

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile.base
else
include $(CURDIR)/../../Makefile.base
endif

vbam-objmangle: $(BUILD)
	$(PREFIX)objcopy --prefix-symbols vbam $(CURDIR)/libvbam.a
	$(PREFIX)objcopy --redefine-syms syms.txt $(CURDIR)/libvbam.a
