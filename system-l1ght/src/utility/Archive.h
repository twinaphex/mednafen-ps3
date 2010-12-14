#ifndef SYSTEM__ARCHIVE_H
#define SYSTEM__ARCHIVE_H

class												ArchiveList : public WinterfaceList
{
	public:
													ArchiveList						(const std::string& aHeader, const std::string& aFileName, MenuHook* aInputHook = 0);
		virtual										~ArchiveList					();

		uint32_t									ItemCount						();

		uint32_t									GetSelectedSize					();
		void										GetSelectedData					(uint32_t aSize, void* aData);
		const std::string&							GetSelectedFileName				();
		
		static bool									IsArchive						(const std::string& aFileName);
		
	protected:
		fex_t*										Archive;
		std::string									FileName;
};

#endif
