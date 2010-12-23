.SUFFIXES:
TARGET		:=	libnestopia.a
BUILD		:=	build
SOURCE		:=	src src/core src/core/api src/core/board src/core/input src/core/vssystem
INCLUDE		:=	../mednafen/src
DATA		:=	bin
LIBS		:=	
LIBDIRS		:=	
CFLAGS		:=-O2 --std=gnu99 -DNST_NO_ZLIB -DSIZEOF_DOUBLE=8 -DMSB_FIRST -DHAVE_MKDIR
CXXFLAGS	:=-O2 -DNST_NO_ZLIB -DSIZEOF_DOUBLE=8 -DMSB_FIRST -DHAVE_MKDIR

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile.base
else
include $(CURDIR)/../../Makefile.base
endif
