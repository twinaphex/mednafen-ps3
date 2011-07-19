ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libgambatte
TYPE		:=	lib
SOURCE		:=	src src/libgambatte/src src/libgambatte/src/file src/libgambatte/src/sound src/libgambatte/src/video src/common src/common/resample
INCLUDE		:=	../mednafen/src src/libgambatte/include src/common src/libgambatte/src ../system

CFLAGS		:=	-DHAVE_STDINT_H
CXXFLAGS	:=	-DHAVE_STDINT_H

include $(CURDIR)/../Makefile-$(PLATFORM).base

