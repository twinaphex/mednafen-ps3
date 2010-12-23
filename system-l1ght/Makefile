.SUFFIXES:
TARGET		:=	libemusys.a
BUILD		:=	build
SOURCE		:=	src src/utility src/utility/Files src/utility/GUI src/fex/fex src/fex/7z_C 
INCLUDE		:=	. src/fex
DATA		:=	bin
LIBS		:=	
LIBDIRS		:=	
CFLAGS		:=-O2 --std=gnu99 
CXXFLAGS	:=-O2

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile.base
else
include $(CURDIR)/../../Makefile.base
endif
