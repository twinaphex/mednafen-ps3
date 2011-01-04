.SUFFIXES:
ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

TARGET		:=	libpcsxr
TYPE		:=	a
BUILD		:=	build
SOURCE		:=	src/libpcsxcore src src/gxvideo src/dfinput src/dfsound
INCLUDE		:=	src src/libpcsxcore ../mednafen/src ../system
DATA		:=	bin
LIBS		:=	
LIBDIRS		:=	
CFLAGS		:=-O2 -DNOPSXREC -fgnu89-inline -D__BIGENDIAN__ -D__BIG_ENDIAN__
CXXFLAGS	:=-O2 -DNOPSXREC 

#-D_WINDOWS

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
