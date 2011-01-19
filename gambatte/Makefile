ifeq ($(strip $(PLATFORM)),)
	PLATFORM = l1ght
endif

TARGET		:=	libgambatte
TYPE		:=	a
SYMPREFIX	:=	gmbt
BUILD		:=	build
SOURCE		:=	src src/libgambatte/src src/libgambatte/src/file src/libgambatte/src/sound src/libgambatte/src/video src/common src/common/resample
INCLUDE		:=	../mednafen/src src/libgambatte/include src/common src/libgambatte/src ../system

CFLAGS		:=	-DHAVE_STDINT_H
CXXFLAGS	:=	-DHAVE_STDINT_H

ifneq ($(BUILD),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
