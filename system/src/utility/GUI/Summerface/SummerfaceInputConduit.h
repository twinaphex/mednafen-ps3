#ifndef SYSTEM__SUMMERFACE_INPUTCONDUIT_H
#define SYSTEM__SUMMERFACE_INPUTCONDUIT_H

class													SummerfaceInputConduit
{
	public:
		virtual int										HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow) = 0;
};

class													SummerfaceStaticConduit	: public SummerfaceInputConduit
{
	public:
														SummerfaceStaticConduit			(int (*aCallback)(void*, Summerface_Ptr aInterface, const std::string&), void* aUserData);

		virtual int										HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow);

	protected:
		int												(*Callback)						(void*, Summerface_Ptr aInterface, const std::string&);
		void*											UserData;
};

//Happy hacky conduit
template<class T>
class													SummerfaceTemplateConduit : public SummerfaceInputConduit
{
	public:
														SummerfaceTemplateConduit		(T* aObject) : Data(aObject) {};

		virtual int										HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow)
		{
			return Data->HandleInput(aInterface, aWindow);
		}

	protected:
		T*												Data;
};


#endif

