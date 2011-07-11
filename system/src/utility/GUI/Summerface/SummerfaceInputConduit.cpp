#include <es_system.h>
#include "SummerfaceInputConduit.h"


											SummerfaceStaticConduit::SummerfaceStaticConduit	(int (*aCallback)(void*, Summerface_Ptr aInterface, const std::string&), void* aUserData)
{
	Callback = aCallback;
	UserData = aUserData;
}

int											SummerfaceStaticConduit::HandleInput				(Summerface_Ptr aInterface, const std::string& aWindow)
{
	if(Callback)
	{
		return Callback(UserData, aInterface, aWindow);
	}

	return 0;
}

