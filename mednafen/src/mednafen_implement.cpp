#include <mednafen_includes.h>

static int		MakeSocket	(const char* aIP, const char* aPort, const char* aPort2 = 0)
{
    int portno = atoi(aPort);
    if(aPort2 != 0)
    {
    	portno = portno * 256 + atoi(aPort2);
    }
    
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent* server = gethostbyname(aIP);

    struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_port = htons(portno);	
	serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    
	connect(socketFD, (sockaddr*)&serv_addr, sizeof(serv_addr));
	return socketFD;
}

static int		slocket;

void		MDFND_NetStart			()
{
#if 0
	netInitialize();
	slocket = MakeSocket("192.168.0.250", "4096");
	MDFNI_NetplayStart(1, 1, "BillyBob", "doing", "sexybeef");
#endif
}

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
#if 0
	printf("SEND DATA\n");

	write(slocket, data, len);
	return 1;
#endif
	return 0;
}

int			MDFND_RecvData			(void *data, uint32_t len)				
{
#if 0
	printf("RECV DATA\n");

	read(slocket, data, len);
	return 1;
#endif
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
#ifdef L1GHT
	MDFN_Thread* thread = (MDFN_Thread*)malloc(sizeof(MDFN_Thread));
	sys_ppu_thread_create((sys_ppu_thread_t*)&thread->data, (void(*)(uint64_t))fn, (uint64_t)data, 1001, 0x10000, THREAD_JOINABLE, 0);
	return thread;
#else
	return 0;
#endif
}

void		MDFND_WaitThread		(MDFN_Thread *thread, int *status)
{
#ifdef L1GHT
	uint64_t out;
	sys_ppu_thread_join((sys_ppu_thread_t)thread->data, &out);
	free(thread);
#endif	
}

void		MDFND_KillThread		(MDFN_Thread *thread)
{
	//TODO: Not used yet apparently, good because it isn't supported on PS3?
}

MDFN_Mutex*	MDFND_CreateMutex		(void)
{
#ifdef L1GHT
	MDFN_Mutex* mutex = (MDFN_Mutex*)malloc(sizeof(MDFN_Mutex));
	sys_lwmutex_attribute_t MutexAttrs;
	memset(&MutexAttrs, 0, sizeof(sys_lwmutex_attribute_t));
	MutexAttrs.attr_protocol = 0;
	MutexAttrs.attr_recursive = LWMUTEX_ATTR_RECURSIVE;
	
	sys_lwmutex_create((sys_lwmutex_t*)&mutex->data, &MutexAttrs);
	return mutex;
#else
	return 0;
#endif
}

void		MDFND_DestroyMutex		(MDFN_Mutex *mutex)
{
#ifdef L1GHT
	sys_lwmutex_destroy((sys_lwmutex_t*)mutex->data);
	free(mutex);
#endif
}

int			MDFND_LockMutex			(MDFN_Mutex *mutex)
{
#ifdef L1GHT
	sys_lwmutex_lock((sys_lwmutex_t*)mutex->data, 0);
#endif
	return 0;
}

int			MDFND_UnlockMutex		(MDFN_Mutex *mutex)
{
#ifdef L1GHT
	sys_lwmutex_unlock((sys_lwmutex_t*)mutex->data);
#endif
	return 0;
}
