#include <ps3_system.h>
#include "SummerfaceItem.h"


											SummerfaceItem::SummerfaceItem						(const std::string& aText, const std::string& aImage)
{
	SetText(aText);
	SetImage(aImage);
}

											SummerfaceItem::~SummerfaceItem						()
{

}

void										SummerfaceItem::SetText								(const std::string& aText)
{
	Text = aText;
}

void										SummerfaceItem::SetImage							(const std::string& aImage)
{
	Image = aImage;
}

std::string									SummerfaceItem::GetText								()
{
	return Text;
}

std::string									SummerfaceItem::GetImage							()
{
	return Image;
}

