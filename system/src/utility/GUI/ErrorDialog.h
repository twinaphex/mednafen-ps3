#ifndef SYSTEM__ERRORDIALOG_H
#define SYSTEM__ERRORDIALOG_H

class						ErrorDialog : public Winterface
{
	public:
							ErrorDialog								(const char* aMessage, const std::string& aHeader = "Error");
		virtual				~ErrorDialog							(){};
							
		virtual bool		Input									();
		virtual bool		DrawLeft								();
							
	protected:
		char				Message[1024];
};

#endif

