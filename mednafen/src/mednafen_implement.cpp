#include <mednafen_includes.h>

//How to implement these?
bool		MDFND_ExitBlockingLoop	()									{return true;}
void		MDFND_MidSync			(const EmulateSpecStruct *espec)	{}

//Netplay, some other time
void		MDFND_NetStart			()									{}
int			MDFND_SendData			(const void *data, uint32_t len)	{return 0;}
int			MDFND_RecvData			(void *data, uint32_t len)			{return 0;}
void		MDFND_NetplayText		(const uint8_t* text, bool NetEcho)	{}
void		MDFND_NetworkClose		()									{}

//Timing and messages
uint32_t	MDFND_GetTime			()									{return (uint32_t)Utility::GetTicks();}
void		MDFND_Sleep				(uint32 ms)							{Utility::Sleep(ms);}
void		MDFND_PrintError		(const char *s)						{es_log->Log("ERROR: %s", s);}
void		MDFND_Message			(const char *s)						{es_log->Log(" INFO: %s", s);}
void		MDFND_DispMessage		(UTF8 *text)						{if(text){MednafenEmu::DisplayMessage((char*)text);}}

//Save states
static MednafenStateMenu*	TargetMenu = 0;

void		MDFNDES_SetStateTarget	(MednafenStateMenu* aMenu)
{
	TargetMenu = aMenu;
}

void		MDFND_SetStateStatus	(StateStatusStruct *status)				
{
	if(TargetMenu)
	{
		TargetMenu->SetStateStatus(status);
	}

	if(status)
	{
		if(status->gfx)
		{
			free(status->gfx);
		}
		free(status);
	}
}

void		MDFND_SetMovieStatus	(StateStatusStruct *status)				
{
}

//Threading
struct					MDFN_Thread
{
	void*				data;
};

struct					MDFN_Mutex
{
	void*				data;
};

MDFN_Thread*MDFND_CreateThread		(int (*fn)(void *), void *data)
{
	MDFN_Thread* thread = (MDFN_Thread*)malloc(sizeof(MDFN_Thread));

#ifdef L1GHT
	sys_ppu_thread_create((sys_ppu_thread_t*)&thread->data, (void(*)(uint64_t))fn, (uint64_t)data, 1001, 0x10000, THREAD_JOINABLE, 0);
#elif defined(MDSDL)
	thread->data = (void*)SDL_CreateThread(fn, data);
#endif
	return thread;
}

void		MDFND_WaitThread		(MDFN_Thread *thread, int *status)
{
#ifdef L1GHT
	uint64_t out;
	sys_ppu_thread_join((sys_ppu_thread_t)thread->data, &out);
	*status = out;
#elif defined(MDSDL)
	SDL_WaitThread((SDL_Thread*)thread->data, status);
#endif

	free(thread);
}

void		MDFND_KillThread		(MDFN_Thread *thread)
{
	//TODO: Not used yet apparently, good because it isn't supported on PS3?
}

MDFN_Mutex*	MDFND_CreateMutex		(void)
{
	MDFN_Mutex* mutex = (MDFN_Mutex*)malloc(sizeof(MDFN_Mutex));

#ifdef L1GHT
	sys_lwmutex_attribute_t MutexAttrs;
	memset(&MutexAttrs, 0, sizeof(sys_lwmutex_attribute_t));
	MutexAttrs.attr_protocol = 0;
	MutexAttrs.attr_recursive = LWMUTEX_ATTR_RECURSIVE;

	sys_lwmutex_create((sys_lwmutex_t*)&mutex->data, &MutexAttrs);
#elif defined(MDSDL)
	mutex->data = (void*)SDL_CreateMutex();
#endif

	return mutex;
}

void		MDFND_DestroyMutex		(MDFN_Mutex *mutex)
{
#ifdef L1GHT
	sys_lwmutex_destroy((sys_lwmutex_t*)mutex->data);
#elif defined(MDSDL)
	SDL_DestroyMutex((SDL_mutex*)mutex->data);
#endif

	free(mutex);

}

int			MDFND_LockMutex			(MDFN_Mutex *mutex)
{
#ifdef L1GHT
	sys_lwmutex_lock((sys_lwmutex_t*)mutex->data, 0);
#elif defined(MDSDL)
	SDL_mutexP((SDL_mutex*)mutex->data);
#endif

	return 0;
}

int			MDFND_UnlockMutex		(MDFN_Mutex *mutex)
{
#ifdef L1GHT
	sys_lwmutex_unlock((sys_lwmutex_t*)mutex->data);
#elif defined(MDSDL)
	SDL_mutexV((SDL_mutex*)mutex->data);
#endif
	return 0;
}

