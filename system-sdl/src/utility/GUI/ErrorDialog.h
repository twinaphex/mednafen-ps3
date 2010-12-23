#ifndef SYSTEM__ERRORDIALOG_H
#define SYSTEM__ERRORDIALOG_H

class						ErrorDialog : public Winterface
{
	public:
							ErrorDialog								(const std::string& aMessage, const std::string& aHeader = "Error");
							
		virtual bool		Input									();
		virtual bool		DrawLeft								();
							
	protected:
		std::string			Message;
};

#endif