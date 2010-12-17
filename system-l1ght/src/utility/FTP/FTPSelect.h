#ifndef SYSTEM__FTP_H
#define SYSTEM__FTP_H

extern "C"
{
	#include "ftpparse.h"
}

class									FTPSelect
{
	public:
										FTPSelect						(const char* aHeader, const char* aHost, const char* aPort, MenuHook* aInputHook = 0);
										~FTPSelect						();

		void							DownloadFile					(const char* aDest);
		const char*						GetFile							();

	protected:
		std::stack<FTPFileList*>		Lists;

		MenuHook*						InputHook;

	private:
		char*							Header;
		char*							Host;
		char*							Port;
};


#endif
