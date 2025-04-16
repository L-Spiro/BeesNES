/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The complete set of elements is stored on disk while only a section (or cache) is kept in RAM.
 *	The disk file path is generated automatically based on the current directory, the string "lvec",
 *	and a unique ID that is incremented for each created instance. The parameter _nMaxRamItems
 *	determines how many elements remain in memory at a given time, and _nTotalSize is the overall
 *	number of elements.
 *
 * Note: Iterator support is explicitly removed to avoid repeated flushing and loading.
 */

#pragma once

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


namespace lsn {

	//------------------------------------------------------------------------------
	// large_vector: A Disk-Backed Vector with Automatic Unique File Path Generation
	//------------------------------------------------------------------------------

	/**
	 * \brief A disk–backed vector that inherits from std::vector and stores most of its data on disk.
	 *
	 * The complete set of elements is stored on disk while only a section (or cache) is kept in RAM.
	 * The disk file path is generated automatically based on the current directory, the string "lvec",
	 * and a unique ID that is incremented for each created instance. The parameter _nMaxRamItems
	 * determines how many elements remain in memory at a given time, and _nTotalSize is the overall
	 * number of elements.
	 *
	 *	Note: Iterator support is explicitly removed to avoid repeated flushing and loading.
	 *
	 * \tparam T The type of elements stored.
	 */
	template <typename T, typename Allocator = std::allocator<T>>
	class large_vector : protected std::vector<T, Allocator> {
	public:
		/**
		 * \brief Constructs a large_vector.
		 *
		 * Generates a unique disk file path based on the current directory, "lvec", and a unique ID.
		 *
		 * \param _nMaxRamItems Maximum number of items to keep in RAM at one time.
		 * \param _nTotalSize Total number of elements that the vector will store.
		 */
		large_vector( size_t _nMaxRamItems, size_t _nTotalSize ) :
			std::vector<T, Allocator>(),
			m_nMaxRamItems( _nMaxRamItems ),
			m_nTotalSize( _nTotalSize ),
			m_nCurrentSectionStart( 0 ),
			m_bDirty( false ) {
			
			{
				// Lock the static mutex for directory management.
				std::lock_guard<std::mutex> lgLock( s_mMutex );
				if ( s_nInstanceCount == 0 ) {
					// Set the base directory to "<current_path>/lvec" and create it.
					s_pBaseDirectory = std::filesystem::current_path() / "lvec";
					std::error_code ecErr;
					if ( !std::filesystem::create_directories( s_pBaseDirectory, ecErr ) && ecErr ) {
						throw std::runtime_error( "Failed to create directories for large_vector file: " + ecErr.message() );
					}
				}
				++s_nInstanceCount;
			}

			// Generate a unique disk file path.
			uint64_t uID = s_uID.fetch_add( 1, std::memory_order_relaxed );
			m_pPathDiskFile = std::filesystem::current_path() / ("lvec" + std::to_string( uID ));

			// Create (or truncate) the file and initialize it with default-initialized elements.
			{
				std::ofstream ofs( m_pPathDiskFile, std::ios::binary | std::ios::trunc );
				if ( !ofs ) { throw std::runtime_error( "Failed to create disk file." ); }
				T tDefVal{};
				for ( size_t i = 0; i < m_nTotalSize; ++i ) {
					ofs.write( reinterpret_cast<const char *>(&tDefVal), sizeof( T ) );
				}
			}
         
			// Resize the inherited vector to hold the initial section.
			std::vector<T, Allocator>::resize( std::min( m_nMaxRamItems, m_nTotalSize ) );
			// Load the first section from disk.
			loadSection( m_nCurrentSectionStart );
		}

		/**
		 * \brief Destructor that deletes the on-disk file.
		 *
		 * Any errors during file deletion are caught and ignored so that no exceptions are thrown.
		 */
		~large_vector() {
			try {
				// Optional flush if needed; ignoring any errors.
				try { flushCurrentSection(); } catch ( ... ) { /* Ignore errors during flush. */ }
				std::filesystem::remove( m_pPathDiskFile );
			} catch ( ... ) {
				// Suppress all exceptions in destructor.
			}
			{
				std::lock_guard<std::mutex> lgLock( s_mMutex );
				// Decrement the instance counter.
				--s_nInstanceCount;
				if ( s_nInstanceCount == 0 ) {
					// When the last instance is removed, delete the base directory and reset the unique ID counter.
					std::error_code ecErr;
					std::filesystem::remove_all( s_pBaseDirectory, ecErr ); // Remove the entire "lvec" folder.
					s_uID.store( 0, std::memory_order_relaxed );
				}
			}
		}

		//-------------------------------------------------------------------------
		// Overridden Base–Class Methods (non–iterator related)
		//-------------------------------------------------------------------------

		/**
		 * \brief Returns the total number of elements stored (on disk and in RAM).
		 *
		 * \return The total number of elements.
		 */
		size_t													size() const {
			return m_nTotalSize;
		}
    
		/**
		 * \brief Checks if the container is empty.
		 *
		 * \return True if there are no elements; false otherwise.
		 */
		bool													empty() const {
			return (m_nTotalSize == 0);
		}
    
		/**
		 * \brief Returns a pointer to the currently cached RAM section.
		 *
		 * \return Pointer to the data of the current section.
		 */
		T *														data() {
			return std::vector<T, Allocator>::data();
		}
    
		/**
		 * \brief Returns a const pointer to the currently cached RAM section.
		 *
		 * \return Const pointer to the current section's data.
		 */
		const T *												data() const {
			return std::vector<T, Allocator>::data();
		}
    
		/**
		 * \brief Provides bounds–checked access to an element.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Reference to the element.
		 * \throws std::out_of_range if _nIndex is outside [0, size()).
		 */
		T &														at( size_t _nIndex ) {
			if ( _nIndex >= m_nTotalSize ) { throw std::out_of_range( "Index out of range in at()." ); }
			return (*this)[_nIndex];
		}
    
		/**
		 * \brief Provides bounds–checked, read–only access to an element.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Const reference to the element.
		 * \throws std::out_of_range if _nIndex is outside [0, size()).
		 */
		const T &												at( size_t _nIndex ) const {
			if ( _nIndex >= m_nTotalSize ) { throw std::out_of_range( "Index out of range in at() const." ); }
			return const_cast<large_vector *>(this)->operator[](_nIndex);
		}
    
		//-------------------------------------------------------------------------
		// Removed Support for front() and back()
		//-------------------------------------------------------------------------
    
		T &														front() = delete;
		const T &												front() const = delete;
		T &														back() = delete;
		const T &												back() const = delete;
    
		//-------------------------------------------------------------------------
		// Overloaded Operator[] with Disk–Backed Caching Logic
		//-------------------------------------------------------------------------

		/**
		 * \brief Provides non–const access to an element using the [] operator.
		 *
		 * If the requested element (global index _nIndex) is not in the currently cached section,
		 * this function flushes the dirty section (if any) to disk and loads the required section.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Reference to the element.
		 * \throws std::out_of_range if _nIndex is not valid.
		 * \throws std::runtime_error if file I/O fails.
		 */
		T &														operator [] ( size_t _nIndex ) {
			if ( _nIndex >= m_nTotalSize ) { throw std::out_of_range( "Index out of range in operator[]." ); }
			// If _nIndex is outside the currently loaded section, flush and load the appropriate section.
			if ( _nIndex < m_nCurrentSectionStart ||
				_nIndex >= m_nCurrentSectionStart + std::vector<T, Allocator>::size() ) {
				flushCurrentSection();
				size_t nNewSectionStart = (_nIndex / m_nMaxRamItems) * m_nMaxRamItems;
				loadSection( nNewSectionStart );
			}
			m_bDirty = true;
			// Adjust _nIndex to be relative to the cached section.
			return std::vector<T, Allocator>::operator[]( _nIndex - m_nCurrentSectionStart );
		}
    
		/**
		 * \brief Provides const access to an element using the [] operator.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Const reference to the element.
		 */
		const T &												operator [] ( size_t _nIndex ) const {
			return const_cast<large_vector *>(this)->operator[](_nIndex);
		}
    
		//-------------------------------------------------------------------------
		// Disable Iterator Support
		//-------------------------------------------------------------------------
    
		// The following iterator functions are explicitly deleted to prevent unintended
		// iteration that would trigger constant flushing/loading of disk sections.
		typename std::vector<T, Allocator>::iterator			begin() = delete;
		typename std::vector<T, Allocator>::iterator			end() = delete;
		typename std::vector<T, Allocator>::const_iterator		begin() const = delete;
		typename std::vector<T, Allocator>::const_iterator		end() const = delete;
		typename std::vector<T, Allocator>::const_iterator		cbegin() const = delete;
		typename std::vector<T, Allocator>::const_iterator		cend() const = delete;

		//-------------------------------------------------------------------------
		// Modification Functions: push_back, pop_back, insert
		//-------------------------------------------------------------------------
    
		/**
		 * \brief Appends an element to the end of the vector.
		 *
		 * Flushes the current in–RAM cache to disk, then appends _tElem at the end of the file.
		 *
		 * \param _tElem The element to append.
		 * \throws std::runtime_error if file I/O fails.
		 */
		void													push_back( const T & _tElem ) {
			flushCurrentSection();
			std::fstream fs( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );
			if ( !fs ) { throw std::runtime_error( "Failed to open file for push_back." ); }
			fs.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
			fs.write( reinterpret_cast<const char *>(&_tElem), sizeof( T ) );
			fs.close();
			++m_nTotalSize;
		}

		/**
		 * \brief Appends an array of elements to the end of the vector.
		 *
		 * Flushes the current in–RAM cache to disk, then appends _nCount elements from _pArray in a single operation.
		 *
		 * \param _pArray Pointer to the first element of the array to append.
		 * \param _nCount Number of elements to append.
		 * \throws std::runtime_error if file I/O fails.
		 */
		void													push_back( const T * _pArray, size_t _nCount ) {
			flushCurrentSection();
			std::fstream fs( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );
			if ( !fs ) { throw std::runtime_error( "Failed to open file for push_back (bulk)." ); }
			fs.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
			fs.write( reinterpret_cast<const char *>(_pArray), _nCount * sizeof( T ) );
			fs.close();
			m_nTotalSize += _nCount;
		}
    
		/**
		 * \brief Removes the last element from the vector.
		 *
		 * Flushes the current cache to disk and then truncates the file to remove the last element.
		 *
		 * \throws std::runtime_error if the vector is empty or file I/O fails.
		 */
		void													pop_back() {
			if ( m_nTotalSize == 0 ) { throw std::runtime_error( "pop_back called on empty vector." ); }
			flushCurrentSection();
			--m_nTotalSize;
			std::filesystem::resize_file( m_pPathDiskFile, m_nTotalSize * sizeof( T ) );
			if ( m_nCurrentSectionStart >= m_nTotalSize ) {
				m_nCurrentSectionStart = (m_nTotalSize > 0) ? (((m_nTotalSize - 1) / m_nMaxRamItems) * m_nMaxRamItems) : 0;
				loadSection( m_nCurrentSectionStart );
			}
		}

		/**
		 * \brief Removes a specified number of elements from the back of the vector.
		 *
		 * Flushes the current in–RAM cache, subtracts _nCount from m_nTotalSize (if valid),
		 * truncates the disk file accordingly, and reloads the current cache if necessary.
		 *
		 * \param _nCount The number of items to pop.
		 * \throws std::runtime_error if _nCount exceeds the current size.
		 */
		void													pop_back( size_t _nCount ) {
			if ( _nCount > m_nTotalSize ) { throw std::runtime_error( "pop_back called with too many items." ); }
			flushCurrentSection();
			m_nTotalSize -= _nCount;
			std::filesystem::resize_file( m_pPathDiskFile, m_nTotalSize * sizeof( T ) );
			if ( m_nCurrentSectionStart >= m_nTotalSize ) {
				m_nCurrentSectionStart = (m_nTotalSize > 0) ? (((m_nTotalSize - 1) / m_nMaxRamItems) * m_nMaxRamItems) : 0;
				loadSection( m_nCurrentSectionStart );
			}
		}
    
		/**
		 * \brief Inserts an element at the specified index.
		 *
		 * Flushes the current cache to disk, then shifts elements in the file to make room for _tElem.
		 * The file is updated and m_nTotalSize increases by one. If the insertion occurs within the
		 * current cached section, the cache is reloaded.
		 *
		 * \param _nIndex The global index at which to insert the element.
		 * \param _tElem The element to insert.
		 * \throws std::out_of_range if _nIndex is greater than the current size.
		 * \throws std::runtime_error if file I/O fails.
		 */
		void													insert( size_t _nIndex, const T & _tElem ) {
			if ( _nIndex > m_nTotalSize ) { throw std::out_of_range( "Insert index out of range." ); }
			flushCurrentSection();
			size_t sNewSize = m_nTotalSize + 1;
			std::fstream fs( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );
			if ( !fs ) { throw std::runtime_error( "Failed to open file for insert." ); }
			
			// Increase file size.
			std::filesystem::resize_file( m_pPathDiskFile, sNewSize * sizeof( T ) );
			
			// Shift elements rightward starting from the end down to _nIndex.
			T tTmp;
			for ( size_t i = m_nTotalSize; i > _nIndex; --i ) {
				fs.seekg( (i - 1) * sizeof( T ), std::ios::beg );
				fs.read( reinterpret_cast<char *>(&tTmp), sizeof( T ) );
				fs.seekp( i * sizeof( T ), std::ios::beg );
				fs.write( reinterpret_cast<const char *>(&tTmp), sizeof( T ) );
			}
			// Write the new element at _nIndex.
			fs.seekp( _nIndex * sizeof( T ), std::ios::beg );
			fs.write( reinterpret_cast<const char *>(&_tElem), sizeof( T ) );
			fs.close();
			m_nTotalSize = sNewSize;
			if ( _nIndex >= m_nCurrentSectionStart &&
				_nIndex < m_nCurrentSectionStart + std::vector<T, Allocator>::size() ) {
				loadSection( m_nCurrentSectionStart );
			}
		}
    
		//-------------------------------------------------------------------------
		// Disk I/O: Flushing and Loading Sections
		//-------------------------------------------------------------------------

		/**
		 * \brief Flushes the current in–RAM section to disk.
		 *
		 * If the current section has been modified, its contents are written to the disk file at the proper offset.
		 *
		 * \throws std::runtime_error if the file fails to open for writing.
		 */
		void													flushCurrentSection() {
			if ( m_bDirty ) {
				std::fstream fs( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );
				if ( !fs ) { throw std::runtime_error( "Failed to open file for writing." ); }
				fs.seekp( m_nCurrentSectionStart * sizeof( T ), std::ios::beg );
				fs.write( reinterpret_cast<const char *>(std::vector<T, Allocator>::data()),
					std::vector<T, Allocator>::size() * sizeof( T ) );
				fs.close();
				m_bDirty = false;
			}
		}
    
		/**
		 * \brief Loads a section from disk into the in–RAM cache.
		 *
		 * The cache is populated with the section starting at _nSectionStart. The number of items loaded
		 * is the minimum of m_nMaxRamItems and the remaining elements (m_nTotalSize - _nSectionStart).
		 *
		 * \param _nSectionStart The starting global index of the section to load.
		 * \throws std::runtime_error if the file cannot be opened for reading.
		 */
		void													loadSection( size_t _nSectionStart ) {
			m_nCurrentSectionStart = _nSectionStart;
			size_t nItemsToLoad = std::min( m_nMaxRamItems, m_nTotalSize - m_nCurrentSectionStart	);
			std::vector<T, Allocator>::resize(	nItemsToLoad );
			std::fstream fs( m_pPathDiskFile, std::ios::in | std::ios::binary );
			if ( !fs ) { throw std::runtime_error( "Failed to open file for reading." ); }
			fs.seekg( m_nCurrentSectionStart * sizeof( T ), std::ios::beg );
			fs.read( reinterpret_cast<char *>(std::vector<T, Allocator>::data()), nItemsToLoad * sizeof( T ) );
			fs.close();
			m_bDirty = false;
		}
    
	private:
		std::filesystem::path									m_pPathDiskFile;		/**< Generated disk file path based on the current directory, "lvec", and a unique ID. */
		size_t													m_nMaxRamItems;         /**< Maximum number of items to keep in RAM. */
		size_t													m_nTotalSize;           /**< Total number of elements stored. */
		size_t													m_nCurrentSectionStart; /**< Global index marking the start of the current RAM section. */
		bool													m_bDirty;               /**< True if the current section has been modified. */
    
		/** A static atomic counter for generating unique IDs for each instance. */
		inline static std::atomic<uint64_t>						s_uID{ 0 };

		// Static variables for instance counting and directory management.
		inline static std::atomic<size_t>						s_nInstanceCount{ 0 };
		inline static std::mutex								s_mMutex;
		inline static std::filesystem::path						s_pBaseDirectory;
	};

}	// namespace lsn
