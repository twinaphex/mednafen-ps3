#include <mednafen_includes.h>

uint32_t	MDFND_GetTime			()
{
	return (uint32_t)Utility::GetTicks();
}

void		MDFND_Sleep				(uint32 ms)
{
	Utility::Sleep(ms);
}

void		MDFND_PrintError		(const char *s)
{
	ps3_log->Log("ERROR: %s", s);
}

void		MDFND_Message			(const char *s)
{
	ps3_log->Log(" INFO: %s", s);
}

bool		MDFND_ExitBlockingLoop	()
{
	return true;
}

int			MDFND_SendData			(const void *data, uint32_t len)		
{
	return 0;
}

int			MDFND_RecvData			(void *data, uint32_t len)				
{
	return 0;
}

void		MDFND_NetplayText		(const uint8_t* text, bool NetEcho)		
{
}

void		MDFND_NetworkClose		()										
{
}

void		MDFND_SetStateStatus	(StateStatusStruct *status)				
{
}

void		MDFND_SetMovieStatus	(StateStatusStruct *status)				
{
}

void		MDFND_DispMessage		(UTF8 *text)
{
	if(text)
	{
		MednafenEmu::DisplayMessage((char*)text);
	}
}

MDFN_Thread*MDFND_CreateThread		(int (*fn)(void *), void *data)
{
	MDFN_Thread* thread = (MDFN_Thread*)malloc(sizeof(MDFN_Thread));
	sys_ppu_thread_create((sys_ppu_thread_t*)&thread->data, (void(*)(uint64_t))fn, (uint64_t)data, 1001, 0x10000, THREAD_JOINABLE, 0);
	return thread;
}

void		MDFND_WaitThread		(MDFN_Thread *thread, int *status)
{
	uint64_t out;
	sys_ppu_thread_join((sys_ppu_thread_t)thread->data, &out);
	free(thread);
}

void		MDFND_KillThread		(MDFN_Thread *thread)
{
	//TODO: Not used yet apparently, good because it isn't supported on PS3?
}

MDFN_Mutex*	MDFND_CreateMutex		(void)
{
	MDFN_Mutex* mutex = (MDFN_Mutex*)malloc(sizeof(MDFN_Mutex));
	sys_lwmutex_attribute_t MutexAttrs;
	memset(&MutexAttrs, 0, sizeof(sys_lwmutex_attribute_t));
	MutexAttrs.attr_protocol = 0;
	MutexAttrs.attr_recursive = LWMUTEX_ATTR_RECURSIVE;
	
	sys_lwmutex_create((sys_lwmutex_t*)&mutex->data, &MutexAttrs);
	return mutex;
}

void		MDFND_DestroyMutex		(MDFN_Mutex *mutex)
{
	sys_lwmutex_destroy((sys_lwmutex_t*)mutex->data);
	free(mutex);
}

int			MDFND_LockMutex			(MDFN_Mutex *mutex)
{
	sys_lwmutex_lock((sys_lwmutex_t*)mutex->data, 0);
	return 0;
}

int			MDFND_UnlockMutex		(MDFN_Mutex *mutex)
{
	sys_lwmutex_unlock((sys_lwmutex_t*)mutex->data);
	return 0;
}
