Mednafen PS3

This is a port of the Mednafen Multi-System emulator to the Sony Play Station 3 using the open source PSL1GHT SDK.
It is currently based on mednafen version 0.9.16-wip

Features:
	Supports following systems:
		Nintendo Entertainment System
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

	Supports following extra emulator cores:
		Nestopia (NES)
		Gambatte (GB/GBC)
		VBA-M (GBA)
		PCSX-Reloaded (PSX, very experimental, currently very low compatibility and very low speed)

	File browser, place your games on a USB drive, or FTP them to the internal drive
	FTP client for loading games
	Load uncompressed or zip or 7zip images
	Load individual files from inside zip or 7zip archives
	
	Save States
	Fast forward	
	Rewind
	IPS Patching
	Screen Shot (.png), Video (.mov), and Audio (.wav) recording
	Experimental netplay support
	2xSAI, hq2x, hq3x plus other scalers

Future:
	Better Settings Menu Layout
	Better user interface
	Improved Two player
	Faster and better GBA support
	Try to work around memory protection to allow PCSX-Reloaded recompiler core to run
	Fix performance issues with recording video

Use:
	Secret Menu:
		Press Start+Select in any menu
		Choose 'Show Log' to display the contents of the global logger.
		Choose 'Get New EBOOT' to choose a new EBOOT.BIN, this will be copied over '/dev_hdd0/game/MDFN90002/USRDIR/EBOOT.BIN'
			Make sure the new EBOOT is valid, and preferably signed
			If you're careful, you can change the mednafen version without jailbreak

	FTP:
		Set Up:
			Press R3 to open the settings menu and select FTP client.
			Set 'host' and 'port' to the address of the FTP server
			Set 'username' and 'password' to the login of the FTP server
			Set 'enable' to ON, then restart mednafen, an FTP option should be available on the first page.
		Caveats:
			FTP is an insecure protocol, any passwords will be sent plaintext across the network
			The stored password is not masked in anyway and will be readable by anyone
			I recommend setting up the server as read only with an anonymous login
			The latest file is stored at /dev_hdd0/game/MDFN90002/USRDIR/temp.ftp
			Attempting to access an unavailable server may delay the PS3 for over minute before the host lookup times out.
			Attempting to load a large file may delay the PS3 while the file is transfered.
			CD games will not work from FTP

	Netplay:
		Netplay uses a client/server model as opposed to a peer-to-peer
		Netplay requires the 'mednafen-server' package from http://sourceforge.net/projects/mednafen/files/

		Set Up:
			Press R3 to open the settings menu and select Netplay.
			Set 'host' and 'port' to the address of the Netplay server
			Set 'username' and 'password' to the login of the Netplay server
			Set 'gameid' to the unique ID for the game (every player wanting to play together should use the same ID)
			Is commands menu chose 'Connect Netplay' to connect to server
			During Netplay press both R2 and R3 to disconnect from the server
		Caveats:
			Attempting to access an unavailable server may delay the PS3 for over minute before the host lookup times out.

	Record Video and Record Audio:
		Choose the option once to start recording, choose it a second time to end recording.
		Enabling recording will slow down emulation, this will be fixed later (hopefully)
		Output directory is the video and wave directories under /dev_hdd0/game/MDFN90002/USRDIR/mednafen/.
		Video format is in a QuickTime .mov container.
		Audio format is in MS .wav format.

	Settings Menu:
		Many settings will not take effect until mednafen is restarted
		Use left and right to change settings value
		Press Cross to use the on screen keyboard to set a value
		Press Triangle to return a setting to it's default value

	IPS Patching:
		Place the patch in the same directory as the game
		Name should be original file name + .ips For example Game.zip=Game.zip.ips Game.nes=Game.nes.ips
		Be careful to assure the ROM is in the right format for the patch. Some formats have headers that can cause patches to not work.

	PCE CD Support:
		Ensure that the correct BIOS file is set in the settings menu
		To load game select .cue file describing disc
		Games can be in 'bin+cue', or iso with Ogg Vorbis audio tracks, MP3 tracks are not supported. (MusePak .mpc files may also be supported, but are not tested, so no guarantees.)
		Please ensure accurate file naming in .cue file, including file path, and file name case.

	PSX Support:
		Ensure that the correct BIOS file is set in the settings menu, running without a BIOS will not work on PS3 (Endian issues).
		To load game select .cue file describing disc.
		CDDA is not supported, CD copy protection is not supported, analog controls are not supported.
		Even the simplest parts of the simplest games won't reach full speed.
		Please ensure accurate file naming in .cue file, including file path, and file name case.

	In Game Buttons:
		R2 for fast forward
		L2 for rewind (if enabled)
		R3 to open commands menu
		
Notes:
	Some systems may require a BIOS image to work, please assign them in the settings menu

Commands:
	In Game press R3 to open the commands menu
	
	Available Commands:
		Change Game: Play a different game
		Reset Game: Reset the game
		Show Text File: Display a text file
		Connect Netplay: Connect to a Netplay server
		Save State: Save the game state
		Load State: Load the game state
		Take Screen Shot: Save a screen shot of the game
		Settings: Open the settings menu
		Configure Controls: Change the controller type and button mapping
		Record Video: Record video frames to disk (currenly slows emulation without affecting output)
		Record Audio: Record audio data to disk (currently slows emulation without affecting output)
		Exit: Return to xmb

Bugs and Issues:
	Gameboy Advance support is spotty, most games will have some frameskip.
	Rewind in Gambatte will crash on certain games.
	Rewind in VBA-M is slow and may cause crashes.
	There is no way to change disk sides in FDS games.
	Some features may be missing from WonderSwan and neo-geo pocket.
	Using netplay may cause a high frameskip.

Thanks to:
	The original author of Mednafen, and all of the emulators it contains.
	The authors of Nestopia, Gambatte, VisualBoyAdvance (and all of it's forks)
	The people who make the PS3 jailbreak possible

