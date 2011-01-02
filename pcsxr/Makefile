.SUFFIXES:
ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

TARGET		:=	libpcsxr
TYPE		:=	a
BUILD		:=	build
SOURCE		:=	src/libpcsxcore src src/dfxvideo 
INCLUDE		:=	src src/libpcsxcore ../mednafen/src
DATA		:=	bin
LIBS		:=	
LIBDIRS		:=	
CFLAGS		:=-O2 -DNOPSXREC -fgnu89-inline
CXXFLAGS	:=-O2 -DNOPSXREC

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
