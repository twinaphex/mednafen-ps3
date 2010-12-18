.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "PSL1GHT must be set in the environment.")
endif

include $(PSL1GHT)/Makefile.base

TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCE		:=	src src/utility src/utility/Files src/utility/GUI src/fex/fex src/fex/7z_C 
INCLUDE		:=	. src/fex
DATA		:=	bin
LIBS		:=	
LIBDIRS		+=	

CFLAGS		+=-O2 --std=gnu99 -I$(PSL1GHT)/include/freetype2
CXXFLAGS	+=-O2 -mcpu=cell -I$(PSL1GHT)/include/freetype2

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/libemusys
export VPATH	:=	$(foreach dir,$(SOURCE),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export BUILDDIR	:=	$(CURDIR)/$(BUILD)
export DEPSDIR	:=	$(BUILDDIR)

CFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES	:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:= $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.bin)))
VCGFILES	:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.vcg)))


export OFILES	:=	$(CFILES:.c=.o) \
					$(CXXFILES:.cpp=.o) \
					$(SFILES:.S=.o) \
					$(BINFILES:.bin=.bin.o) \
					$(VCGFILES:.vcg=.vcg.o)


export BINFILES	:=	$(BINFILES:.bin=.bin.h)
export VCGFILES	:=	$(VCGFILES:.vcg=.vcg.h)

export INCLUDES	:=	$(foreach dir,$(INCLUDE),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD)
export LIBPATHS :=      $(foreach dir,$(LIBDIRS),-L$(CURDIR)/$(dir))

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo Clean...
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).a $(OUTPUT).pkg

pkg: $(BUILD)
	@echo Creating PKG...
	@mkdir -p $(BUILD)/pkg
	@mkdir -p $(BUILD)/pkg/USRDIR
	@cp $(ICON0) $(BUILD)/pkg/
	@$(FSELF) -n $(BUILD)/$(TARGET).elf $(BUILD)/pkg/USRDIR/EBOOT.BIN
	@$(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) $(BUILD)/pkg/PARAM.SFO
	@$(PKG) --contentid $(CONTENTID) $(BUILD)/pkg/ $(OUTPUT).pkg

run: $(BUILD)
	@$(PS3LOADAPP) $(OUTPUT).self

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT).a: $(OFILES)
$(OFILES): $(BINFILES)  $(VCGFILES)

-include $(DEPENDS)

endif
