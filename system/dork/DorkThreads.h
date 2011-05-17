#pragma once

class								DorkThread : public ESThread
{
	public:
									DorkThread			(ThreadFunction aThreadFunction, void* aUserData); //External

		virtual						~DorkThread			(); //External
		virtual int32_t				Wait				(); //External

		static void					ThreadWrapper		(uint64_t aUser);

	private:
		ThreadFunction				Function;
		void*						UserData;

		sys_ppu_thread_t			Thread;
		int32_t						Result;
		bool						Dead;
};

class								DorkMutex : public ESMutex
{
	public:
									DorkMutex			(); //External
		virtual						~DorkMutex			(); //External

		void						Lock				(); //External
		void						Unlock				(); //External

	private:
		sys_mutex_t					Mutex;
};

class								DorkSemaphore : public ESSemaphore
{
	public:
									DorkSemaphore		(uint32_t aValue); //External
		virtual						~DorkSemaphore		(); //External

		virtual uint32_t			GetValue			(); //External
		virtual void				Post				(); //External
		virtual void				Wait				(); //External

	private:
		sys_semaphore_t				Semaphore;
};


class								DorkThreads : public ESThreads
{
	public:
		virtual ESThread*			MakeThread			(ThreadFunction aThreadFunction, void* aUserData) {return new DorkThread(aThreadFunction, aUserData);};
		virtual ESMutex*			MakeMutex			() {return new DorkMutex();};
		virtual ESSemaphore*		MakeSemaphore		(uint32_t aValue) {return new DorkSemaphore(aValue);};
};

