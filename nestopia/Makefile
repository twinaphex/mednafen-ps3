ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libnestopia
TYPE		:=	lib
SOURCE		:=	src src/core src/core/api src/core/board src/core/input src/core/vssystem
INCLUDE		:=	../mednafen/src

CFLAGS		:=	-DNST_NO_ZLIB -DNST_NO_SCALEX -DNST_NO_HQ2X -DNST_NO_2XSAI
CXXFLAGS	:=	-DNST_NO_ZLIB -DNST_NO_SCALEX -DNST_NO_HQ2X -DNST_NO_2XSAI

include $(CURDIR)/../Makefile-$(PLATFORM).base

