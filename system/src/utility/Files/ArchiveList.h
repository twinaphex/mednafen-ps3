#ifndef SYSTEM__ARCHIVE_H
#define SYSTEM__ARCHIVE_H

class												ArchiveList : public SummerfaceList
{
	public:
													ArchiveList						(const Area& aRegion, const std::string& aFileName); //External
		virtual										~ArchiveList					(); //External

		uint32_t									GetSelectedSize					() const; //External
		void										GetSelectedData					(uint32_t aSize, void* aData) const; //External
		std::string									GetSelectedFileName				() const {return GetSelected()->GetText();}
	
	public: //Static helper
		static bool									IsArchive						(const std::string& aFileName); //External
		
	protected:
		void										FindFexFile						(const std::string& aFileName) const; //External

		std::string									FileName;
		mutable fex_t*								Archive;
};

#endif

