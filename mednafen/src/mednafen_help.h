#pragma once
#include "ers.h"

DEFINE_PTR_TYPE(MDFN_Surface);
DEFINE_PTR_TYPE(Texture);
DEFINE_PTR_TYPE(InputHandler);
DEFINE_PTR_TYPE(Filter);
DEFINE_PTR_TYPE(TextFileViewer);

class												FastCounter;

class	MednafenEmu
{
	typedef SummerfaceItemUser<std::string>			CommandItem;
	typedef smartptr::shared_ptr<CommandItem>		CommandItem_Ptr;
	typedef GridListView<CommandItem>				CommandList;

	public:
		///Initialize the mednafen core and perform all one time startup functions. Must be called before any other function.
		static void						Init				();

		///Close any running game, shut down the mednafen core and delete all objects created by Init(). Must be called at shutdown. It is not
		///possible to call Init again after this function.
		static void						Quit				();

		///Load a new game into mednafen. If aData is non zero MednafenEmu will call free on it after it is done with it.
		///Does nothing if a game is already loaded, Will call CloseGame if needed.
		///@param aFileName Name of the file to load. If aData and aSize are non-zero this is only informational.
		///@param aData Preloaded data for the ROM. If set to 0 the file from aFileName is loaded from disk.
		///@param aSize Size of preloaded ROM data.
		static bool						LoadGame			(std::string aFileName, void* aData = 0, int aSize = 0);

		///Close any running game. Does nothing if no game is loaded.
		static void						CloseGame			();
	
		///Run the emulator for one frame. It is an error to call this function before a game is loaded.
		///@return True if the screen needs to be updated. False if the emulator skipped drawing the frame.
		static bool						Frame				();

		static void						Sync				(const EmulateSpecStruct* aSpec, bool aInputs);

		///Present an image to the screen. By default this will use the internal structures from the emulator,
		///but specific data can be provided (used by the SaveState menu to show previews). It is an error to
		///call this function before a game is loaded.
		///@param aPixels Pointer to a 32-bit RGBA image to draw.
		///@param aWidth Width of the image.
		///@param aHeight Height of the image.
		///@param aPitch Pitch of the image in pixels (not bytes).
		static void						Blit				(uint32_t* aPixels = 0, uint32_t aWidth = 0, uint32_t aHeight = 0, uint32_t aPitch = 0);

		static void						DoCommands			();

		///Tell the emulator to perform a command. It is an error to call this function before a game is loaded.
		static int						DoCommand			(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton = 0xFFFFFFFF);

		///Reread list of settings, used by the driver, from the mednafen core.
		///@param aOnLoad Set to force certain settings to update. Used when loading a new game to prevent stale settings from
		///carrying over.
		static void						ReadSettings		(bool aOnLoad = false);

	public:		//Inlines
		///Return a value indication whether a game has been loaded.
		///@return True if a game is currently loaded into the emulator.
		static bool						IsGameLoaded		() {return IsInitialized && IsLoaded;}

		///Return a value indication whether Init() has been called.
		///@return True if the emulator is initialized. 
		static bool						IsEmuInitialized	() {return IsInitialized;}

		///Set a message to be displayed in the corner of the screen for a brief period.
		///@param aMessage Message to display.
		static void						DisplayMessage		(std::string aMessage) {Message = aMessage; MessageTime = MDFND_GetTime();}
		
		///Tell the emulator core to redraw the last emulated frame. Used to keep the game image in the background while
		///the GUI is active. It is OK to call this function before a game is loaded.
		///@return True if a frame was actually drawn, false otherwise (e.g. No game was loaded)
		static bool						DummyFrame			() {if(IsGameLoaded() && !SuspendDraw){Blit(0, 0, 0, 0); return true;} return false;}

	protected:	//Internals
		static void						GenerateSettings	(std::vector<MDFNSetting>& aSettings);

	protected:
		static bool						IsInitialized;		///<Set by Init, cleared by Quit.
		static bool						IsLoaded;			///<Set by LoadGame, cleared by CloseGame.

		static Texture_Ptr				Buffer;				///<ESTexture used for presenting the frame.
		static MDFN_Surface_Ptr			Surface;			///<Mednafen Surface object used to receive video from the emulator.
		static bool						SuspendDraw;		///<Used to disable the DummyFrame function during the save state menu.

		static MDFNGI*					GameInfo;			///<Mednafen structure describing the loaded emulator module.
		static InputHandler_Ptr			Inputs;				///<The InputHandler object that translates button presses for the mednafen core.
		static FastCounter				Counter;			///<A FastCounter object used to handle fast forward and FPS counting.
		static EmuRealSyncher			Syncher;			///<A structure used to sync emulator time with real time. (Used during netplay only).
	
		static std::string				Message;			///<A message to display in the corner of the screen.
		static uint32_t					MessageTime;		///<Time, in milliseconds, which the message was shown.
		static bool						RecordingVideo;		///<Set to indicate the video recorder is active.
		static bool						RecordingWave;		///<Set to indicate the audio recorder is active.

		static std::vector<MDFNSetting>	Settings;			///<A list of all settings passed to the mednafen core at init.

		static EmulateSpecStruct		EmulatorSpec;		///<A mednafen structure used to pass data between the core and driver.
		static MDFN_Rect				VideoWidths[512];	///<An array describing the width of every line on the emulated display.
		static int16					Samples[48000];		///<A buffer holding audio data for the emulated frame.
		static int16_t					SamplesUp[48000];	///<A buffer used to convert mono audio data to stereo.
		static bool						SkipNext;			///<Set to true to skip the next frame. Set if the amount of buffered audio gets too low.
		static uint32_t					SkipCount;			///<Number of consecutively skipped frames. Used to prevent too many skipped frames from occurring at once.

		//Text viewer
		static TextFileViewer_Ptr		TextFile;			///<The opened text file.

		//Setting cache
		static bool						RewindSetting;		///<Determine if the State rewinder should be actively recording frames.
		static bool						DisplayFPSSetting;	///<Determine if the Frames per second should be displayed in the corner of the screen.
		static int32_t					AspectSetting;		///<Determine the aspect correction mode to use.
		static int32_t					UnderscanSetting;	///<Amount, in percent, that the screen should be scaled to account for display differences.
		static std::string				ShaderSetting;		///<Name of CG shader file used for presenting the screen.
		static Area						UndertuneSetting;	///<Amount, in percent, that the screen should be scaled to account for display differences.
		static bool						VsyncSetting;		///<Determine if screen flips should be synchronized to the displays refresh rate.
		static std::string				BorderSetting;		///<Path to a png file made available for use by the CG shader.
};


