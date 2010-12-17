#include <ps3_system.h>


								FTPSelect::FTPSelect					(const char* aHeader, const char* aHost, const char* aPort, MenuHook* aInputHook)
{
	Header = strdup(aHeader);
	Host = strdup(aHost);
	Port = strdup(aPort);

	InputHook = aInputHook;
	
	Lists.push(new FTPFileList(Header, Host, Port, "/"));
}

								FTPSelect::~FTPSelect					()
{
	while(Lists.size() != 0)
	{
		delete Lists.top();
		Lists.pop();
	}
	
	free(Host);
	free(Port);
	free(Header);
}

void							FTPSelect::DownloadFile					(const char* aDest)
{
	Lists.top()->DownloadFile(aDest);
}

const char*						FTPSelect::GetFile						()
{
	while(!WantToDie())
	{
		Lists.top()->Do();
		
		if(Lists.top()->GetChosenFile() == 0 && Lists.size() > 1)
		{
			delete Lists.top();
			Lists.pop();
		}
		else if(Lists.top()->GetChosenFile() == 0)
		{
			return 0;
		}
		else if(((FTPItem*)Lists.top()->GetSelected())->IsDirectory())
		{
			Lists.push(new FTPFileList(Header, Host, Port, Lists.top()->GetChosenFile()));
		}
		else
		{
			return Lists.top()->GetChosenFile();
		}
	}

	return 0;
}

								
						

