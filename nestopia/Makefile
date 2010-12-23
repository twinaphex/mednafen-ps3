.SUFFIXES:
ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

TARGET		:=	libnestopia.a
BUILD		:=	build
SOURCE		:=	src src/core src/core/api src/core/board src/core/input src/core/vssystem
INCLUDE		:=	../mednafen/src
DATA		:=	bin
LIBS		:=	
LIBDIRS		:=	
CFLAGS		:=-O2 --std=gnu99 -DNST_NO_ZLIB
CXXFLAGS	:=-O2 -DNST_NO_ZLIB

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
