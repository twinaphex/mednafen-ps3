#include <ps3_system.h>

							Logger::Logger						(const std::string& aHeader) : TextViewer("", aHeader)
{
}

							Logger::~Logger						()
{
}
									
void						Logger::Log							(const char* aMessage, ...)
{
	char array[1024];
	va_list args;
	va_start (args, aMessage);
	vsnprintf(array, 1024, aMessage, args);
	va_end(args);
	
	Lines.push_back(std::string(array));
}

