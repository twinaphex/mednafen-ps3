Mednafen PS3

This is a port of the Mednafen Multi-System emulator to the Sony Play Station 3 using the open source PSL1GHT SDK.
It is currently based on mednafen version 0.9.15-wip

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

	File browser, place your games on a USB drive, or FTP them to the internal drive
	Experimental FTP client
	Load uncompressed or zip or 7zip images
	Load individual files for inside zip or 7zip archives
	
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
	Secret Menu:
		Press Start+Select in any menu
		Choose 'Show Log' to display the contents of the global logger.
		Choose 'Get New EBOOT' to choose a new EBOOT.BIN, this will be copied over '/dev_hdd0/game/MDFN90002/USRDIR/EBOOT.BIN'
			Make sure the new EBOOT is valid, and preferably signed
			If you're careful, you can change the mednafen version without jailbreak

	FTP:
		Set Up:
			Press R3 to open the settings menu and press R1 until the ftp.ps3 options appear.
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
	Gameboy Advance support is spotty, some games can't get to full speed
	Some features may be missing from WonderSwan and neo-geo pocket	

Thanks to:
	The original author of Mednafen, and all of the emulators it contains.
	The authors of Nestopia, Gambatte, VisualBoyAdvance (and all of it's forks)
	The people who make the PS3 jailbreak possible
