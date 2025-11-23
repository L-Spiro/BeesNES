#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Direct3D 12 filters.
 */

#include "LSNDx12FilterBase.h"
#include "../Utilities/LSNUtilities.h"

#include <Base/LSWBase.h>

namespace lsn {

	// == Members.
	CDx12FilterBase::LSN_DX12_GLOBAL_STATE CDx12FilterBase::s_dgsState;

	CDx12FilterBase::CDx12FilterBase() {
	}
	CDx12FilterBase::~CDx12FilterBase() {
	}


	// == CDx12FilterBase.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CDx12FilterBase::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		// Allocate generic double/triple-buffered software targets even for GPU filters (keeps interface stable).
		return CGpuFilterBase::Init( _stBuffers, _ui16Width, _ui16Height );		// :contentReference[oaicite:0]{index=0} :contentReference[oaicite:1]{index=1}
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CDx12FilterBase::Activate() {
		CGpuFilterBase::Activate();												// :contentReference[oaicite:2]{index=2}

		// Create device/swap chain on first activation.
		if ( s_dgsState.CreateDx12() ) {
			s_dgsState.bValidState = true;
		}
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CDx12FilterBase::DeActivate() {
		CGpuFilterBase::DeActivate();											// :contentReference[oaicite:3]{index=3}

		// Drop a reference; destroy when the last filter deactivates.
		s_dgsState.DestroyDx12();
	}

	/**
	 * Informs the filter of a window resize.
	 */
	void CDx12FilterBase::FrameResize() {
		OnSizeDx12();
	}

	/**
	 * Gets a reference to the shared global device.
	 * 
	 * \return Returns a reference to the shared global device.
	 **/
	CDirectX12Device & CDx12FilterBase::Device() {
		return s_dgsState.dx12Device;											// device wrapper exists and owns swap chain. :contentReference[oaicite:4]{index=4}
	}

	/**
	 * \brief Handles a window-size change for the swap chain.
	 *
	 * \return Returns true on success; false if the device is not available or resize failed.
	 */
	bool CDx12FilterBase::OnSizeDx12() {
		if ( !s_dgsState.pwParent ) { return false; }
		if ( !s_dgsState.dx12Device.GetDevice() ) { return false; }			// wrapper access. :contentReference[oaicite:5]{index=5}

		if ( !s_dgsState.LayoutTarget() ) { return false; }

		// Ask the device to resize its swap chain/backbuffers to match the current client area.
		// The device wrapper owns the swap chain; its Resize() path should flush and rebuild RTVs.
		//return s_dgsState.dx12Device.ResizeForWindow( s_dgsState.pwParent );	// implemented in your device; keeps hwnd and present params cached. :contentReference[oaicite:6]{index=6}
		return false;	// TODO.
	}


	// == LSN_DX12_GLOBAL_STATE.
	/**
	 * Initializes the DirectX 12 device for the current parent window.
	 * 
	 * \return Returns true if the device/swap chain are ready.
	 **/
	bool CDx12FilterBase::LSN_DX12_GLOBAL_STATE::CreateDx12() {
		if ( dx12Device.GetDevice() ) {
			++i32RefCnt;
			return true;
		}
		bValidState = false;

		// Let the wrapper decide dynamic availability; Supported() will check for d3d12/dxgi.
		if ( !pwParent || !CDirectX12::Supported() ) { return false; }
		if ( !LayoutTarget() ) { return false; }

		// Get the list of display devices.
		std::string sAdapter;

		// Create with default adapter; window-bound swap chain targets pwParent->Wnd().
		if ( dx12Device.Create( pwParent->Wnd(), sAdapter ) ) {
			++i32RefCnt;
			bValidState = true;
			return true;
		}
		return false;
	}

	/**
	 * Destroys the DirectX 12 device and swap chain when the ref-count reaches 0.
	 **/
	void CDx12FilterBase::LSN_DX12_GLOBAL_STATE::DestroyDx12() {
		if ( i32RefCnt ) {
			if ( --i32RefCnt == 0 ) {
				bValidState = false;
				dx12Device.Reset();											// release device/queue/swap chain; RAII inside wrappers. :contentReference[oaicite:10]{index=10}
			}
		}
	}

	/**
	 * \brief Lays out and caches the drawable client region for the swap chain.
	 *
	 * \return Returns true on success.
	 */
	bool CDx12FilterBase::LSN_DX12_GLOBAL_STATE::LayoutTarget() {
		if ( !pwParent ) { return false; }
		rScreenRect = pwParent->VirtualClientRect( nullptr );
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX12
