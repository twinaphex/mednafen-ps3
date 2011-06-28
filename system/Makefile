ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libemusys
TYPE		:=	lib
BUILD		:=	build
SOURCE		:=	$(PLATFORM) src opengl_common src/utility src/utility/Files src/utility/GUI src/fex/fex src/fex/7z_C src/utility/GUI/Summerface
INCLUDE		:=	. src src/fex

ifneq ($(strip $(NO_LUA)),1)
	SOURCE	+=	src/lua
	INCLUDE	+=	src/lua
else
	CXXFLAGS+=	-DNO_LUA
endif

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif

