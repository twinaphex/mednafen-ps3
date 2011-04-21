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

class								DorkThreads : public ESThreads
{
	public:
		virtual ESThread*			MakeThread			(ThreadFunction aThreadFunction, void* aUserData) {return new DorkThread(aThreadFunction, aUserData);};
		virtual ESMutex*			MakeMutex			() {return new DorkMutex();};
};

