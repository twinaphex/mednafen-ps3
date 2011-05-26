ifeq ($(strip $(PLATFORM)),)
	PLATFORM = l1ght
endif

TARGET		:=	libemusys
TYPE		:=	a
BUILD		:=	build
SOURCE		:=	$(PLATFORM) src opengl_common src/utility src/utility/Files src/utility/GUI src/fex/fex src/fex/7z_C src/utility/GUI/Summerface src/lua src/lua/lua
INCLUDE		:=	. src src/lua src/fex

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
