ifeq ($(strip $(PLATFORM)),)
	PLATFORM = l1ght
endif

TARGET		:=	libemusys
TYPE		:=	a
BUILD		:=	build
SOURCE		:=	$(PLATFORM) src src/utility src/utility/Files src/utility/GUI src/fex/fex src/fex/7z_C src/utility/GUI/Summerface
INCLUDE		:=	. src/fex

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
