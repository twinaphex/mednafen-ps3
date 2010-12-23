.SUFFIXES:
TARGET		:=	libgambatte.a
BUILD		:=	build
SOURCE		:=	src src/libgambatte/src src/libgambatte/src/file src/libgambatte/src/sound src/libgambatte/src/video src/common src/common/resample src/libgambatte/src/video/filters
INCLUDE		:=	../mednafen/src src/libgambatte/include src/common src/libgambatte/src ../system-l1ght

CFLAGS		:=-O2 --std=gnu99 -DHAVE_STDINT_H -DSIZEOF_DOUBLE=8 -DMSB_FIRST -DHAVE_MKDIR
CXXFLAGS	:=-O2 -DHAVE_STDINT_H -DSIZEOF_DOUBLE=8 -DMSB_FIRST -DHAVE_MKDIR

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile.base
else
include $(CURDIR)/../../Makefile.base
endif
