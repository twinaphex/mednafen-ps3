.SUFFIXES:
ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

TARGET		:=	libemusys
TYPE		:=	a
BUILD		:=	build
SOURCE		:=	$(PLATFORM) src/utility src/utility/Files src/utility/GUI src/fex/fex src/fex/7z_C 
INCLUDE		:=	. src/fex
LIBS		:=	
LIBDIRS		:=	
CFLAGS		:=-O2 --std=gnu99 
CXXFLAGS	:=-O2

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
