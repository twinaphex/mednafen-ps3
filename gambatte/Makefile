.SUFFIXES:
ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

TARGET		:=	libgambatte
TYPE		:=	a
BUILD		:=	build
SOURCE		:=	src src/libgambatte/src src/libgambatte/src/file src/libgambatte/src/sound src/libgambatte/src/video src/common src/common/resample src/libgambatte/src/video/filters
INCLUDE		:=	../mednafen/src src/libgambatte/include src/common src/libgambatte/src ../system

CFLAGS		:=-O2 --std=gnu99 -DHAVE_STDINT_H
CXXFLAGS	:=-O2 -DHAVE_STDINT_H

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
