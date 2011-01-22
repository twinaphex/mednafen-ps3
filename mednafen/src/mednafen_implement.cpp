#include <mednafen_includes.h>

//How to implement these?
static bool	BlockExit = false;
void		MDFNDES_BlockExit		(bool aExit)
{
	BlockExit = aExit;
}

bool		MDFND_ExitBlockingLoop	()									{return BlockExit;}
void		MDFND_MidSync			(const EmulateSpecStruct *espec)	{}

//Netplay, some other time
ESSocket*	slocket;
void		MDFND_NetStart			()
{
	if(!slocket)
	{
		slocket = es_network->OpenSocket(MDFN_GetSettingS("net.es.host").c_str(), MDFN_GetSettingUI("net.es.port"));
		MDFNI_NetplayStart(1, 1, MDFN_GetSettingS("net.es.username"), MDFN_GetSettingS("net.es.gameid"), MDFN_GetSettingS("net.es.password"));
	}
	else
	{
		delete slocket;
		slocket = 0;

		MDFNI_NetplayStop();
	}
}


int			MDFND_SendData			(const void *data, uint32_t len)
{
	slocket->Write(data, len);
	return 1;
}

int			MDFND_RecvData			(void *data, uint32_t len)
{
	slocket->Read(data, len);
	return 1;
}

void		MDFND_NetplayText		(const uint8_t* text, bool NetEcho)	{if(text){MednafenEmu::DisplayMessage((char*)text);}}
void		MDFND_NetworkClose		()									{}

//Timing and messages
uint32_t	MDFND_GetTime			()									{return (uint32_t)Utility::GetTicks();}
void		MDFND_Sleep				(uint32 ms)							{Utility::Sleep(ms);}
void		MDFND_PrintError		(const char *s)						{es_log->Log("ERROR: %s", s);}
void		MDFND_Message			(const char *s)						{es_log->Log(" INFO: %s", s);}
void		MDFND_DispMessage		(UTF8 *text)						{if(text){MednafenEmu::DisplayMessage((char*)text);}}

//Save states
//TODO: Put this somewhere
StateStatusStruct*	States[10];
void		MDFND_SetStateStatus	(StateStatusStruct *status)				
{
	if(!status)
	{
		return;
	}

	if(States[status->current])
	{
		if(States[status->current]->gfx)
		{
			free(States[status->current]->gfx);
		}
		free(States[status->current]);
	}

	States[status->current] = status;
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

#ifdef L1GHT
void					ThreadWrap	(uint64_t aData)
{
	uint64_t* threaddata = (uint64_t*)aData;

	uint32_t result = ((int (*)(void*))threaddata[0])((void*)threaddata[1]);
	sys_ppu_thread_exit(result);
}
#endif

MDFN_Thread*MDFND_CreateThread		(int (*fn)(void *), void *data)
{
	MDFN_Thread* thread = (MDFN_Thread*)malloc(sizeof(MDFN_Thread));

#ifdef L1GHT
	uint64_t threaddata[2] = {(uint64_t)fn, (uint64_t)data};
	sys_ppu_thread_create((sys_ppu_thread_t*)&thread->data, ThreadWrap, (uint64_t)threaddata, 1001, 0x10000, THREAD_JOINABLE, 0);
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

	if(status)
	{
		*status = out;
	}
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

