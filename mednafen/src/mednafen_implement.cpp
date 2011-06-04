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
bool		NetplayOn = false;
ESSocket*	slocket = 0;
void		MDFND_NetStart			()
{
	MDFND_NetworkClose();

	try
	{
		slocket = ESNetwork::OpenSocket(MDFN_GetSettingS("net.es.host").c_str(), MDFN_GetSettingUI("net.es.port"));
		MDFNI_NetplayStart(1, 1, MDFN_GetSettingS("net.es.username"), MDFN_GetSettingS("net.es.gameid"), MDFN_GetSettingS("net.es.password"));
		NetplayOn = true;
	}
	catch(ESException& except)
	{
		char buffer[2048];
		snprintf(buffer, 2048, "Failed to start netplay: %s", except.what());
		ESSUB_Error(buffer);
		return;
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

void		MDFND_NetworkClose		()
{
	if(NetplayOn)
	{
		MDFND_DispMessage((UTF8*)"Network disconnected");

		NetplayOn = false;
		MDFNI_NetplayStop();
	}

	delete slocket;
	slocket = 0;
}

void		MDFND_NetplayText		(const uint8_t* text, bool NetEcho)	{if(text){MednafenEmu::DisplayMessage((char*)text);}}

//Timing and messages
uint32_t	MDFND_GetTime			()									{return (uint32_t)Utility::GetTicks();}
void		MDFND_Sleep				(uint32 ms)							{Utility::Sleep(ms);}
void		MDFND_PrintError		(const char *s)						{es_log->Log("ERROR: %s", s);}
void		MDFND_Message			(const char *s)						{es_log->Log(" INFO: %s", s);}
void		MDFND_DispMessage		(UTF8 *text)						{if(text){MednafenEmu::DisplayMessage((char*)text);}}

//Save states
//TODO: Put this somewhere
StateStatusStruct*	StateStatusInfo;
void		MDFND_SetStateStatus	(StateStatusStruct *status)				
{
	if(!status)
	{
		return;
	}

	if(StateStatusInfo)
	{
		if(StateStatusInfo->gfx)
		{
			free(StateStatusInfo->gfx);
		}
		free(StateStatusInfo);
	}

	StateStatusInfo = status;
}

void		MDFND_SetMovieStatus	(StateStatusStruct *status)				
{
}

//Threading
struct					MDFN_Thread
{
	ESThread*			Thread;
};

struct					MDFN_Mutex
{
	ESMutex*			Mutex;
};

MDFN_Thread*MDFND_CreateThread		(int (*fn)(void *), void *data)
{
	MDFN_Thread* thread = (MDFN_Thread*)malloc(sizeof(MDFN_Thread));
	thread->Thread = es_threads->MakeThread(fn, data);
	return thread;
}

void		MDFND_WaitThread		(MDFN_Thread *thread, int *status)
{
	int result = thread->Thread->Wait();

	if(status)
	{
		*status = result;
	}

	delete thread->Thread;
	free(thread);
}

void		MDFND_KillThread		(MDFN_Thread *thread)
{
	//TODO: Not used yet apparently, good because it isn't supported on PS3?
}

MDFN_Mutex*	MDFND_CreateMutex		(void)
{
	MDFN_Mutex* mutex = (MDFN_Mutex*)malloc(sizeof(MDFN_Mutex));
	mutex->Mutex = es_threads->MakeMutex();
	return mutex;
}

void		MDFND_DestroyMutex		(MDFN_Mutex *mutex)
{
	delete mutex->Mutex;
	free(mutex);
}

int			MDFND_LockMutex			(MDFN_Mutex *mutex)
{
	mutex->Mutex->Lock();
	return 0;
}

int			MDFND_UnlockMutex		(MDFN_Mutex *mutex)
{
	mutex->Mutex->Unlock();
	return 0;
}

void		MDFND_Rumble			(int s, int l)
{
	ESInput::RumbleOn(s, l);
}

