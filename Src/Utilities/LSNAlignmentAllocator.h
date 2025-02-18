/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An aligned allocator for std::vector.
 */


#pragma once

#include <cstdint>
#include <malloc.h>


namespace lsn {

	/**
	 * Class CAlignmentAllocator
	 * \brief An aligned allocator for std::vector.
	 *
	 * Description: An aligned allocator for std::vector.
	 */
	template <typename T, size_t N = 64>
	class CAlignmentAllocator {
	public :
		typedef T                                                   value_type;
		typedef size_t                                              size_type;
		typedef ptrdiff_t                                           difference_type;

		typedef T *                                                 pointer;
		typedef const T *                                           const_pointer;

		typedef T &                                                 reference;
		typedef const T &                                           const_reference;

	public :
		inline                                                      CAlignmentAllocator() throw () {}

		template <typename T2>
		inline                                                      CAlignmentAllocator( const CAlignmentAllocator<T2, N> & ) throw () {}

		inline                                                      ~CAlignmentAllocator() throw () {}


		// == Operators.
		/**
		 * Returns false if and only if storage allocated from *this
		 *  can be deallocated from other, and vice versa.
		 *  Always returns false for stateless allocators.
		 * 
		 * \param _aaOther The object against which to compare.
		 * \return Returns false.
		 **/
		bool                                                        operator != ( const CAlignmentAllocator<T, N> &_aaOther ) const  { return !((*this) == _aaOther); }

		/**
		 * Returns true if and only if storage allocated from *this
		 *  can be deallocated from other, and vice versa.
		 *  Always returns true for stateless allocators.
		 * 
		 * \param _aaOther The object against which to compare.
		 * \return Returns true.
		 **/
		bool                                                        operator == ( const CAlignmentAllocator<T, N> &_aaOther ) const { return true; }


		// == Functions.
		/**
		 * Gets a pointer to the given reference.
		 * 
		 * \param _rR The reference whose pointer is to be obtained.
		 * \return Returns a pointer to the given reference.
		 **/
		inline pointer                                              address( reference _rR ) { return &_rR; }

		/**
		 * Gets a constant pointer to the given reference.
		 * 
		 * \param _rR The reference whose pointer is to be obtained.
		 * \return Returns a constant pointer to the given reference.
		 **/
		inline const_pointer                                        address( const_reference _rR ) const { return &_rR; }

		/**
		 * Performs an aligned allocation of _sN elements.
		 * 
		 * \param _sN The number of elements to allocate.
		 * \return Returns a pointer to the allocated _sN elements or nullptr.
		 **/
		inline pointer                                              allocate( size_type _sN ) { return reinterpret_cast<pointer>(::_aligned_malloc( _sN * sizeof( value_type ), N )); }

		/**
		 * Deallocation of the given pointer.
		 * 
		 * \param _pP The pointer to deallocate.
		 **/
		inline void                                                 deallocate( pointer _pP, size_type ) { ::_aligned_free( _pP ); }

		/**
		 * Constructs an object at the given pointer.
		 * 
		 * \param _pP The address at which to construct the object.
		 * \param _vtErt The constructed value.
		 **/
		inline void                                                 construct( pointer _pP, const value_type &_vtErt ) { new ( _pP ) value_type( _vtErt ); }

		/**
		 * Calls the destructor for the given value at address _pP.
		 * 
		 * \param _pP The address of the item to deconstruct.
		 **/
		inline void                                                 destroy( pointer _pP ) { _pP->~value_type(); }

		/**
		 * Returns the maximum number of items that can fit into the vector.
		 * 
		 * \return * Returns the maximum number of items that can fit into the vector.
		 **/
		inline size_type                                            max_size() const throw () { return size_type( -1 ) / sizeof( value_type ); }

		template <typename T2>
		struct rebind {
			typedef CAlignmentAllocator<T2, N>                      other;
		};

        
	};


}	// namespace lsn
