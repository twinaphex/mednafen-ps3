ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libemusys
TYPE		:=	lib
BUILD		:=	build
SOURCE		:=	$(PLATFORM) src opengl_common src/utility src/utility/Files src/utility/GUI src/utility/GUI/Summerface
SOURCE		+=	src/thirdparty/fex/fex src/thirdparty/fex/7z_C src/thirdparty/utf8_decode src/thirdparty/simpleini
INCLUDE		:=	. src src/thirdparty/fex

#ifneq ($(strip $(NO_LUA)),1)
#	SOURCE	+=	src/lua
#	INCLUDE	+=	src/lua
#else
#	CXXFLAGS+=	-DNO_LUA
#endif

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif

