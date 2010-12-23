#include <ps3_system.h>

void										ImageManager::Purge									()
{
	for(std::map<std::string, Texture*>::iterator i = Images.begin(); i != Images.end(); i ++)
	{
		delete i->second;
	}
	
	Images.clear();
}

void										ImageManager::LoadDirectory							(const std::string& aPath)
{
	std::vector<std::string> items;

	if(Utility::ListDirectory(aPath, items))
	{
		for(int i = 0 ; i != items.size(); i ++)
		{
			if(items[i].find(".png") != std::string::npos)
			{
				LoadImage(items[i].substr(0, items[i].length() - 4), aPath + items[i]);
			}
		}
	}
	else
	{
		printf("ImageManager::LoadDirectory: Path '%s' not found\n", aPath.c_str());
	}
}
	
Texture*									ImageManager::LoadImage								(const std::string& aName, const std::string& aPath)
{
/*	if(Images[aName] == 0)
	{
		PngDatas Png;
		memset(&Png, 0, sizeof(Png));
	
		LoadPNG(&Png, aPath.c_str());
		
		if(Png.bmp_out == 0)
		{
			printf("ImageManager::LoadImage: Could not read '%s'\n", aPath.c_str());
			return 0;
		}
		
		Texture* output = new Texture(Png.width, Png.height);
		
		if(Png.wpitch / 4 != Png.width)
		{
			delete output;
			free(Png.bmp_out);
			printf("ImageManager::LoadImage: Image Bad ? '%s'\n", aPath.c_str());
			return 0;
		}

		memcpy(output->GetPixels(), Png.bmp_out, Png.width * Png.height * 4);
		free(Png.bmp_out);
		
		output->SetFilter(true);
		
		Images[aName] = output;
	}*/
	
	return Images[aName];
}

Texture*									ImageManager::GetImage								(const std::string& aName)
{
	return Images[aName];
}


std::map<std::string, Texture*>				ImageManager::Images;
