ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libyabause
TYPE		:=	lib
SOURCE		:=	src src/yabause src/yabause/c68k
INCLUDE		:=	../mednafen/src src/yabause src/yabause/c68k

CFLAGS		+=	-DMDFNPS3 -DVERSION=\"10\" 

include $(CURDIR)/../Makefile-$(PLATFORM).base

