#pragma once

//Type for Fex
class												fex_t;

///Summerface widget for listing and loading files from compressed archives. This can be used transparently on non-archive files which will be
///represented as an archive containing a single file.
class												ArchiveList : public AnchoredListView<SummerfaceItem>
{
	public:
		///Create a new ArchiveList.
		///@param aRegion Area, in percent, of the screen covered by the list.
		///@param aFileName Path to the archive file to process.
													ArchiveList						(const Area& aRegion, const std::string& aFileName); //External

		///Close the archive.
		virtual										~ArchiveList					(); //External

		///Determine if a valid archive is loaded. If this is set it typically means that the file passed to the constructor could not be opened.
		///@return True if a valid archive is loaded.
		bool										IsValid							() const
		{
			return Archive != 0;
		}

		///Get the file size, in bytes, of the selected item.
		///@return The size of the selected file.
		uint32_t									GetSelectedSize					() const; //External

		///Extract the data of the selected item. The required size of aData can be determined by calling GetSelectedSize().
		///@param aSize Size of the passed buffer.
		///@param aData Buffer to store the extracted file.
		///@return True if the operation was successful.
		bool										GetSelectedData					(uint32_t aSize, void* aData) const; //External

		///Get the filename of the selected item in the list.
		///@return The filename of the selected item in the list.
		std::string									GetSelectedFileName				() const
		{
			return GetSelected()->GetText();
		}
	
	public: //Static helper
		///Utility function to determine if a file is an archive.
		///@param Path of the file to check.
		///@return True if the file is an archive, false otherwise.
		static bool									IsArchive						(const std::string& aFileName); //External
		
	private:
		///Utility function to seek the archive to a particular file.
		///@param aFileName Name of the file to seek to.
		///@return False if the file was not found in the archive.
		bool										FindFexFile						(const std::string& aFileName) const; //External

		mutable fex_t*								Archive;						///<Handle to the archive.
};


