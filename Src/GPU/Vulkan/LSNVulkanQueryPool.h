/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 query pool.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanQueryPool
	 * \brief A Vulkan 1.0 query pool.
	 *
	 * Description: A Vulkan 1.0 query pool.
	 */
	class CVulkanQueryPool {
	public :
		// == Various constructors.
		CVulkanQueryPool();
		~CVulkanQueryPool();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a query pool. A query pool contains an array of queries.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pqpciCreateInfo Specifies the query pool in a VkQueryPoolCreateInfo structure.
		 * \return Returns true if the query pool was created.
		 **/
		bool													CreateQueryPool(
			VkDevice _dDevice,
			const VkQueryPoolCreateInfo * _pqpciCreateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan query pool handle.
		 **/
		inline VkQueryPool										Get() const { return m_qpQueryPool.qpQueryPool; }


	protected :
		// == Members.
		/** The wrapped RAII query pool object. */
		CVulkan::LSN_QUERY_POOL									m_qpQueryPool;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
