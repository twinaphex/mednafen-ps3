#ifndef SYSTEM__ARCHIVE_H
#define SYSTEM__ARCHIVE_H

class												ArchiveList : public SummerfaceList
{
	public:
													ArchiveList						(const Area& aRegion, const std::string& aFileName);
		virtual										~ArchiveList					();

		uint32_t									GetSelectedSize					();
		void										GetSelectedData					(uint32_t aSize, void* aData);
		std::string									GetSelectedFileName				();
		
		static bool									IsArchive						(const std::string& aFileName);
		
	protected:
		void										FindFexFile						(const std::string& aFileName);

		fex_t*										Archive;
		std::string									FileName;
};

#endif

