ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	liblsnes
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src

CXXFILES	+=	src/mednafen.cpp

include $(CURDIR)/../Makefile-$(PLATFORM).base

