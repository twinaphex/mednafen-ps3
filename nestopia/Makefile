ifeq ($(strip $(PLATFORM)),)
	PLATFORM = l1ght
endif

TARGET		:=	libnestopia
ifeq ($(strip $(PLATFORM)),dll)
TYPE		:=	so
else
TYPE		:=	a
endif
SONAME		:=	libnestopia
SYMPREFIX	:=	nest
BUILD		:=	build
SOURCE		:=	src src/core src/core/api src/core/board src/core/input src/core/vssystem
INCLUDE		:=	../mednafen/src
CFLAGS		:=	-DNST_NO_ZLIB -DNST_NO_SCALEX -DNST_NO_HQ2X -DNST_NO_2XSAI
CXXFLAGS	:=	-DNST_NO_ZLIB -DNST_NO_SCALEX -DNST_NO_HQ2X -DNST_NO_2XSAI

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
