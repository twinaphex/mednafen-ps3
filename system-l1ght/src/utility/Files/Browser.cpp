#include <ps3_system.h>

								Browser::Browser							(const std::string& aHeader, const std::string& aHost, const std::string& aPort, const std::string& aUserName, const std::string& aPassword, std::vector<std::string>& aBookMarks, bool aEnableFTP, MenuHook* aInputHook) : WinterfaceList(aHeader, true, true, aInputHook), BookMarks(aBookMarks)
{
	Header = aHeader;
	Hook = aInputHook;
	
	Host = aHost;
	Port = aPort;
	UserName = aUserName;
	Password = aPassword;
	EnableFTP = aEnableFTP;
	
	Items.push_back(new ListItem("Local"));

	if(aEnableFTP)
	{
		Items.push_back(new ListItem("FTP"));
	}
	
	Files = 0;
	FTP = 0;
}
								
								Browser::~Browser							()
{
	delete Files;
	delete FTP;
}

std::string						Browser::SelectedFile						()
{
	return Result;
}

bool							Browser::Input								()
{
	if(!EnableFTP)
	{
		if(!Files)
		{
			Files = new FileSelect(Header, BookMarks, Hook);
		}
		
		Result = Files->GetFile();
		return true;
	}
	else if(PS3Input::ButtonDown(0, PS3_BUTTON_CROSS))
	{
		if(GetSelected()->GetText() == "Local")
		{
			if(!Files)
			{
				Files = new FileSelect(Header, BookMarks, Hook);
			}
			
			Result = Files->GetFile();

			if(!Result.empty())
			{
				return true;
			}
		}
		else if(GetSelected()->GetText() == "FTP")
		{
			if(!FTP)
			{
				FTP = new FTPSelect(Header, Host, Port, UserName, Password, Hook);
			}
			
			Result = FTP->GetFile();

			if(!Result.empty())
			{
				FTP->DownloadFile(Paths.Build("."));
				Result = Paths.Build(FTP->GetFileName());
				return true;
			}
		}
	}
	
	return WinterfaceList::Input();
}
