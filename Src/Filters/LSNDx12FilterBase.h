/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Direct3D 12 filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNGpuFilterBase.h"

#include "../GPU/DirectX12/LSNDirectX12.h"
#include "../GPU/DirectX12/LSNDirectX12Device.h"
#include "../GPU/DirectX12/LSNDirectX12CommandQueue.h"
#include "../GPU/DirectX12/LSNDirectX12GraphicsCommandList.h"
#include "../GPU/DirectX12/LSNDirectX12DescriptorHeap.h"
#include "../GPU/DirectX12/LSNDirectX12Fence.h"
#include "../GPU/DirectX12/LSNDirectX12Resource.h"

#include <Widget/LSWWidget.h>

#include <vector>

#ifdef LSN_DX12

namespace lsn {

	/**
	 * Class CDx12FilterBase
	 * \brief The base class for Direct3D 12 filters.
	 *
	 * Description: Manages a shared D3D12 device and swap-chain context for all D3D12-based
	 *	filters. Derived classes build their pipelines (root signatures, PSOs, resources) on top.
	 *	This mirrors the DX9 base in structure while using the D3D12 wrappers.
	 */
	class CDx12FilterBase : public CGpuFilterBase {
	public :
		CDx12FilterBase();
		virtual ~CDx12FilterBase();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) override;

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate() override;

		/**
		 * Informs the filter of a window resize.
		 *
		 * Derived filters typically rebuild size-dependent resources in response.
		 */
		virtual void										FrameResize() override;

		/**
		 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.  The final buffer, along with
		 *	its width, height, bit-depth, and stride, are returned.
		 *
		 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.  Its format will be that returned in InputFormat().
		 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
		 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
		 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
		 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
		 * \param _ui64PpuFrame The PPU frame associated with the input data.
		 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
		 * \param _i32DispLeft The display area left.
		 * \param _i32DispTop The display area top.
		 * \param _ui32DispWidth The display area width.
		 * \param _ui32DispHeight The display area height.
		 * \return Returns a pointer to the filtered output buffer (if used).  For D3D12 presenters, this is typically unused.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * /*_pui8Input*/, uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
			int32_t /*_i32DispLeft*/, int32_t /*_i32DispTop*/, uint32_t /*_ui32DispWidth*/, uint32_t /*_ui32DispHeight*/ ) override { return nullptr; }

		/**
		 * Which GPU API is being used?
		 * 
		 * \return Returns a LSN_GRAPHICS_API value indicating which GPU API is being used, if any.
		 **/
		virtual LSN_GRAPHICS_API							GpuApi() const { return LSN_GA_DX12; }

		// == Members.
		/** Global: window class name for the DX12 child target. */
		static const wchar_t *								LSN_DX12_TARGET_CLASS;


		// == Functions.
		/**
		 * Sets the parent window handle.  Must be set only when there is a 0 reference count.
		 *
		 * \param _pwParent The parent window for the present target (swap chain owner).
		 * \return Returns false if the global reference count is not 0.
		 **/
		static bool											SetRenderWindowParent( lsw::CWidget * _pwParent ) {
			if ( s_dgsState.i32RefCnt != 0 ) { return false; }
			s_dgsState.pwParent = _pwParent;
			return true;
		}

		/**
		 * \brief Check cooperative level and recover from device loss when possible.
		 *
		 * \return Returns true if the device is usable for this frame; false if lost.
		 **/
		static bool											HandleDeviceLoss() {
			if ( !s_dgsState.i32RefCnt || !s_dgsState.hWndTarget || !s_dgsState.dx12Device.GetDevice() ) { return false; }
			HRESULT hRes = s_dgsState.dx12Device.GetDevice()->GetDeviceRemovedReason();
			if ( FAILED( hRes ) ) {
				// D3D12 device is completely removed. Requires full teardown and recreation.
				return false; 
			}
			return true;
		}

		/**
		 * Gets a reference to the shared global device.
		 * 
		 * \return Returns a reference to the shared global device.
		 **/
		static CDirectX12Device &							Device();

		/**
		 * \brief Handles a window-size change for the swap chain.
		 *
		 * Re-sizes the backbuffers to match the client area of the parent window.
		 *
		 * \return Returns true on success; false if the device is not available or resize failed.
		 */
		static bool											OnSizeDx12();


	protected :
		// == Types.
		/** The shared base data for all Direct3D 12 filters. */
		struct LSN_DX12_GLOBAL_STATE {
			lsw::CWidget *									pwParent = nullptr;										/**< Parent window (swap-chain owner). */
			HWND											hWndTarget = NULL;										/**< Render window. */
			CDirectX12Device								dx12Device;												/**< DirectX 12 device and swap-chain owner. */
			int32_t											i32RefCnt = 0;											/**< Reference count. */
			lsw::LSW_RECT									rScreenRect;											/**< Cached output rectangle in client coordinates. */
			bool											bValidState = false;									/**< Is the device path currently valid? */

			/**
			 * Initializes the DirectX 12 device for the current parent window.
			 * 
			 * \return Returns true if the device/swap chain are ready.
			 **/
			bool											CreateDx12();

			/**
			 * \brief Creates the child target window used for DX12 presentation.
			 * 
			 * \return Returns true if created or already exists.
			 */
			bool											CreateDx12TargetChild();

			/**
			 * Destroys the DirectX 12 device and swap chain when the ref-count reaches 0.
			 **/
			void											DestroyDx12();

			/**
			 * \brief Lays out and caches the drawable client region for the swap chain.
			 *
			 * \return Returns true on success.
			 */
			bool											LayoutTargetChild();

			~LSN_DX12_GLOBAL_STATE();
		};

#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fW;
			float fU, fV;
		};
#pragma pack( pop )


		// == Members.
		/** The global Direct3D 12 state. */
		static LSN_DX12_GLOBAL_STATE						s_dgsState;

		// == Functions.
		/**
		 * \brief Registers the DX12 child target window class (no background erase).
		 * 
		 * \return Returns true if the class is registered or already existed.
		 */
		static bool LSN_FASTCALL							RegisterDx12TargetClass();

		/**
		 * \brief Fills a buffer with an XYZRHW|TEX1 quad.
		 *
		 * Note: Direct3D 12 does not require the -0.5f offset used in Direct3D 9.
		 *
		 * \param _dx12rBuffer The vertex buffer resource to arrange.
		 * \param _fL Left X in pixels.
		 * \param _fT Top Y in pixels.
		 * \param _fR Right X in pixels.
		 * \param _fB Bottom Y in pixels.
		 * \param _fU0 Left U coordinate.
		 * \param _fV0 Top V coordinate.
		 * \param _fU1 Right U coordinate.
		 * \param _fV1 Bottom V coordinate.
		 * \return Returns true on success.
		 */
		bool												FillQuad( CDirectX12Resource &_dx12rBuffer, float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 );

		/**
		 * \brief Computes UVs for a WxH texture.
		 *
		 * D3D12 does not require half-texel offsets, making this function a passthrough for API parity.
		 */
		static inline void									HalfTexelUv( uint32_t /*_ui32W*/, uint32_t /*_ui32H*/,
			float &_fU0, float &_fV0, float &_fU1, float &_fV1 ) {
			_fU0 = 0.0f;
			_fV0 = 0.0f;
			_fU1 = 1.0f;
			_fV1 = 1.0f;
		}

	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
