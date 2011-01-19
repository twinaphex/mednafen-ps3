#include <ps3_system.h>
#include "SummerfaceLabel.h"

											SummerfaceLabel::SummerfaceLabel					(const Area& aRegion, const std::string& aMessage) : SummerfaceWindow(aRegion)
{
	SetMessage(aMessage);
}

											SummerfaceLabel::~SummerfaceLabel					()
{

}

bool										SummerfaceLabel::Draw								()
{
	FontManager::GetBigFont()->PutString(GetMessage().c_str(), 2, 2, Colors::Normal);	
	return false;
}

std::string									SummerfaceLabel::GetMessage							()
{
	return Message;
}

void										SummerfaceLabel::SetMessage							(const std::string& aMessage)
{
	Message = aMessage;
}

void										SummerfaceLabel::AppendMessage						(const std::string& aMessage)
{
	SetMessage(GetMessage() + aMessage);
}

