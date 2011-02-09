ifeq ($(strip $(PLATFORM)),)
	PLATFORM = l1ght
endif

TARGET		:=	libvbam
ifeq ($(strip $(PLATFORM)),dll)
TYPE		:=	so
else
TYPE		:=	a
endif
SONAME		:=	libvbam
SYMPREFIX	:=	vbam
BUILD		:=	build
SOURCE		:=	src src/src src/src/common src/src/apu src/src/gb src/src/gba 
INCLUDE		:=	../mednafen/src src/src src/fex

ifeq ($(strip $(PLATFORM)),dll)
SOURCE		+=	src/fex/fex src/fex/7z_C
endif


CFLAGS		:=	-DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER 
CXXFLAGS	:=	-DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER 

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif

