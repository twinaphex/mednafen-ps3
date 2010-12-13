#ifndef SYSTEM__TEXTVIEWER_H
#define SYSTEM__TEXTVIEWER_H

class							TextViewer : public Winterface
{
	public:
								TextViewer					(const std::string& aFileName);
								TextViewer					(const std::string& aText, const std::string& aHeader);
								
		virtual					~TextViewer					();
		
		bool					DrawLeft					();
		bool					Input						();
		
	protected:
		void					LoadStream					(std::istream& aStream);
	
		std::vector<std::string>Lines;
	
		int32_t					Top;
		int32_t					Left;
		uint32_t				LongestLine;
		uint32_t				LinesDrawn;
};

#endif
