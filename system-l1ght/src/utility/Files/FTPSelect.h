#ifndef SYSTEM__FTP_H
#define SYSTEM__FTP_H

extern "C"
{
	#include "ftpparse.h"
}

class									FTPSelect
{
	public:
										FTPSelect						(const std::string& aHeader, const std::string& aHost, const std::string& aPort, MenuHook* aInputHook = 0);
										~FTPSelect						();

		void							DownloadFile					(const std::string& aDest);
		
		std::string						GetFileName						();
		std::string						GetFile							();

	protected:
		std::string						Header;
		std::string						Host;
		std::string						Port;
	
		std::stack<FTPFileList*>		Lists;

		MenuHook*						InputHook;
};


#endif
