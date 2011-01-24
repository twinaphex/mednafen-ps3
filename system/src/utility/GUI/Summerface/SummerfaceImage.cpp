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
		Texture* tex = ImageManager::GetImage(Image);

		uint32_t x = 1, y = 1, w = es_video->GetClip().Width - 2, h = es_video->GetClip().Height - 2;
		Utility::CenterAndScale(x, y, w, h, tex->GetWidth(), tex->GetHeight());

		es_video->PlaceTexture(tex, x, y, w, h, 0xFFFFFFFF);
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


