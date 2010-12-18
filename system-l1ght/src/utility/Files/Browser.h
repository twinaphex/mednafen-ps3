#ifndef SYSTEM__BROWSER_H
#define SYSTEM__BROWSER_H

class									Browser : public WinterfaceList
{
	public:
										Browser								(const std::string& aHeader, const std::string& aHost, const std::string& aPort, const std::string& aUserName, const std::string& aPassword, std::vector<std::string>& aBookMarks, bool aEnableFTP, MenuHook* aInputHook = 0);
		virtual							~Browser							();

		std::string						SelectedFile						();

		bool							Input								();

	protected:
		FileSelect*						Files;
		FileSelect*						FTP;
		
		std::string						Header;
		MenuHook*						Hook;
		
		bool							EnableFTP;
		
		std::string						Result;
		
		std::string						Host;
		std::string						Port;		
		std::string						UserName;
		std::string						Password;		
		
		std::vector<std::string>&		BookMarks;
};

#endif
