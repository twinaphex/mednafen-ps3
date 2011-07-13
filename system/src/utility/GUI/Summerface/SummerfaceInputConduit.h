#ifndef SYSTEM__SUMMERFACE_INPUTCONDUIT_H
#define SYSTEM__SUMMERFACE_INPUTCONDUIT_H

class													SummerfaceInputConduit
{
	public:
		virtual int										HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton) = 0;
};

class													SummerfaceStaticConduit	: public SummerfaceInputConduit
{
	public:
														SummerfaceStaticConduit			(int (*aCallback)(void*, Summerface_Ptr aInterface, const std::string&, uint32_t), void* aUserData) :
			Callback(aCallback),
			UserData(aUserData)
			{

			}

		virtual int										HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton)
		{
			return Callback ? Callback(UserData, aInterface, aWindow, aButton) : 0;
		}

	protected:
		int												(*Callback)						(void*, Summerface_Ptr aInterface, const std::string&, uint32_t);
		void*											UserData;
};

//Happy hacky conduit
template<class T>
class													SummerfaceTemplateConduit : public SummerfaceInputConduit
{
	public:
														SummerfaceTemplateConduit		(T* aObject) : Data(aObject) {};

		virtual int										HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton)
		{
			return Data->HandleInput(aInterface, aWindow, aButton);
		}

	protected:
		T*												Data;
};

#endif

