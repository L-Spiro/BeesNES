/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 query pool.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanQueryPool.h"

namespace lsn {

	// == Various constructors.
	CVulkanQueryPool::CVulkanQueryPool() {
	}
	CVulkanQueryPool::~CVulkanQueryPool() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanQueryPool::Reset() {
		m_qpQueryPool.Reset();
	}

	/**
	 * Creates a query pool. A query pool contains an array of queries.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pqpciCreateInfo Specifies the query pool in a VkQueryPoolCreateInfo structure.
	 * \return Returns true if the query pool was created.
	 **/
	bool CVulkanQueryPool::CreateQueryPool(
		VkDevice _dDevice,
		const VkQueryPoolCreateInfo * _pqpciCreateInfo ) {
		if ( !_dDevice || !_pqpciCreateInfo ) { return false; }
		Reset();
		return m_qpQueryPool.Create( _dDevice, _pqpciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
