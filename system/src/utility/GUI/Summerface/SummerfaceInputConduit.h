#ifndef SYSTEM__SUMMERFACE_INPUTCONDUIT_H
#define SYSTEM__SUMMERFACE_INPUTCONDUIT_H

class													SummerfaceInputConduit
{
	public:
		virtual bool									HandleInput						(Summerface* aInterface, const std::string& aWindow) = 0;
};

class													SummerfaceStaticConduit	: public SummerfaceInputConduit
{
	public:
														SummerfaceStaticConduit			(bool (*aCallback)(void*, Summerface* aInterface, const std::string&), void* aUserData);

		virtual bool									HandleInput						(Summerface* aInterface, const std::string& aWindow);

	protected:
		bool											(*Callback)						(void*, Summerface* aInterface, const std::string&);
		void*											UserData;
};

#endif

