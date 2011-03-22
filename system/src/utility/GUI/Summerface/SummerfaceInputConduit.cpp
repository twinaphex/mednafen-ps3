#include <es_system.h>
#include "SummerfaceInputConduit.h"


											SummerfaceStaticConduit::SummerfaceStaticConduit	(bool (*aCallback)(void*, Summerface* aInterface, const std::string&), void* aUserData)
{
	Callback = aCallback;
	UserData = aUserData;
}

bool										SummerfaceStaticConduit::HandleInput				(Summerface* aInterface, const std::string& aWindow)
{
	if(Callback)
	{
		Callback(UserData, aInterface, aWindow);
	}
}

