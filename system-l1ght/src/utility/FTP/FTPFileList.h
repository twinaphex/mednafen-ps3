#ifndef SYSTEM__FTPFILELIST_H
#define SYSTEM__FTPFILELIST_H

extern "C"
{
	#include "ftpparse.h"
}

class									FTPFileList : public WinterfaceList
{
	public:
										FTPFileList						(const char* aHeader, const char* aHost, const char* aPort, const char* aPath);
		virtual							~FTPFileList					();

		void							DownloadFile					(const char* aDest);

		const char*						GetChosenFile					();
		
	protected:
		void							MakePassiveConnection			();
		uint32_t						DoCommand						(const char* aCommand, uint32_t aNeededResult = 0, bool aResult = true);

		int								OutSocket;
		int								InSocket;
		char							Buffer[2048];
				
	private:
		char*							Header;
		char*							Host;
		char*							Port;
		char*							Path;
};

#endif
