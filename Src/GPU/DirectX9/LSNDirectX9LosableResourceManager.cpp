/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Manages objects whose DirectX 9.0 resources can be lost (when the device is lost, for example).
 *	Every losable resource registers itself with this class automatically (using a small circular dependency).
 *	This class can then inform those resources when the device is lost and regained, allowing them to re-register
 *	their resources.
 */


#ifdef LSN_DX9

#include "LSNDirectX9LosableResourceManager.h"

namespace lsn {

	// == Members.
	/** List of resources. */
	std::vector<CDirectX9LosableResource *> CDirectX9LosableResourceManager::m_vResources;

	/** Unique resource ID. */
	uint32_t CDirectX9LosableResourceManager::m_ui32ResId = 0;

	/** Thread safety. */
	lsw::CCriticalSection CDirectX9LosableResourceManager::m_csCrit;

	// == Functions.
	/**
	 * Destroy the losable resource manager.  Should be called when shutting down.
	 */
	void CDirectX9LosableResourceManager::Destroy() {
		lsw::CCriticalSection::CEnterCrit lLockMe( m_csCrit );
		assert( m_vResources.size() == 0 );
		m_vResources.clear();
		m_vResources = std::vector<CDirectX9LosableResource *>();
	}

	/**
	 * Register a resource (also gives the resource a unique ID).
	 *
	 * \param _plrRes The resource to register.  Losable resources call this on
	 *	themselves directly, so this function should never be called by the user.
	 * \return Returns false if a memory error occurred.  If false is returned, the
	 *	engine must shut down.
	 */
	bool CDirectX9LosableResourceManager::RegisterRes( CDirectX9LosableResource * _plrRes ) {
		lsw::CCriticalSection::CEnterCrit lLockMe( m_csCrit );
		try {
			m_vResources.push_back( _plrRes );
			_plrRes->m_ui32UniqueLosableResourceId = ++m_ui32ResId;
			return true;
		}
		catch ( ... ) {
		}
		return false;
	}

	/**
	 * Removes a resource by its ID.
	 *
	 * \param _ui32Id Unique ID of the resource to remove from the list.
	 */
	void CDirectX9LosableResourceManager::RemoveRes( uint32_t _ui32Id ) {
		lsw::CCriticalSection::CEnterCrit lLockMe( m_csCrit );
		for ( size_t I = m_vResources.size(); I--; ) {
			if ( m_vResources[I]->m_ui32UniqueLosableResourceId == _ui32Id ) {
				m_vResources.erase( m_vResources.begin() + I );
			}
		}
		if ( m_vResources.size() == 0 ) { m_vResources.clear(); }
	}

	/**
	 * Notify all objects that the device has been lost.
	 */
	void CDirectX9LosableResourceManager::OnLostDevice() {
		lsw::CCriticalSection::CEnterCrit lLockMe( m_csCrit );

		for ( size_t I = m_vResources.size(); I--; ) {
			m_vResources[I]->OnDeviceLost();
		}
	}

	/**
	 * Notify all objects that the device has been reset.
	 */
	void CDirectX9LosableResourceManager::OnResetDevice() {
		lsw::CCriticalSection::CEnterCrit lLockMe( m_csCrit );

		for ( size_t I = m_vResources.size(); I--; ) {
			m_vResources[I]->OnDeviceReset();
		}
	}

	/**
	 * Gets the number of losable resources currently registered.
	 *
	 * \return Returns the number of registered losable resources.
	 */
	size_t CDirectX9LosableResourceManager::GetLosableResCount() {
		lsw::CCriticalSection::CEnterCrit lLockMe( m_csCrit );
		return m_vResources.size();
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
