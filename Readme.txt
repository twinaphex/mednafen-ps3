Mednafen PS3

This is a port of the Mednafen Multi-System emulator to the Sony Play Station 3 using the open source PSL1GHT SDK.
It is currently based on mednafen version 0.9.15-wip

Features:
	Supports following systems:
		Nintendo Enterainment System
		Nintendo Game Boy
		Nintendo Game Boy Color
		Nintendo Game Boy Advance
		Nintendo Virtual Boy
		Atari Lynx
		Sega Master System
		Sega Game Gear
		Sega Mega Drive / Sega Genesis
		NEC PC-Engine (with CD)
		SNK NeoGeo Pocket Color
		Bandai WonderSwan Color

	File browser, place your games on a USB drive, or FTP them to the internal drive
	Load uncompressed or zipped images
	
	Save States
	Fast forward	
	Rewind
	IPS Patching

Future:
	Better Default Controls
	Better Settings Menu Layout
	Better user interface
	Improved Two player
	Other features
	Possibility of netplay
	Faster and better GBA support

Use:
	There are currently no default controls assigned, the first time you use a new system please press R3 and assign them in the settings menu.

	Settings Menu:
		Many settings will not take effect until mednafen is restarted
		Use left and right to change settings value
		Press Cross to use the on screen keyboard to set a value
		Press Triangle to return a setting to it's default value
		Press L1 and R1 to change the currently displayed category of settings
		Press L2 to open a file browser and set the value to the result
		Press R2 to get a key press and set the value to the result

	IPS Patching:
		Place the patch in the same directory as the game
		Name should be original file name + .ips For example Game.zip=Game.zip.ips Game.nes=Game.nes.ips
		Be careful to assure the ROM is in the right format for the patch. Some formats have headers that can cause patches to not work.

	PCE CD Support:
		Ensure that the correct bios file is set in the settings menu
		To load game select .cue file describing disc
		Games can be in 'bin+cue', or iso with Ogg Vorbis audio tracks, MP3 tracks are not supported. (MusePak .mpc files may also be supported, but are not tested, so no guarantees.)
		Please ensure accurate file naming in .cue file, including file path, and file name case.

	In Game Buttons:
		R2 for fast forward
		L2 for rewind (if enabled)
		R3 to open commands menu
		
Notes:
	Some systems may require a bios image to work, please assign them in the settings menu

Commands:
	In Game press R3 to open the commands menu
	
	Available Commands:
		Settings: Open the settings menu
		Reset: Reset the game
		Take Screen Shot: Save a screen shot of the game
		Save State: Save the game state
		Load State: Load the game state
		Enable Rewind: Enable rewind function (press L2 to rewind the game)
		Exit: Return to xmb

Bugs and Issues:
	Loading new games is not supported
	Gameboy Advance support is spotty, some games can't get to full speed
	Some features may be missing from WonderSwan and neo-geo pocket	

Thanks to:
	The original author of Mednafen, and all of the emulators it contains.
	The authors of Nestopia, Gambatte, VirtualBoyAdvance (and all of it's forks)
	The people who make the PS3 jailbreak possible
