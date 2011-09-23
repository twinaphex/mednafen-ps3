#pragma once

//TODO: This is only used in ESNetwork, realisitly this whole thing should be deleted and ESNetwork return proper error codes.

class					ESException : public std::exception
{
	public:
		static void		ErrorCheck				(bool aCondition, const char* aMessage, ...)
		{
			if(!aCondition)
			{
				char buffer[2048];

				va_list args;
				va_start (args, aMessage);
				vsnprintf(buffer, 2048, aMessage, args);
				va_end (args);

				throw ESException(buffer);
			}
		}

	public:
						ESException				(const char* aMessage, ...)
		{
			char buffer[2048];

			va_list args;
			va_start (args, aMessage);
			vsnprintf(buffer, 2048, aMessage, args);
			va_end(args);

			Error = buffer;
		}

						ESException				(const std::string& aError)		{Error = aError;}
		virtual			~ESException			()								throw(){};
						
		const char*		what					()								{return Error.c_str();}
	
	protected:
		std::string		Error;
};


