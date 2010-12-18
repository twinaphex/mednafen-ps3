#ifndef SYSTEM__BROWSER_H
#define SYSTEM__BROWSER_H

class									Browser : public WinterfaceList
{
	public:
										Browser								(const std::string& aHeader, std::vector<std::string>& aBookMarks, bool aEnableFTP, MenuHook* aInputHook = 0);
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
		
		std::vector<std::string>&		BookMarks;
};

#endif
