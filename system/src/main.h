#pragma once

//ES Interfaces with happy macro
#define DEC_AND_DEF(a,b)	class a; extern a* b;
DEC_AND_DEF(PathBuild, es_paths);
#undef DEC_AND_DEF

class	TextViewer;
extern TextViewer* es_log;

//Happy functions
void					InitES					(void (*aExitFunction)() = 0, int argc = 0, char** argv = 0);
void					QuitES					();
bool					ESHasArgument			(const std::string& aName);
volatile bool			WantToDie				();
volatile bool			WantToSleep				();
void __attribute__((__deprecated__))			Abort					(const char* aMessage);
void					ESSUB_Error				(const char* aMessage, const char* aHeader = 0);
std::string				ESSUB_GetString			(const std::string& aHeader, const std::string& aMessage);
bool					ESSUB_Confirm			(const char* aMessage, bool* aCancel = 0);
bool					ESSUB_GetNumber			(int64_t& aValue, const char* aHeader, uint32_t aDigits, bool aHex);


