ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET		:=	liblsnes
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src

CXXFILES	+=	src/mednafen.cpp

include $(CURDIR)/../Makefile-$(PLATFORM).base

