#include <ps3_system.h>
#include "SummerfaceImage.h"

											SummerfaceImage::SummerfaceImage					(const Area& aRegion, const std::string& aAsset) : SummerfaceWindow(aRegion)
{
	Image = aAsset;
}

											SummerfaceImage::~SummerfaceImage					()
{

}

bool										SummerfaceImage::Draw								()
{
	if(ImageManager::GetImage(Image))
	{
		es_video->PlaceTexture(ImageManager::GetImage(Image), 1, 1, es_video->GetClip().Width - 2, es_video->GetClip().Height - 2, 0xFFFFFFFF);
	}

	return false;
}

std::string									SummerfaceImage::GetImage							()
{
	return Image;
}

void										SummerfaceImage::SetImage							(const std::string& aAsset)
{
	Image = aAsset;
}


