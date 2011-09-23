#pragma once

class	TextViewer;
extern TextViewer* es_log;

namespace				LibES
{
	///Initialize libES.
	///@param aExitFunction Optional function to be called at exit time, should not return.
	///@param argc The argc value for the application's command line.
	///@param argv The argv value for the application's command line.
	void				Initialize				(void (*aExitFunction)() = 0, int argc = 0, char** argv = 0);

	///Shutdown libES.
	void				Shutdown				();

	///Determine if the specified string was passed as an argument.
	///@param aName Name of the argument to check.
	///@return True if the argument was passed on the command line.
	bool				HasArgument				(const std::string& aName);

	///Determine if the application should exit. Should be called as a condition of loops.
	///@return True if the application should exit.
	volatile bool		WantToDie				();

	///Determine if the application should halt processing. Will return true when the platform's OS
	///has control.
	volatile bool		WantToSleep				();

	///Stop processing. Will call the exit function passed to Initialize, or libc's abort otherwise.
	///@param aMessage Message to display to the user.
	void				Abort					(const std::string& aMessage);

	///Display a message to the user using either a platform specific or a default error handler.
	///@param aMessage Message to display.
	///@param aHeader Brief header for the message, used in GUI windows.
	void				Error					(const std::string& aMessage, const std::string& aHeader = "");

	///Get a string from the user using either a platform specific or a default method.
	///@param aHeader Message displayed to detail the string's purpose.
	///@param aMessage Default string to be placed in the input handler's buffer.
	std::string			GetString				(const std::string& aHeader, const std::string& aMessage);

	///Get a response to a yes/no question from the user using either a platform specific or a default method.
	///@param aMessage Message to display to the user.
	///@param aCancel If not null will be set to true if the user uses a method to cancel the dialog, rather than selecting an
	///answer.
	///@return True if 'yes' was selected.
	bool				Confirm					(const std::string& aMessage, bool* aCancel = 0);

	///Get a number input from the user using either a platform specific or a default method.
	///@param aValue Pointer to the the initial value of the dialog, as well as the output for the selected number.
	///@param aHeader Message to display with the dialog.
	///@param aDigits Number of digits allowed in the returned number.
	///@param aHex True if the input number should be in hex format, false for decimal.
	///@return A value indicating whether the user canceled the dialog.
	bool				GetNumber				(int64_t* aValue, const std::string& aHeader, uint32_t aDigits, bool aHex);

	///Return a system path.
	///@param aPath Portion of the path relative to the system dir.
	///@return The resulting path.
	std::string			BuildPath				(const std::string& aPath);
}

namespace				LibESPlatform
{
	///Initialize libES.
	void				Initialize				();

	///Shutdown libES.
	void				Shutdown				();

	///Determine if the application should exit. Should be called as a condition of loops.
	///@return True if the application should exit.
	volatile bool		WantToDie				();

	///Determine if the application should halt processing. Will return true when the platform's OS
	///has control.
	volatile bool		WantToSleep				();

#ifdef	HAVE_ESSUB_ERROR
	///Display a message to the user using either a platform specific or a default error handler.
	///@param aMessage Message to display.
	///@param aHeader Brief header for the message, used in GUI windows.
	void				Error					(const std::string& aMessage, const std::string& aHeader = "");
#endif

#ifdef HAVE_ESSUB_GETSTRING
	///Get a string from the user using either a platform specific or a default method.
	///@param aHeader Message displayed to detail the string's purpose.
	///@param aMessage Default string to be placed in the input handler's buffer.
	std::string			GetString				(const std::string& aHeader, const std::string& aMessage);
#endif

#ifdef HAVE_ESSUB_CONFIRM
	///Get a response to a yes/no question from the user using either a platform specific or a default method.
	///@param aMessage Message to display to the user.
	///@param aCancel If not null will be set to true if the user uses a method to cancel the dialog, rather than selecting an
	///answer.
	///@return True if 'yes' was selected.
	bool				Confirm					(const std::string& aMessage, bool* aCancel = 0);
#endif

#ifdef HAVE_ESSUB_GETNUMBER
	///Get a number input from the user using either a platform specific or a default method.
	///@param aValue Pointer to the the initial value of the dialog, as well as the output for the selected number.
	///@param aHeader Message to display with the dialog.
	///@param aDigits Number of digits allowed in the returned number.
	///@param aHex True if the input number should be in hex format, false for decimal.
	///@return A value indicating whether the user canceled the dialog.
	bool				GetNumber				(int64_t* aValue, const std::string& aHeader, uint32_t aDigits, bool aHex);
#endif

	///Return a system path.
	///@param aPath Portion of the path relative to the system dir.
	///@return The resulting path.
	std::string			BuildPath				(const std::string& aPath);
}

