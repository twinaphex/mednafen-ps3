#pragma once

#include "TextViewer.h"

class				Logger : public TextViewer
{
	public:
									Logger						(const Area& aRegion);
									~Logger						();
									
		void						Log							(const char* aString, ...);
};


