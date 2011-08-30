ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

##Package Details
SFOXML				:=	package/sfo.xml
ICON0				:=	package/ICON0.png
TITLE				:=	Mednafen
APPID				:=	MDFN90002
CONTENTID			:=	UP0001-$(APPID)_00-0000000000000000
PACKAGE				:=	mednafen.pkg

##Main Files
CXXFILES			+=	src/src/cdplay.cpp src/src/dis6502.cpp src/src/file.cpp src/src/math_ops.cpp src/src/memory.cpp src/src/netplay.cpp src/src/player.cpp src/src/settings.cpp \
						src/src/debug.cpp src/src/endian.cpp src/src/FileWrapper.cpp src/src/md5.cpp src/src/mempatcher.cpp src/src/okiadpcm.cpp src/src/PSFLoader.cpp src/src/state.cpp \
						src/src/dis6280.cpp src/src/error.cpp src/src/general.cpp src/src/mednafen.cpp src/src/movie.cpp src/src/qtrecord.cpp src/src/tests.cpp

CFILES				+=	src/src/resampler/resample.c

##Tremor (vorbis decoder)
CFILES				+=	src/src/tremor/bitwise.c src/src/tremor/block.c src/src/tremor/codebook.c src/src/tremor/floor0.c src/src/tremor/floor1.c src/src/tremor/framing.c \
						src/src/tremor/info.c src/src/tremor/mapping0.c src/src/tremor/mdct.c src/src/tremor/registry.c src/src/tremor/res012.c src/src/tremor/sharedbook.c \
						src/src/tremor/synthesis.c 	src/src/tremor/vorbisfile.c src/src/tremor/window.c

###HW Modules
##CPU
CXXFILES			+=	src/src/hw_cpu/z80-fuse/z80.cpp src/src/hw_cpu/z80-fuse/z80_ops.cpp
CXXFILES			+=	src/src/hw_cpu/v810/v810_cpu.cpp src/src/hw_cpu/v810/v810_cpuD.cpp
CXXFILES			+=	src/src/hw_cpu/huc6280/huc6280.cpp

CFILES				+=	src/src/hw_cpu/v810/fpu-new/softfloat.c
CFILES				+=	src/src/hw_cpu/c68k/c68k.c src/src/hw_cpu/c68k/c68kexec.c

##VIDEO
CXXFILES			+=	src/src/hw_video/huc6270/vdc.cpp

##SOUND
CXXFILES			+=	src/src/hw_sound/ym2612/Ym2612_Emu.cpp
CXXFILES			+=	src/src/hw_sound/ym2413/emu2413.cpp
#CXXFILES			+=	src/src/hw_sound/gb_apu/Gb_Apu.cpp src/src/hw_sound/gb_apu/Gb_Apu_State.cpp src/src/hw_sound/gb_apu/Gb_Oscs.cpp
CXXFILES			+=	src/src/hw_sound/sms_apu/Sms_Apu.cpp
CXXFILES			+=	src/src/hw_sound/pce_psg/pce_psg.cpp

##MISC
CXXFILES			+=	src/src/hw_misc/arcade_card/arcade_card.cpp



##Source files for modules
CPPFLAGS			+=	-DWANT_NES_EMU -DWANT_VB_EMU -DWANT_SMS_EMU -DWANT_MD_EMU -DWANT_PCE_EMU -DWANT_PCEFAST_EMU -DWANT_PCFX_EMU -DWANT_WSWAN_EMU -DWANT_LYNX_EMU -DWANT_NGP_EMU
#-DWANT_SNES_EMU -DWANT_GB_EMU -DWANT_GBA_EMU

CXXFILES			+=	src/src/nes/cart.cpp src/src/nes/fds.cpp src/src/nes/fds-sound.cpp src/src/nes/filter.cpp src/src/nes/ines.cpp \
						src/src/nes/input.cpp src/src/nes/nes.cpp src/src/nes/nsf.cpp src/src/nes/nsfe.cpp src/src/nes/sound.cpp src/src/nes/unif.cpp \
						src/src/nes/vsuni.cpp src/src/nes/x6502.cpp src/src/nes/boards/107.cpp src/src/nes/boards/112.cpp src/src/nes/boards/113.cpp \
						src/src/nes/boards/114.cpp src/src/nes/boards/117.cpp src/src/nes/boards/140.cpp src/src/nes/boards/151.cpp src/src/nes/boards/152.cpp \
						src/src/nes/boards/156.cpp src/src/nes/boards/15.cpp src/src/nes/boards/163.cpp src/src/nes/boards/16.cpp src/src/nes/boards/180.cpp \
						src/src/nes/boards/182.cpp src/src/nes/boards/184.cpp src/src/nes/boards/185.cpp src/src/nes/boards/187.cpp src/src/nes/boards/189.cpp \
						src/src/nes/boards/18.cpp src/src/nes/boards/193.cpp src/src/nes/boards/208.cpp src/src/nes/boards/21.cpp src/src/nes/boards/222.cpp \
						src/src/nes/boards/228.cpp src/src/nes/boards/22.cpp src/src/nes/boards/232.cpp src/src/nes/boards/234.cpp src/src/nes/boards/23.cpp \
						src/src/nes/boards/240.cpp src/src/nes/boards/241.cpp src/src/nes/boards/242.cpp src/src/nes/boards/244.cpp src/src/nes/boards/246.cpp \
						src/src/nes/boards/248.cpp src/src/nes/boards/25.cpp src/src/nes/boards/32.cpp src/src/nes/boards/33.cpp src/src/nes/boards/34.cpp \
						src/src/nes/boards/38.cpp src/src/nes/boards/41.cpp src/src/nes/boards/42.cpp src/src/nes/boards/46.cpp src/src/nes/boards/51.cpp \
						src/src/nes/boards/65.cpp src/src/nes/boards/67.cpp src/src/nes/boards/68.cpp src/src/nes/boards/70.cpp src/src/nes/boards/72.cpp \
						src/src/nes/boards/73.cpp src/src/nes/boards/75.cpp src/src/nes/boards/76.cpp src/src/nes/boards/77.cpp src/src/nes/boards/78.cpp \
						src/src/nes/boards/80.cpp src/src/nes/boards/8237.cpp src/src/nes/boards/82.cpp src/src/nes/boards/86.cpp src/src/nes/boards/87.cpp \
						src/src/nes/boards/88.cpp src/src/nes/boards/89.cpp src/src/nes/boards/8.cpp src/src/nes/boards/90.cpp src/src/nes/boards/92.cpp \
						src/src/nes/boards/93.cpp src/src/nes/boards/94.cpp src/src/nes/boards/95.cpp src/src/nes/boards/96.cpp src/src/nes/boards/97.cpp \
						src/src/nes/boards/99.cpp src/src/nes/boards/codemasters.cpp src/src/nes/boards/colordreams.cpp src/src/nes/boards/deirom.cpp \
						src/src/nes/boards/emu2413.cpp src/src/nes/boards/ffe.cpp src/src/nes/boards/fme7.cpp src/src/nes/boards/h2288.cpp \
						src/src/nes/boards/malee.cpp src/src/nes/boards/maxicart.cpp src/src/nes/boards/mmc1.cpp src/src/nes/boards/mmc2and4.cpp \
						src/src/nes/boards/mmc3.cpp src/src/nes/boards/mmc5.cpp src/src/nes/boards/n106.cpp src/src/nes/boards/nina06.cpp \
						src/src/nes/boards/novel.cpp src/src/nes/boards/sachen.cpp src/src/nes/boards/simple.cpp src/src/nes/boards/super24.cpp \
						src/src/nes/boards/supervision.cpp src/src/nes/boards/tengen.cpp src/src/nes/boards/vrc6.cpp src/src/nes/boards/vrc7.cpp \
						src/src/nes/input/arkanoid.cpp src/src/nes/input/bworld.cpp src/src/nes/input/cursor.cpp src/src/nes/input/fkb.cpp \
						src/src/nes/input/ftrainer.cpp src/src/nes/input/hypershot.cpp src/src/nes/input/mahjong.cpp src/src/nes/input/oekakids.cpp \
						src/src/nes/input/partytap.cpp src/src/nes/input/powerpad.cpp src/src/nes/input/shadow.cpp src/src/nes/input/suborkb.cpp \
						src/src/nes/input/toprider.cpp src/src/nes/input/zapper.cpp src/src/nes/ntsc/nes_ntsc.cpp src/src/nes/ppu/palette.cpp \
						src/src/nes/ppu/ppu.cpp

#CXXFILES			:=	src/src/snes/interface.cpp src/src/snes/src/cartridge/cartridge.cpp src/src/snes/src/cheat/cheat.cpp src/src/snes/src/memory/memory.cpp \
#						src/src/snes/src/memory/smemory/smemory.cpp src/src/snes/src/cpu/cpu.cpp src/src/snes/src/cpu/core/core.cpp src/src/snes/src/cpu/scpu/scpu.cpp \
#						src/src/snes/src/smp/smp.cpp src/src/snes/src/smp/core/core.cpp src/src/snes/src/smp/ssmp/ssmp.cpp src/src/snes/src/dsp/sdsp/sdsp.cpp \
#						src/src/snes/src/ppu/ppu.cpp src/src/snes/src/ppu/bppu/bppu.cpp src/src/snes/src/system/system.cpp src/src/snes/src/chip/sa1/sa1.cpp \
#						src/src/snes/src/chip/bsx/bsx.cpp src/src/snes/src/chip/srtc/srtc.cpp src/src/snes/src/chip/sdd1/sdd1.cpp src/src/snes/src/chip/spc7110/spc7110.cpp \
#						src/src/snes/src/chip/cx4/cx4.cpp src/src/snes/src/chip/dsp1/dsp1.cpp src/src/snes/src/chip/dsp2/dsp2.cpp src/src/snes/src/chip/dsp3/dsp3.cpp \
#						src/src/snes/src/chip/dsp4/dsp4.cpp src/src/snes/src/chip/obc1/obc1.cpp src/src/snes/src/chip/st010/st010.cpp src/src/snes/src/chip/st011/st011.cpp \
#						src/src/snes/src/chip/st018/st018.cpp src/src/snes/src/chip/superfx/superfx.cpp src/src/snes/src/chip/21fx/21fx.cpp
#CFILES				:=	src/src/snes/src/lib/libco/libco.c


#CXXFILES			+=	src/src/gb/gb.cpp src/src/gb/gbGlobals.cpp src/src/gb/gfx.cpp src/src/gb/memory.cpp src/src/gb/sound.cpp src/src/gb/z80.cpp

#CXXFILES			+=	src/src/gba/arm.cpp src/src/gba/bios.cpp src/src/gba/eeprom.cpp src/src/gba/flash.cpp src/src/gba/GBA.cpp src/src/gba/GBAinline.cpp \
#						src/src/gba/Gfx.cpp src/src/gba/Globals.cpp src/src/gba/Mode0.cpp src/src/gba/Mode1.cpp src/src/gba/Mode2.cpp src/src/gba/Mode3.cpp \
#						src/src/gba/Mode4.cpp src/src/gba/Mode5.cpp src/src/gba/RTC.cpp src/src/gba/Sound.cpp src/src/gba/sram.cpp src/src/gba/thumb.cpp

CXXFILES			+=	src/src/vb/input.cpp src/src/vb/timer.cpp src/src/vb/vb.cpp src/src/vb/vip.cpp src/src/vb/vsu.cpp

CXXFILES			+=	src/src/sms/cart.cpp src/src/sms/pio.cpp src/src/sms/romdb.cpp src/src/sms/sound.cpp src/src/sms/tms.cpp src/src/sms/memz80.cpp src/src/sms/render.cpp \
						src/src/sms/sms.cpp src/src/sms/system.cpp src/src/sms/vdp.cpp

CXXFILES			+=	src/src/md/cart/cart.cpp src/src/md/cart/map_eeprom.cpp src/src/md/cart/map_ff.cpp src/src/md/cart/map_realtec.cpp src/src/md/cart/map_rmx3.cpp \
						src/src/md/cart/map_rom.cpp src/src/md/cart/map_sbb.cpp src/src/md/cart/map_sram.cpp src/src/md/cart/map_ssf2.cpp src/src/md/cart/map_svp.cpp \
						src/src/md/cart/map_yase.cpp src/src/md/cd/cdc_cdd.cpp src/src/md/cd/cd.cpp src/src/md/cd/interrupt.cpp src/src/md/cd/pcm.cpp src/src/md/cd/timer.cpp \
						src/src/md/genesis.cpp src/src/md/genio.cpp src/src/md/header.cpp src/src/md/input/gamepad.cpp src/src/md/input/mouse.cpp src/src/md/mem68k.cpp \
						src/src/md/membnk.cpp src/src/md/memvdp.cpp src/src/md/memz80.cpp src/src/md/sound.cpp src/src/md/system.cpp src/src/md/vdp.cpp

CXXFILES			+=	src/src/pce/hes.cpp src/src/pce/huc.cpp src/src/pce/input.cpp src/src/pce/pce.cpp src/src/pce/subhw.cpp src/src/pce/tsushin.cpp src/src/pce/vce.cpp \
						src/src/pce/input/gamepad.cpp src/src/pce/input/mouse.cpp src/src/pce/input/tsushinkb.cpp

CXXFILES			+=	src/src/pce_fast/hes.cpp src/src/pce_fast/huc.cpp src/src/pce_fast/pce.cpp src/src/pce_fast/vdc.cpp src/src/pce_fast/huc6280.cpp src/src/pce_fast/input.cpp \
						src/src/pce_fast/tsushin.cpp

CXXFILES			+=	src/src/pcfx/huc6273.cpp src/src/pcfx/interrupt.cpp src/src/pcfx/king.cpp src/src/pcfx/rainbow.cpp src/src/pcfx/timer.cpp src/src/pcfx/fxscsi.cpp \
						src/src/pcfx/input.cpp src/src/pcfx/jrevdct.cpp src/src/pcfx/pcfx.cpp src/src/pcfx/soundbox.cpp src/src/pcfx/input/gamepad.cpp src/src/pcfx/input/mouse.cpp

CXXFILES			+=	src/src/wswan/gfx.cpp src/src/wswan/main.cpp src/src/wswan/rtc.cpp src/src/wswan/tcache.cpp src/src/wswan/eeprom.cpp src/src/wswan/interrupt.cpp \
						src/src/wswan/memory.cpp src/src/wswan/sound.cpp src/src/wswan/v30mz.cpp

CXXFILES			+=	src/src/lynx/c65c02.cpp src/src/lynx/cart.cpp src/src/lynx/memmap.cpp src/src/lynx/mikie.cpp src/src/lynx/ram.cpp src/src/lynx/rom.cpp src/src/lynx/susie.cpp \
						src/src/lynx/system.cpp

CXXFILES			+=	src/src/ngp/bios.cpp src/src/ngp/biosHLE.cpp src/src/ngp/dma.cpp src/src/ngp/flash.cpp src/src/ngp/gfx.cpp src/src/ngp/gfx_scanline_colour.cpp \
						src/src/ngp/gfx_scanline_mono.cpp src/src/ngp/interrupt.cpp src/src/ngp/mem.cpp src/src/ngp/neopop.cpp src/src/ngp/rom.cpp src/src/ngp/rtc.cpp \
						src/src/ngp/sound.cpp src/src/ngp/T6W28_Apu.cpp src/src/ngp/Z80_interface.cpp src/src/ngp/TLCS-900h/TLCS900h_disassemble.cpp \
						src/src/ngp/TLCS-900h/TLCS900h_disassemble_dst.cpp src/src/ngp/TLCS-900h/TLCS900h_disassemble_extra.cpp src/src/ngp/TLCS-900h/TLCS900h_disassemble_reg.cpp \
						src/src/ngp/TLCS-900h/TLCS900h_disassemble_src.cpp src/src/ngp/TLCS-900h/TLCS900h_interpret.cpp src/src/ngp/TLCS-900h/TLCS900h_interpret_dst.cpp \
						src/src/ngp/TLCS-900h/TLCS900h_interpret_reg.cpp src/src/ngp/TLCS-900h/TLCS900h_interpret_single.cpp src/src/ngp/TLCS-900h/TLCS900h_interpret_src.cpp \
						src/src/ngp/TLCS-900h/TLCS900h_registers.cpp

MAINSRC				:=	src/filters src/src/cdrom src/src/compress src/src/mpcdec src/src/sound src/src/string src/src/trio src/src/video
MAINCPP				:=	-DNETWORK -DPSS_STYLE=1 -DHAVE_MKDIR -DMEDNAFEN_VERSION=\"0.9.18\" -DMEDNAFEN_VERSION_NUMERIC=0x918 -DSIZEOF_DOUBLE=8

FRONTENDSRC			:=	src

##Mednafen config
CPPFLAGS			+=	-DMDFNPS3 -DMPC_FIXED_POINT $(MAINCPP)
SOURCE				+=	$(FRONTENDSRC) $(MAINSRC)
INCLUDE				:=	. src ../system ../system/$(PLATFORM) src/src/hw_misc src/src/hw_video src/include src/src/hw_cpu src/src/sms src/src/hw_sound src/include/blip src/src/ngp src/src/ngp/TLCS-900h src/src/snes/src/lib
LIBDIRS				:=	../system ../nestopia ../gambatte ../vbam ../pcsxr ../stella ../yabause
LDFLAGS				+=	-lemusys -lnestopia -lgambatte -lpcsxr -lvbam -lstella -lyabause

#LIBDIRS			+=	../lsnes ../desmume
#LDFLAGS			+=	-llsnes -lsnes -ldesmume

##Build info
TARGET				:=	mednafen
TYPE				:=	bin

include $(CURDIR)/../Makefile-$(PLATFORM).base

