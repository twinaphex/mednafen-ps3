#ifndef SYSTEM__IMAGEMANAGER_H
#define SYSTEM__IMAGEMANAGER_H

class													ImageManager
{
	public:
		static void										Purge							();
		static void										LoadDirectory					(const std::string& aPath);
		static void										CreateScratch					();

		static Texture*									LoadImage						(const std::string& aName, const std::string& aPath);
		static Texture*									GetImage						(const std::string& aName);

	protected:
		static std::map<std::string, Texture*>			Images;
};

#endif

