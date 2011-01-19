#ifndef	SYSTEM__LOGGER_H
#define	SYSTEM__LOGGER_H

class				Logger : public TextViewer
{
	public:
									Logger						(const std::string& aHeader = "Message Log");
									~Logger						();
									
		void						Log							(const char* aString, ...);
};

#endif

