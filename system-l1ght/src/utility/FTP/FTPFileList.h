#ifndef SYSTEM__FTPFILELIST_H
#define SYSTEM__FTPFILELIST_H

extern "C"
{
	#include "ftpparse.h"
}

class									FTPFileList : public WinterfaceList
{
	public:
										FTPFileList						(const std::string& aHeader, const std::string& aHost, const std::string& aPort, const std::string& aPath);
		virtual							~FTPFileList					();

		void							DownloadFile					(const std::string& aDest);

		std::string						GetChosenFile					();
		
	protected:
		void							BuildListItem					(const std::string& aName);
	
		void							MakePassiveConnection			();
		uint32_t						DoCommand						(const std::string& aCommand, uint32_t aNeededResult = 0, bool aResult = true);

		int								OutSocket;
		int								InSocket;
		char							Buffer[2048];
				
		std::string						Header;
		std::string						Host;
		std::string						Port;
		std::string						Path;
};

#endif
