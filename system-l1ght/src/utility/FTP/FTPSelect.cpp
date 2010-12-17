#include <ps3_system.h>


								FTPSelect::FTPSelect					(const std::string& aHeader, const std::string& aHost, const std::string& aPort, MenuHook* aInputHook)
{
	Header = aHeader;
	Host = aHost;
	Port = aPort;

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
}

void							FTPSelect::DownloadFile					(const std::string& aDest)
{
	Lists.top()->DownloadFile(aDest);
}

std::string						FTPSelect::GetFileName					()
{
	return Lists.top()->GetSelected()->GetText();
}

std::string						FTPSelect::GetFile						()
{
	while(!WantToDie())
	{
		Lists.top()->Do();
		
		if(Lists.top()->GetChosenFile().empty() && Lists.size() > 1)
		{
			delete Lists.top();
			Lists.pop();
		}
		else if(Lists.top()->GetChosenFile().empty())
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

								
						

