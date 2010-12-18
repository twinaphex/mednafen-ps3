#ifndef SYSTEM__FILEENUMERATOR_H
#define SYSTEM__FILEENUMERATOR_H

class							FileEnumerator
{
	public:
		virtual void			ListPath				(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<ListItem*>& aOutput) = 0;
		virtual std::string		ObtainFile				(const std::string& aPath) = 0;
};

class							LocalEnumerator	: public FileEnumerator
{
	public:
		virtual void			ListPath				(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<ListItem*>& aOutput);
		virtual std::string		ObtainFile				(const std::string& aPath);
};

class							FTPEnumerator : public FileEnumerator
{
	public:
		virtual void			ListPath				(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<ListItem*>& aOutput);
		virtual std::string		ObtainFile				(const std::string& aPath);
		
	public:
		static void				SetCredentials			(const std::string& aHost, const std::string& aPort, const std::string& aUserName, const std::string& aPassword);
	
	protected:
		static std::string		Host;
		static std::string		Port;
		static std::string		UserName;
		static std::string		Password;
};

class							Enumerators
{
	public:
		static FileEnumerator&	GetEnumerator			(const std::string& aPath)
		{
			if(aPath.find("ftp:/") == 0)
			{
				return FTP;
			}
			
			if(aPath.find("file:/") == 0)
			{
				return Local;
			}
			
			return Local;
		}
		
		static std::string		CleanPath				(const std::string& aPath)
		{
			if(aPath.find("ftp:/") == 0)
			{
				return aPath.substr(4);
			}

			if(aPath.find("file:/") == 0)
			{
				return aPath.substr(5);
			}
			
			return aPath;
		}
		
		//HACK: These are defined in 'LocalEnumerator.cpp'
		static LocalEnumerator	Local;
		static FTPEnumerator	FTP;		
};

#endif
