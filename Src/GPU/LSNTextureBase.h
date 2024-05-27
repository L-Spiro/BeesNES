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
	 * Class CDirectX9LosableResourceManager
	 * \brief The base class for textures.
	 *
	 * Description: The base class for textures.
	 */
	class CTextureBase {
	public :
		CTextureBase();
		virtual ~CTextureBase();


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


	protected :
		// == Members.
		std::vector<std::unique_ptr<CSurface>>					m_vMipMaps;									/**< The array of mipmaps.  Index 0 is the base level. */
		size_t													m_sArraySize = 0;							/**< Number of slices in an array.  1 for flat 1D/2D images. */
		size_t													m_sFaces = 0;								/**< 1 for normal textures, 6 for cube textures. */
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
