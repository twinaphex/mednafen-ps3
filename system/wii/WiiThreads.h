#pragma once

class								WiiThread : public ESThread
{
	public:
									WiiThread			(ThreadFunction aThreadFunction, void* aUserData); //External

		virtual						~WiiThread			(); //External
		virtual int32_t				Wait				(); //External

	private:
		static void*				ThreadWrapper		(void* aUserData);

		ThreadFunction				Function;
		void*						UserData;

		lwp_t						Thread;
		int32_t						Result;
};

class								WiiMutex : public ESMutex
{
	public:
									WiiMutex			(); //External
		virtual						~WiiMutex			(); //External

		void						Lock				(); //External
		void						Unlock				(); //External

	private:
		mutex_t						Mutex;
};

class								WiiThreads : public ESThreads
{
	public:
		virtual ESThread*			MakeThread			(ThreadFunction aThreadFunction, void* aUserData) {return new WiiThread(aThreadFunction, aUserData);}
		virtual ESMutex*			MakeMutex			() {return new WiiMutex();}
};

