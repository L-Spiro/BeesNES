/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for textures.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNSurface.h"

namespace lsn {

	/**
	 * Class CTextureBase
	 * \brief The base class for textures.
	 *
	 * Description: The base class for textures.
	 */
	class CTextureBase {
	public :
		// == Functions.
		/**
		 * Resets the object instance back to scratch, ready to be reused.
		 **/
		virtual void											Reset();

		/**
		 * Gets the width of the base texture.
		 * 
		 * \return Returns the width of mip-map 0.
		 **/
		inline uint32_t											Width() const;

		/**
		 * Gets the height of the base texture.
		 * 
		 * \return Returns the height of mip-map 0.
		 **/
		inline uint32_t											Height() const;

		/**
		 * Gets the depth of the base texture.
		 * 
		 * \return Returns the depth of mip-map 0.
		 **/
		inline uint32_t											Depth() const;

		/**
		 * Gets the number of mipmaps.
		 * 
		 * \return Returns the number of mipmaps.
		 **/
		inline size_t											Mipmaps() const;

		/**
		 * Gets the number of array elements.
		 * 
		 * \return Returns the number of array elements.
		 **/
		inline size_t											ArraySize() const;

		/**
		 * Gets the number of faces.
		 * 
		 * \return Returns the number of faces.
		 **/
		inline size_t											Faces() const;

		/**
		 * Allocates surfaces.
		 * 
		 * \param _ui32W Surface width.
		 * \param _ui32H Surface height.
		 * \param _ui32D Surface depth.
		 * \param _sMips Total mipmaps.
		 * \param _sFaces Total faces.
		 * \param _sArray Total array surfaces.
		 * \param _ui32Pool Texture pool.
		 * \param _ui32Usage Texture usage.
		 * \param _ui32Format Texture format.
		 * \return Returns true if the allocation was successful.
		 **/
		bool													Alloc( uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, size_t _sMips, size_t _sFaces, size_t _sArray,
			uint32_t _ui32Pool, uint32_t _ui32Usage, uint32_t _ui32Format );


	protected :
		// == Members.
		std::vector<std::unique_ptr<CSurface>>					m_vMipMaps;									/**< The array of mipmaps.  Index 0 is the base level. */
		size_t													m_sArraySize = 0;							/**< Number of slices in an array.  1 for flat 1D/2D images. */
		size_t													m_sFaces = 0;								/**< 1 for normal textures, 6 for cube textures. */
		uint32_t												m_ui32MemoryPool = 0;						/**< The memory pool. */
		uint32_t												m_ui32Usage = 0;							/**< Texture usage. */
		uint32_t												m_ui32Format = 0;							/**< The texture format. */


		// == Functions.
		CTextureBase();
		virtual ~CTextureBase();
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the width of the base texture.
	 * 
	 * \return Returns the width of mip-map 0.
	 **/
	inline uint32_t CTextureBase::Width() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Width() : 0; }

	/**
	 * Gets the height of the base texture.
	 * 
	 * \return Returns the height of mip-map 0.
	 **/
	inline uint32_t CTextureBase::Height() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Height() : 0; }

	/**
	 * Gets the depth of the base texture.
	 * 
	 * \return Returns the depth of mip-map 0.
	 **/
	inline uint32_t CTextureBase::Depth() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Depth() : 0; }

	/**
	 * Gets the number of mipmaps.
	 * 
	 * \return Returns the number of mipmaps.
	 **/
	inline size_t CTextureBase::Mipmaps() const { return m_vMipMaps.size(); }

	/**
	 * Gets the number of array elements.
	 * 
	 * \return Returns the number of array elements.
	 **/
	inline size_t CTextureBase::ArraySize() const { return m_sArraySize; }

	/**
	 * Gets the number of faces.
	 * 
	 * \return Returns the number of faces.
	 **/
	inline size_t CTextureBase::Faces() const { return m_sFaces; }

}	// namespace lsn
