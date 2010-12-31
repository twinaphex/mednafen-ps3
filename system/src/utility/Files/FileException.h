#ifndef SYSTEM__FILEEXCEPTION_H
#define SYSTEM__FILEEXCEPTION_H

class					FileException : public std::exception
{
	public:
						FileException			(const std::string& aError)		{Error = aError;}
		virtual			~FileException			()								throw(){};
						
		const char*		what					()								{return Error.c_str();}
	
	protected:
		std::string		Error;
};

#endif

