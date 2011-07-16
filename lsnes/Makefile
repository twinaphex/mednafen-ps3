ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	liblsnes
TYPE		:=	lib
BUILD		:=	build
SOURCE		:=	src
INCLUDE		:=	../mednafen/src src

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif

