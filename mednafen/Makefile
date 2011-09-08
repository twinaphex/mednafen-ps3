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
CXXFILES			+=	src/cheatsearcher.cpp src/ers.cpp src/FastCounter.cpp src/inputhandler.cpp src/main.cpp src/mednafen_help.cpp \
						src/mednafen_implement.cpp src/savestates.cpp src/SettingCategoryMenu.cpp src/SettingGroupMenu.cpp src/textviewer.cpp

CXXFILES			+=	src/mednafen/cdplay.cpp src/mednafen/dis6502.cpp src/mednafen/file.cpp src/mednafen/math_ops.cpp src/mednafen/memory.cpp src/mednafen/netplay.cpp src/mednafen/player.cpp src/mednafen/settings.cpp \
						src/mednafen/debug.cpp src/mednafen/endian.cpp src/mednafen/FileWrapper.cpp src/mednafen/md5.cpp src/mednafen/mempatcher.cpp src/mednafen/okiadpcm.cpp src/mednafen/PSFLoader.cpp src/mednafen/state.cpp \
						src/mednafen/dis6280.cpp src/mednafen/error.cpp src/mednafen/general.cpp src/mednafen/mednafen.cpp src/mednafen/movie.cpp src/mednafen/qtrecord.cpp src/mednafen/tests.cpp

CFILES				+=	src/mednafen/resampler/resample.c

CXXFILES			+=	src/mednafen/sound/Blip_Buffer.cpp src/mednafen/sound/Fir_Resampler.cpp src/mednafen/sound/sound.cpp \
						src/mednafen/sound/Stereo_Buffer.cpp src/mednafen/sound/WAVRecord.cpp

CXXFILES			+=	src/mednafen/string/ConvertUTF.cpp src/mednafen/string/escape.cpp
CFILES				+=	src/mednafen/string/world_strtod.c

CFILES				+=	src/mednafen/trio/trio.c src/mednafen/trio/trionan.c src/mednafen/trio/triostr.c

CXXFILES			+=	src/mednafen/video/Deinterlacer.cpp src/mednafen/video/font-data.cpp src/mednafen/video/png.cpp src/mednafen/video/primitives.cpp \
						src/mednafen/video/resize.cpp src/mednafen/video/selblur.cpp src/mednafen/video/surface.cpp src/mednafen/video/tblur.cpp \
						src/mednafen/video/text.cpp src/mednafen/video/video.cpp
CFILES				+=	src/mednafen/video/font-data-12x13.c src/mednafen/video/font-data-18x18.c

##Compression support
CFILES				+=	src/mednafen/compress/blz.c src/mednafen/compress/ioapi.c src/mednafen/compress/minilzo.c src/mednafen/compress/quicklz.c src/mednafen/compress/unzip.c

##CD-ROM handler
CXXFILES			+=	src/mednafen/cdrom/audioreader.cpp src/mednafen/cdrom/cdromfile.cpp src/mednafen/cdrom/cdromif.cpp src/mednafen/cdrom/crc32.cpp \
						src/mednafen/cdrom/galois.cpp src/mednafen/cdrom/l-ec.cpp src/mednafen/cdrom/lec.cpp src/mednafen/cdrom/pcecd.cpp \
						src/mednafen/cdrom/recover-raw.cpp src/mednafen/cdrom/scsicd.cpp src/mednafen/cdrom/SimpleFIFO.cpp

##CD Audio Support
#OGG
CFILES				+=	src/mednafen/tremor/bitwise.c src/mednafen/tremor/block.c src/mednafen/tremor/codebook.c src/mednafen/tremor/floor0.c src/mednafen/tremor/floor1.c src/mednafen/tremor/framing.c \
						src/mednafen/tremor/info.c src/mednafen/tremor/mapping0.c src/mednafen/tremor/mdct.c src/mednafen/tremor/registry.c src/mednafen/tremor/res012.c src/mednafen/tremor/sharedbook.c \
						src/mednafen/tremor/synthesis.c 	src/mednafen/tremor/vorbisfile.c src/mednafen/tremor/window.c

#Musepak
CFILES				+=	src/mednafen/mpcdec/huffsv46.c src/mednafen/mpcdec/huffsv7.c src/mednafen/mpcdec/idtag.c src/mednafen/mpcdec/mpc_decoder.c src/mednafen/mpcdec/mpc_reader.c \
						src/mednafen/mpcdec/requant.c src/mednafen/mpcdec/streaminfo.c src/mednafen/mpcdec/synth_filter.c

CPPFLAGS			+=	-DMPC_FIXED_POINT

###HW Modules
##CPU
CXXFILES			+=	src/mednafen/hw_cpu/z80-fuse/z80.cpp src/mednafen/hw_cpu/z80-fuse/z80_ops.cpp
CXXFILES			+=	src/mednafen/hw_cpu/v810/v810_cpu.cpp src/mednafen/hw_cpu/v810/v810_cpuD.cpp
CXXFILES			+=	src/mednafen/hw_cpu/huc6280/huc6280.cpp

CFILES				+=	src/mednafen/hw_cpu/v810/fpu-new/softfloat.c
CFILES				+=	src/mednafen/hw_cpu/c68k/c68k.c src/mednafen/hw_cpu/c68k/c68kexec.c

##VIDEO
CXXFILES			+=	src/mednafen/hw_video/huc6270/vdc.cpp

##SOUND
CXXFILES			+=	src/mednafen/hw_sound/ym2612/Ym2612_Emu.cpp
CXXFILES			+=	src/mednafen/hw_sound/ym2413/emu2413.cpp
#CXXFILES			+=	src/mednafen/hw_sound/gb_apu/Gb_Apu.cpp src/mednafen/hw_sound/gb_apu/Gb_Apu_State.cpp src/mednafen/hw_sound/gb_apu/Gb_Oscs.cpp
CXXFILES			+=	src/mednafen/hw_sound/sms_apu/Sms_Apu.cpp
CXXFILES			+=	src/mednafen/hw_sound/pce_psg/pce_psg.cpp

##MISC
CXXFILES			+=	src/mednafen/hw_misc/arcade_card/arcade_card.cpp



##Source files for modules
CPPFLAGS			+=	-DWANT_NES_EMU -DWANT_VB_EMU -DWANT_SMS_EMU -DWANT_MD_EMU -DWANT_PCE_EMU -DWANT_PCEFAST_EMU -DWANT_PCFX_EMU -DWANT_WSWAN_EMU -DWANT_LYNX_EMU -DWANT_NGP_EMU
#-DWANT_SNES_EMU -DWANT_GB_EMU -DWANT_GBA_EMU

CXXFILES			+=	src/mednafen/nes/cart.cpp src/mednafen/nes/fds.cpp src/mednafen/nes/fds-sound.cpp src/mednafen/nes/filter.cpp src/mednafen/nes/ines.cpp \
						src/mednafen/nes/input.cpp src/mednafen/nes/nes.cpp src/mednafen/nes/nsf.cpp src/mednafen/nes/nsfe.cpp src/mednafen/nes/sound.cpp src/mednafen/nes/unif.cpp \
						src/mednafen/nes/vsuni.cpp src/mednafen/nes/x6502.cpp src/mednafen/nes/boards/107.cpp src/mednafen/nes/boards/112.cpp src/mednafen/nes/boards/113.cpp \
						src/mednafen/nes/boards/114.cpp src/mednafen/nes/boards/117.cpp src/mednafen/nes/boards/140.cpp src/mednafen/nes/boards/151.cpp src/mednafen/nes/boards/152.cpp \
						src/mednafen/nes/boards/156.cpp src/mednafen/nes/boards/15.cpp src/mednafen/nes/boards/163.cpp src/mednafen/nes/boards/16.cpp src/mednafen/nes/boards/180.cpp \
						src/mednafen/nes/boards/182.cpp src/mednafen/nes/boards/184.cpp src/mednafen/nes/boards/185.cpp src/mednafen/nes/boards/187.cpp src/mednafen/nes/boards/189.cpp \
						src/mednafen/nes/boards/18.cpp src/mednafen/nes/boards/193.cpp src/mednafen/nes/boards/208.cpp src/mednafen/nes/boards/21.cpp src/mednafen/nes/boards/222.cpp \
						src/mednafen/nes/boards/228.cpp src/mednafen/nes/boards/22.cpp src/mednafen/nes/boards/232.cpp src/mednafen/nes/boards/234.cpp src/mednafen/nes/boards/23.cpp \
						src/mednafen/nes/boards/240.cpp src/mednafen/nes/boards/241.cpp src/mednafen/nes/boards/242.cpp src/mednafen/nes/boards/244.cpp src/mednafen/nes/boards/246.cpp \
						src/mednafen/nes/boards/248.cpp src/mednafen/nes/boards/25.cpp src/mednafen/nes/boards/32.cpp src/mednafen/nes/boards/33.cpp src/mednafen/nes/boards/34.cpp \
						src/mednafen/nes/boards/38.cpp src/mednafen/nes/boards/41.cpp src/mednafen/nes/boards/42.cpp src/mednafen/nes/boards/46.cpp src/mednafen/nes/boards/51.cpp \
						src/mednafen/nes/boards/65.cpp src/mednafen/nes/boards/67.cpp src/mednafen/nes/boards/68.cpp src/mednafen/nes/boards/70.cpp src/mednafen/nes/boards/72.cpp \
						src/mednafen/nes/boards/73.cpp src/mednafen/nes/boards/75.cpp src/mednafen/nes/boards/76.cpp src/mednafen/nes/boards/77.cpp src/mednafen/nes/boards/78.cpp \
						src/mednafen/nes/boards/80.cpp src/mednafen/nes/boards/8237.cpp src/mednafen/nes/boards/82.cpp src/mednafen/nes/boards/86.cpp src/mednafen/nes/boards/87.cpp \
						src/mednafen/nes/boards/88.cpp src/mednafen/nes/boards/89.cpp src/mednafen/nes/boards/8.cpp src/mednafen/nes/boards/90.cpp src/mednafen/nes/boards/92.cpp \
						src/mednafen/nes/boards/93.cpp src/mednafen/nes/boards/94.cpp src/mednafen/nes/boards/95.cpp src/mednafen/nes/boards/96.cpp src/mednafen/nes/boards/97.cpp \
						src/mednafen/nes/boards/99.cpp src/mednafen/nes/boards/codemasters.cpp src/mednafen/nes/boards/colordreams.cpp src/mednafen/nes/boards/deirom.cpp \
						src/mednafen/nes/boards/emu2413.cpp src/mednafen/nes/boards/ffe.cpp src/mednafen/nes/boards/fme7.cpp src/mednafen/nes/boards/h2288.cpp \
						src/mednafen/nes/boards/malee.cpp src/mednafen/nes/boards/maxicart.cpp src/mednafen/nes/boards/mmc1.cpp src/mednafen/nes/boards/mmc2and4.cpp \
						src/mednafen/nes/boards/mmc3.cpp src/mednafen/nes/boards/mmc5.cpp src/mednafen/nes/boards/n106.cpp src/mednafen/nes/boards/nina06.cpp \
						src/mednafen/nes/boards/novel.cpp src/mednafen/nes/boards/sachen.cpp src/mednafen/nes/boards/simple.cpp src/mednafen/nes/boards/super24.cpp \
						src/mednafen/nes/boards/supervision.cpp src/mednafen/nes/boards/tengen.cpp src/mednafen/nes/boards/vrc6.cpp src/mednafen/nes/boards/vrc7.cpp \
						src/mednafen/nes/input/arkanoid.cpp src/mednafen/nes/input/bworld.cpp src/mednafen/nes/input/cursor.cpp src/mednafen/nes/input/fkb.cpp \
						src/mednafen/nes/input/ftrainer.cpp src/mednafen/nes/input/hypershot.cpp src/mednafen/nes/input/mahjong.cpp src/mednafen/nes/input/oekakids.cpp \
						src/mednafen/nes/input/partytap.cpp src/mednafen/nes/input/powerpad.cpp src/mednafen/nes/input/shadow.cpp src/mednafen/nes/input/suborkb.cpp \
						src/mednafen/nes/input/toprider.cpp src/mednafen/nes/input/zapper.cpp src/mednafen/nes/ntsc/nes_ntsc.cpp src/mednafen/nes/ppu/palette.cpp \
						src/mednafen/nes/ppu/ppu.cpp

#CXXFILES			:=	src/mednafen/snes/interface.cpp src/mednafen/snes/src/cartridge/cartridge.cpp src/mednafen/snes/src/cheat/cheat.cpp src/mednafen/snes/src/memory/memory.cpp \
#						src/mednafen/snes/src/memory/smemory/smemory.cpp src/mednafen/snes/src/cpu/cpu.cpp src/mednafen/snes/src/cpu/core/core.cpp src/mednafen/snes/src/cpu/scpu/scpu.cpp \
#						src/mednafen/snes/src/smp/smp.cpp src/mednafen/snes/src/smp/core/core.cpp src/mednafen/snes/src/smp/ssmp/ssmp.cpp src/mednafen/snes/src/dsp/sdsp/sdsp.cpp \
#						src/mednafen/snes/src/ppu/ppu.cpp src/mednafen/snes/src/ppu/bppu/bppu.cpp src/mednafen/snes/src/system/system.cpp src/mednafen/snes/src/chip/sa1/sa1.cpp \
#						src/mednafen/snes/src/chip/bsx/bsx.cpp src/mednafen/snes/src/chip/srtc/srtc.cpp src/mednafen/snes/src/chip/sdd1/sdd1.cpp src/mednafen/snes/src/chip/spc7110/spc7110.cpp \
#						src/mednafen/snes/src/chip/cx4/cx4.cpp src/mednafen/snes/src/chip/dsp1/dsp1.cpp src/mednafen/snes/src/chip/dsp2/dsp2.cpp src/mednafen/snes/src/chip/dsp3/dsp3.cpp \
#						src/mednafen/snes/src/chip/dsp4/dsp4.cpp src/mednafen/snes/src/chip/obc1/obc1.cpp src/mednafen/snes/src/chip/st010/st010.cpp src/mednafen/snes/src/chip/st011/st011.cpp \
#						src/mednafen/snes/src/chip/st018/st018.cpp src/mednafen/snes/src/chip/superfx/superfx.cpp src/mednafen/snes/src/chip/21fx/21fx.cpp
#CFILES				:=	src/mednafen/snes/src/lib/libco/libco.c


#CXXFILES			+=	src/mednafen/gb/gb.cpp src/mednafen/gb/gbGlobals.cpp src/mednafen/gb/gfx.cpp src/mednafen/gb/memory.cpp src/mednafen/gb/sound.cpp src/mednafen/gb/z80.cpp

#CXXFILES			+=	src/mednafen/gba/arm.cpp src/mednafen/gba/bios.cpp src/mednafen/gba/eeprom.cpp src/mednafen/gba/flash.cpp src/mednafen/gba/GBA.cpp src/mednafen/gba/GBAinline.cpp \
#						src/mednafen/gba/Gfx.cpp src/mednafen/gba/Globals.cpp src/mednafen/gba/Mode0.cpp src/mednafen/gba/Mode1.cpp src/mednafen/gba/Mode2.cpp src/mednafen/gba/Mode3.cpp \
#						src/mednafen/gba/Mode4.cpp src/mednafen/gba/Mode5.cpp src/mednafen/gba/RTC.cpp src/mednafen/gba/Sound.cpp src/mednafen/gba/sram.cpp src/mednafen/gba/thumb.cpp

CXXFILES			+=	src/mednafen/vb/input.cpp src/mednafen/vb/timer.cpp src/mednafen/vb/vb.cpp src/mednafen/vb/vip.cpp src/mednafen/vb/vsu.cpp

CXXFILES			+=	src/mednafen/sms/cart.cpp src/mednafen/sms/pio.cpp src/mednafen/sms/romdb.cpp src/mednafen/sms/sound.cpp src/mednafen/sms/tms.cpp src/mednafen/sms/memz80.cpp src/mednafen/sms/render.cpp \
						src/mednafen/sms/sms.cpp src/mednafen/sms/system.cpp src/mednafen/sms/vdp.cpp

CXXFILES			+=	src/mednafen/md/cart/cart.cpp src/mednafen/md/cart/map_eeprom.cpp src/mednafen/md/cart/map_ff.cpp src/mednafen/md/cart/map_realtec.cpp src/mednafen/md/cart/map_rmx3.cpp \
						src/mednafen/md/cart/map_rom.cpp src/mednafen/md/cart/map_sbb.cpp src/mednafen/md/cart/map_sram.cpp src/mednafen/md/cart/map_ssf2.cpp src/mednafen/md/cart/map_svp.cpp \
						src/mednafen/md/cart/map_yase.cpp src/mednafen/md/cd/cdc_cdd.cpp src/mednafen/md/cd/cd.cpp src/mednafen/md/cd/interrupt.cpp src/mednafen/md/cd/pcm.cpp src/mednafen/md/cd/timer.cpp \
						src/mednafen/md/genesis.cpp src/mednafen/md/genio.cpp src/mednafen/md/header.cpp src/mednafen/md/input/gamepad.cpp src/mednafen/md/input/mouse.cpp src/mednafen/md/mem68k.cpp \
						src/mednafen/md/membnk.cpp src/mednafen/md/memvdp.cpp src/mednafen/md/memz80.cpp src/mednafen/md/sound.cpp src/mednafen/md/system.cpp src/mednafen/md/vdp.cpp

CXXFILES			+=	src/mednafen/pce/hes.cpp src/mednafen/pce/huc.cpp src/mednafen/pce/input.cpp src/mednafen/pce/pce.cpp src/mednafen/pce/subhw.cpp src/mednafen/pce/tsushin.cpp src/mednafen/pce/vce.cpp \
						src/mednafen/pce/input/gamepad.cpp src/mednafen/pce/input/mouse.cpp src/mednafen/pce/input/tsushinkb.cpp

CXXFILES			+=	src/mednafen/pce_fast/hes.cpp src/mednafen/pce_fast/huc.cpp src/mednafen/pce_fast/pce.cpp src/mednafen/pce_fast/vdc.cpp src/mednafen/pce_fast/huc6280.cpp src/mednafen/pce_fast/input.cpp \
						src/mednafen/pce_fast/tsushin.cpp

CXXFILES			+=	src/mednafen/pcfx/huc6273.cpp src/mednafen/pcfx/interrupt.cpp src/mednafen/pcfx/king.cpp src/mednafen/pcfx/rainbow.cpp src/mednafen/pcfx/timer.cpp src/mednafen/pcfx/fxscsi.cpp \
						src/mednafen/pcfx/input.cpp src/mednafen/pcfx/jrevdct.cpp src/mednafen/pcfx/pcfx.cpp src/mednafen/pcfx/soundbox.cpp src/mednafen/pcfx/input/gamepad.cpp src/mednafen/pcfx/input/mouse.cpp

CXXFILES			+=	src/mednafen/wswan/gfx.cpp src/mednafen/wswan/main.cpp src/mednafen/wswan/rtc.cpp src/mednafen/wswan/tcache.cpp src/mednafen/wswan/eeprom.cpp src/mednafen/wswan/interrupt.cpp \
						src/mednafen/wswan/memory.cpp src/mednafen/wswan/sound.cpp src/mednafen/wswan/v30mz.cpp

CXXFILES			+=	src/mednafen/lynx/c65c02.cpp src/mednafen/lynx/cart.cpp src/mednafen/lynx/memmap.cpp src/mednafen/lynx/mikie.cpp src/mednafen/lynx/ram.cpp src/mednafen/lynx/rom.cpp src/mednafen/lynx/susie.cpp \
						src/mednafen/lynx/system.cpp

CXXFILES			+=	src/mednafen/ngp/bios.cpp src/mednafen/ngp/biosHLE.cpp src/mednafen/ngp/dma.cpp src/mednafen/ngp/flash.cpp src/mednafen/ngp/gfx.cpp src/mednafen/ngp/gfx_scanline_colour.cpp \
						src/mednafen/ngp/gfx_scanline_mono.cpp src/mednafen/ngp/interrupt.cpp src/mednafen/ngp/mem.cpp src/mednafen/ngp/neopop.cpp src/mednafen/ngp/rom.cpp src/mednafen/ngp/rtc.cpp \
						src/mednafen/ngp/sound.cpp src/mednafen/ngp/T6W28_Apu.cpp src/mednafen/ngp/Z80_interface.cpp src/mednafen/ngp/TLCS-900h/TLCS900h_disassemble.cpp \
						src/mednafen/ngp/TLCS-900h/TLCS900h_disassemble_dst.cpp src/mednafen/ngp/TLCS-900h/TLCS900h_disassemble_extra.cpp src/mednafen/ngp/TLCS-900h/TLCS900h_disassemble_reg.cpp \
						src/mednafen/ngp/TLCS-900h/TLCS900h_disassemble_src.cpp src/mednafen/ngp/TLCS-900h/TLCS900h_interpret.cpp src/mednafen/ngp/TLCS-900h/TLCS900h_interpret_dst.cpp \
						src/mednafen/ngp/TLCS-900h/TLCS900h_interpret_reg.cpp src/mednafen/ngp/TLCS-900h/TLCS900h_interpret_single.cpp src/mednafen/ngp/TLCS-900h/TLCS900h_interpret_src.cpp \
						src/mednafen/ngp/TLCS-900h/TLCS900h_registers.cpp

##Mednafen config
CPPFLAGS			+=	-DMDFNPS3 -DNETWORK -DPSS_STYLE=1 -DHAVE_MKDIR -DMEDNAFEN_VERSION=\"0.9.18\" -DMEDNAFEN_VERSION_NUMERIC=0x918 -DSIZEOF_DOUBLE=8
INCLUDE				:=	. src ../system ../system/$(PLATFORM) src/mednafen/hw_misc src/mednafen/hw_video src/include src/mednafen/hw_cpu src/mednafen/sms src/mednafen/hw_sound src/include/blip src/mednafen/ngp src/mednafen/ngp/TLCS-900h src/mednafen/snes/src/lib
LIBDIRS				:=	../system ../nestopia ../gambatte ../vbam ../pcsxr ../stella ../yabause
LDFLAGS				+=	-lemusys -lnestopia -lgambatte -lpcsxr -lvbam -lstella -lyabause

#LIBDIRS			+=	../lsnes ../desmume
#LDFLAGS			+=	-llsnes -lsnes -ldesmume

##Build info
TARGET				:=	mednafen
TYPE				:=	bin

include $(CURDIR)/../Makefile-$(PLATFORM).base

