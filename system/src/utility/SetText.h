#pragma once

//A Hacky reimplementation of gettext

#if ENABLE_NLS

#ifdef __cplusplus
extern "C" {
#endif 

void				SETTEXT_CleanUp					(); //Clear the message cache and free any memory, after calling GetText will return it's argument without translation.
void				SETTEXT_SetMessageFile			(const char* aMessages);
char*				SETTEXT_GetText					(const char* aString);	//The char* should be const char*, the same hack exists in gnu gettext

#ifdef __cplusplus
}
#endif 

#endif

