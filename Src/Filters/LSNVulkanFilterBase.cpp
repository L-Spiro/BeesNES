#ifdef LSN_VULKAN1

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Vulkan 1.0 filters.
 */

#include "LSNVulkanFilterBase.h"
#include "../Utilities/LSNUtilities.h"

#ifdef LSN_WINDOWS
#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#endif

#include <algorithm>
#include <cstring>

namespace lsn {

	// == Members.
#ifdef LSN_WINDOWS
	/** Global: window class name for the Vulkan child target. */
	const wchar_t * CVulkanFilterBase::LSN_VULKAN_TARGET_CLASS = L"LSN_VULKAN_TARGET";
#endif

	/** The global Vulkan state containing the shared device. */
	CVulkanFilterBase::LSN_VULKAN_GLOBAL_STATE CVulkanFilterBase::s_vgsState;

	CVulkanFilterBase::CVulkanFilterBase() {
	}
	CVulkanFilterBase::~CVulkanFilterBase() {
	}

	// == Functions.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CVulkanFilterBase::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

		const uint16_t wBitDepth = uint16_t( OutputBits() );
		const uint32_t dwStride = uint32_t( RowStride( _ui16Width, wBitDepth ) );
		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( dwStride * _ui16Height );	
		}
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );
		return InputFormat();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CVulkanFilterBase::Activate() {
		CGpuFilterBase::Activate();
	}

	/**
	 * Called when the filter is no longer active.
	 */
	void CVulkanFilterBase::DeActivate() {
		m_fRenderFence.Reset();
		m_sImageAvailable.Reset();
		m_sRenderFinished.Reset();
		m_cbCommandBuffer.Reset();
		m_cpCommandPool.Reset();

		m_tgGamma.Reset();
		m_pPhosphor.Reset();
		m_tpsScaler.Reset();
		m_rsResampler.Reset();
		m_trRenderer.Reset();

		m_rpGammaPass.Reset();
		m_rpScalerPass.Reset();
		m_rpBackBufferPass.Reset();

		ReleaseBaseSizeDependents();
		ReleaseSwapchainResources();

		m_sPointSampler.Reset();
		m_sLinearSampler.Reset();
		m_dslCommonLayout.Reset();
		
		m_dsGammaSet.Reset();
		m_dsScalerSet.Reset();
		m_dsRendererSet.Reset();
		if ( m_pdpDescriptorPool ) { m_pdpDescriptorPool->Reset(); m_pdpDescriptorPool.reset(); }

		if ( m_pvkDevice ) {
			s_vgsState.DestroyVulkan();
			m_pvkDevice = nullptr;
		}

		CGpuFilterBase::DeActivate();
	}

	/**
	 * Presents the active swapchain frame to the target surface.
	 */
	void CVulkanFilterBase::Present() {
		if LSN_UNLIKELY( !s_vgsState.bValidState || !m_pvkDevice || !m_sRenderFinished.Get() || !m_bCanPresent ) { return; }
		m_bCanPresent = false; // Consume standard readiness.
		
		VkQueue qQueue = m_pvkDevice->GetCommandQueue();
		if ( !qQueue ) { return; }

		VkSwapchainKHR scSwapchains[] = { m_pvkDevice->GetSwapChain() };
		VkPresentInfoKHR piPresent = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		piPresent.waitSemaphoreCount = 1;
		VkSemaphore sSemaphores[] = { m_sRenderFinished.Get() };
		piPresent.pWaitSemaphores = sSemaphores;
		piPresent.swapchainCount = 1;
		piPresent.pSwapchains = scSwapchains;
		piPresent.pImageIndices = &m_ui32ImageIndex;

		VkResult rRes = CVulkan::m_pfQueuePresentKHR( qQueue, &piPresent );
		if ( rRes == VK_ERROR_OUT_OF_DATE_KHR || rRes == VK_SUBOPTIMAL_KHR ) {
			m_pvkDevice->ResizeSwapChain();
		}
	}

	/**
	 * Registers the custom Win32 child window class used for the rendering surface.
	 *
	 * \return Returns true if the class registered successfully.
	 */
	bool LSN_FASTCALL CVulkanFilterBase::RegisterVulkanTargetClass() {
#ifdef LSN_WINDOWS
		lsw::CWndClassEx wceEx(
			[]( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam )->LRESULT {
				switch ( _uMsg ) {
					case WM_ERASEBKGND : return 1;
					case WM_PAINT : {
						PAINTSTRUCT ps;
						::BeginPaint( _hWnd, &ps );
						::EndPaint( _hWnd, &ps );
						return 0;
					}
					case WM_PRINTCLIENT : return 0;
				}
				return ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam );
			}, LSN_VULKAN_TARGET_CLASS, CS_DBLCLKS, lsw::CBase::GetThisHandle(), NULL, ::LoadCursorW( NULL, IDC_ARROW ), NULL );
		return lsw::CBase::RegisterClassExW( wceEx.Obj() ) != 0;
#else
		// Platforms such as macOS/iOS use CAMetalLayer backed by NSView/UIView without manual window class registration.
		return true;
#endif
	}

	/**
	 * Triggers a layout update and swapchain resize.
	 *
	 * \return Returns true on success.
	 */
	bool CVulkanFilterBase::OnSizeVulkan() {
		s_vgsState.LayoutTargetChild();
		if ( s_vgsState.bValidState ) {
			if ( !s_vgsState.vkDevice.ResizeSwapChain() ) { return false; }
		}
		return true;
	}

	/**
	 * Ensures that the intermediate images and framebuffers are sized correctly to match the incoming data.
	 *
	 * \param _pvkDevice The active Vulkan device.
	 * \param _ui32NativeW The unscaled native output width.
	 * \param _ui32NativeH The unscaled native output height.
	 * \return Returns true on success.
	 */
	bool CVulkanFilterBase::EnsureBaseSizeAndResources( CVulkanDevice * _pvkDevice, uint32_t _ui32NativeW, uint32_t _ui32NativeH ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pvkDevice->GetDevice() ) { return false; }
		VkDevice dDevice = _pvkDevice->GetDevice();


		uint32_t ui32ImageCount;
		CVulkan::m_pfGetSwapchainImagesKHR( dDevice, _pvkDevice->GetSwapChain(), &ui32ImageCount, nullptr );
		if ( m_vSwapImages.size() != ui32ImageCount ) {
			ReleaseSwapchainResources();

			m_vSwapImages.resize( ui32ImageCount );
			CVulkan::m_pfGetSwapchainImagesKHR( dDevice, _pvkDevice->GetSwapChain(), &ui32ImageCount, m_vSwapImages.data() );

			m_vSwapViews.resize( ui32ImageCount );
			m_vSwapFramebuffers.resize( ui32ImageCount );

			if ( !m_rpBackBufferPass.Valid() ) { CreateRenderPass( VK_FORMAT_B8G8R8A8_UNORM, m_rpBackBufferPass, true ); }

#ifdef LSN_WINDOWS
			RECT rClient;
			::GetClientRect( s_vgsState.hWndTarget, &rClient );
			uint32_t ui32W = static_cast<uint32_t>(rClient.right - rClient.left);
			uint32_t ui32H = static_cast<uint32_t>(rClient.bottom - rClient.top);
#else
			// TODO.
			lsw::LSW_RECT rClient = s_vgsState.pwParent->VirtualClientRect( nullptr );
			uint32_t ui32W = static_cast<uint32_t>(rClient.Width());
			uint32_t ui32H = static_cast<uint32_t>(rClient.Height());
#endif

			for ( uint32_t I = 0; I < ui32ImageCount; ++I ) {
				VkImageViewCreateInfo ivciViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
				ivciViewInfo.image = m_vSwapImages[I];
				ivciViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ivciViewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
				ivciViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				ivciViewInfo.subresourceRange.levelCount = 1;
				ivciViewInfo.subresourceRange.layerCount = 1;
				m_vSwapViews[I].Create( dDevice, &ivciViewInfo );

				VkFramebufferCreateInfo fbciInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
				fbciInfo.renderPass = m_rpBackBufferPass.rpRenderPass;
				fbciInfo.attachmentCount = 1;
				fbciInfo.pAttachments = &m_vSwapViews[I].ivImageView;
				fbciInfo.width = ui32W;
				fbciInfo.height = ui32H;
				fbciInfo.layers = 1;
				m_vSwapFramebuffers[I].Create( dDevice, &fbciInfo );
			}
		}

		if ( !m_cpCommandPool.Get() ) {
			m_cpCommandPool.CreateCommandPool( dDevice, 0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );
			m_cbCommandBuffer.CreateCommandBuffer( dDevice, m_cpCommandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY );
			m_fRenderFence.CreateFence( dDevice, true ); // Signaled initially
			m_sImageAvailable.CreateSemaphore( dDevice );
			m_sRenderFinished.CreateSemaphore( dDevice );
		}

		if ( m_ui32RsrcW == _ui32NativeW && m_ui32RsrcH == _ui32NativeH && m_piGamma.get() && m_fbGamma.Valid() ) {
			return true;
		}

		ReleaseBaseSizeDependents();

		VkFormat fmtRt = m_bUse16BitInitialTarget ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;

		if ( !m_rpGammaPass.Valid() ) { CreateRenderPass( fmtRt, m_rpGammaPass ); }
		if ( !m_rpScalerPass.Valid() ) { CreateRenderPass( fmtRt, m_rpScalerPass ); }


		m_piGamma = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciInfo.imageType = VK_IMAGE_TYPE_2D;
		iciInfo.extent = { _ui32NativeW, _ui32NativeH, 1 };
		iciInfo.mipLevels = 1;
		iciInfo.arrayLayers = 1;
		iciInfo.format = fmtRt;
		iciInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iciInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		m_piGamma->CreateImage( dDevice, &iciInfo );

		VkMemoryRequirements mrReq;
		CVulkan::m_pfGetImageMemoryRequirements( dDevice, m_piGamma->Get(), &mrReq );

		VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		m_pdmGammaMemory = std::make_unique<CVulkanDeviceMemory>();
		m_pdmGammaMemory->AllocateMemory( dDevice, &maiAlloc );
		CVulkan::m_pfBindImageMemory( dDevice, m_piGamma->Get(), m_pdmGammaMemory->Get(), 0 );

		VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivciView.image = m_piGamma->Get();
		ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciView.format = fmtRt;
		ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		m_ivGammaView.Create( dDevice, &ivciView );

		VkFramebufferCreateInfo fbciFrame = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbciFrame.renderPass = m_rpGammaPass.rpRenderPass;
		fbciFrame.attachmentCount = 1;
		fbciFrame.pAttachments = &m_ivGammaView.ivImageView;
		fbciFrame.width = _ui32NativeW;
		fbciFrame.height = _ui32NativeH;
		fbciFrame.layers = 1;
		m_fbGamma.Create( dDevice, &fbciFrame );


		if ( !m_dslCommonLayout.Get() ) {
			VkDescriptorSetLayoutBinding dslbBinding = {};
			dslbBinding.binding = 0;
			dslbBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			dslbBinding.descriptorCount = 1;
			dslbBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutCreateInfo dslciLayout = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			dslciLayout.bindingCount = 1;
			dslciLayout.pBindings = &dslbBinding;
			m_dslCommonLayout.CreateDescriptorSetLayout( dDevice, &dslciLayout );
		}

		if ( !m_pdpDescriptorPool ) {
			VkDescriptorPoolSize dpsSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 };
			VkDescriptorPoolCreateInfo dpciPool = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
			dpciPool.poolSizeCount = 1;
			dpciPool.pPoolSizes = &dpsSize;
			dpciPool.maxSets = 10;
			
			m_pdpDescriptorPool = std::make_unique<CVulkanDescriptorPool>();
			m_pdpDescriptorPool->CreateDescriptorPool( dDevice, &dpciPool );

			VkDescriptorSetLayout layouts[] = { m_dslCommonLayout.Get(), m_dslCommonLayout.Get(), m_dslCommonLayout.Get() };
			VkDescriptorSetAllocateInfo dsaiAlloc = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
			dsaiAlloc.descriptorPool = m_pdpDescriptorPool->Get();
			dsaiAlloc.descriptorSetCount = 3;
			dsaiAlloc.pSetLayouts = layouts;
			
			VkDescriptorSet allocated[3];
			CVulkan::m_pfAllocateDescriptorSets( dDevice, &dsaiAlloc, allocated );
			m_dsGammaSet.dsDescriptorSet = allocated[0];
			m_dsScalerSet.dsDescriptorSet = allocated[1];
			m_dsRendererSet.dsDescriptorSet = allocated[2];
		}

		if ( !CreateSamplers( dDevice ) ) { return false; }

		m_ui32RsrcW = _ui32NativeW;
		m_ui32RsrcH = _ui32NativeH;
		return true;
	}

	/**
	 * Releases base intermediate resources depending on the native source size.
	 */
	void CVulkanFilterBase::ReleaseBaseSizeDependents() {
		m_fbGamma.Reset();
		m_ivGammaView.Reset();
		if LSN_LIKELY( m_pdmGammaMemory.get() ) { m_pdmGammaMemory->Reset(); }
		if LSN_LIKELY( m_piGamma.get() ) { m_piGamma->Reset(); }

		m_fbResampled.Reset();
		m_ivResampledView.Reset();
		if LSN_LIKELY( m_pdmResampledMemory.get() ) { m_pdmResampledMemory->Reset(); }
		if LSN_LIKELY( m_piResampled.get() ) { m_piResampled->Reset(); }

		m_fbScaler.Reset();
		m_ui32RsrcW = m_ui32RsrcH = 0;
	}

	/**
	 * Releases resources tied directly to the backbuffer count and extent of the swapchain.
	 */
	void CVulkanFilterBase::ReleaseSwapchainResources() {
		m_vSwapFramebuffers.clear();
		m_vSwapViews.clear();
		m_vSwapImages.clear();
	}

	/**
	 * Renders the common Vulkan pipeline components sequentially.
	 *
	 * \param _pvkDevice The active Vulkan device.
	 * \param _pcbCommandBuffer The command buffer in which to record pass commands.
	 * \param _ivSrc The incoming image view generated by the emulator core.
	 * \param _sSrcSampler The sampler used to read the initial source image.
	 * \param _ui32NativeW The base resolution width (prior to scaling).
	 * \param _ui32NativeH The base resolution height (prior to scaling).
	 * \param _rOutput The destination rectangle on the target window.
	 * \param _ui32ImageIndex The active swapchain backbuffer index being rendered to.
	 * \param _bFlipY Determines whether the integer scaler pass should flip the Y axis.
	 * \return Returns true if pipeline execution was successfully recorded.
	 */
	bool CVulkanFilterBase::RenderBase( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandBuffer, VkImageView _ivSrc, VkSampler _sSrcSampler, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, uint32_t _ui32ImageIndex, bool _bFlipY ) {
		if LSN_UNLIKELY( !_ivSrc || !_pvkDevice || !_pcbCommandBuffer ) { return false; }
		if LSN_UNLIKELY( !EnsureBaseSizeAndResources( _pvkDevice, _ui32NativeW, _ui32NativeH ) ) { return false; }

		VkCommandBuffer cbCmd = _pcbCommandBuffer->Get();
		VkImageView ivCurrentSource = _ivSrc;
		VkSampler sCurrentSampler = _sSrcSampler ? _sSrcSampler : m_sPointSampler.sSampler;
		VkFormat fmtRt = m_bUse16BitInitialTarget ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
		std::vector<uint32_t> vDummy;

		// Initialize shaders and resources for the base stages
		if ( !m_tgGamma.EnsureResources( _pvkDevice ) || 
			 !m_tgGamma.EnsureShaders( _pvkDevice, m_rpGammaPass.rpRenderPass, fmtRt, vDummy, vDummy ) ) { return false; }

		if ( !m_trRenderer.EnsureResources( _pvkDevice ) || 
			 !m_trRenderer.EnsureShaders( _pvkDevice, m_rpBackBufferPass.rpRenderPass, VK_FORMAT_B8G8R8A8_UNORM, vDummy, vDummy ) ) { return false; }

		CNesPalette::LSN_GAMMA effGamma = GetEffectiveGamma();
		if ( effGamma != CNesPalette::LSN_G_NONE ) {
			UpdateDescriptorSet( m_dsGammaSet.dsDescriptorSet, ivCurrentSource, sCurrentSampler );

			VkRenderPassBeginInfo rpbiInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			rpbiInfo.renderPass = m_rpGammaPass.rpRenderPass;
			rpbiInfo.framebuffer = m_fbGamma.fbFramebuffer;
			rpbiInfo.renderArea.extent = { _ui32NativeW, _ui32NativeH };
			
			CVulkan::m_pfCmdBeginRenderPass( cbCmd, &rpbiInfo, VK_SUBPASS_CONTENTS_INLINE );
			m_tgGamma.Render( _pvkDevice, _pcbCommandBuffer, m_dsGammaSet.dsDescriptorSet, _ui32NativeW, _ui32NativeH, effGamma );
			CVulkan::m_pfCmdEndRenderPass( cbCmd );

			ivCurrentSource = m_ivGammaView.ivImageView;
			sCurrentSampler = m_sPointSampler.sSampler;
		}

		if ( m_bEnablePhosphorDecay ) {
			if ( !m_pPhosphor.EnsureResources( _pvkDevice, _pcbCommandBuffer, _ui32NativeW, _ui32NativeH, fmtRt ) ||
				 !m_pPhosphor.EnsureShaders( _pvkDevice, fmtRt, vDummy, vDummy ) ) { return false; }

			if ( m_pPhosphor.RenderPhosphor( _pvkDevice, _pcbCommandBuffer, ivCurrentSource, sCurrentSampler, _ui32NativeW, _ui32NativeH, m_fPhosphorDecayRateRed, m_fPhosphorDecayRateGreen, m_fPhosphorDecayRateBlue, m_fInitPhosphorDecay ) ) {
				ivCurrentSource = m_pPhosphor.GetCurrentPhosphorView();
			}
		}

		uint32_t uiActualW = GetActualHorSharpness( _rOutput.Width() );
		uint32_t uiActualH = GetActualVertSharpness( _rOutput.Height() );

		if ( m_tpsScaler.EnsureResources( _pvkDevice, uiActualW, uiActualH, fmtRt ) ) {
			if ( !m_tpsScaler.EnsureShaders( _pvkDevice, m_rpScalerPass.rpRenderPass, fmtRt, vDummy, vDummy ) ) { return false; }

			if ( !m_fbScaler.Valid() ) {
				VkFramebufferCreateInfo fbciFrame = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
				fbciFrame.renderPass = m_rpScalerPass.rpRenderPass;
				fbciFrame.attachmentCount = 1;
				VkImageView ivScalerTarget = m_tpsScaler.GetTargetView(); 
				fbciFrame.pAttachments = &ivScalerTarget;
				fbciFrame.width = uiActualW;
				fbciFrame.height = uiActualH;
				fbciFrame.layers = 1;
				m_fbScaler.Create( _pvkDevice->GetDevice(), &fbciFrame );
			}

			UpdateDescriptorSet( m_dsScalerSet.dsDescriptorSet, ivCurrentSource, sCurrentSampler );

			VkRenderPassBeginInfo rpbiInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			rpbiInfo.renderPass = m_rpScalerPass.rpRenderPass;
			rpbiInfo.framebuffer = m_fbScaler.fbFramebuffer;
			rpbiInfo.renderArea.extent = { uiActualW, uiActualH };
			
			CVulkan::m_pfCmdBeginRenderPass( cbCmd, &rpbiInfo, VK_SUBPASS_CONTENTS_INLINE );
			m_tpsScaler.Render( _pvkDevice, _pcbCommandBuffer, m_dsScalerSet.dsDescriptorSet, _ui32NativeW, _ui32NativeH, _rOutput, _bFlipY );
			CVulkan::m_pfCmdEndRenderPass( cbCmd );

			ivCurrentSource = m_tpsScaler.GetTargetView();
		}

		uint32_t ui32DstW = static_cast<uint32_t>(_rOutput.Width());
		uint32_t ui32DstH = static_cast<uint32_t>(_rOutput.Height());

		if ( m_bUseHighQualityResampler ) {
			m_rsResampler.SetFilter( GetPreferredConvolutionFilter( ui32DstW, ui32DstH ) );
			
			if LSN_UNLIKELY( !m_piResampled.get() || m_ui32ResampledTargetW != ui32DstW || m_ui32ResampledTargetH != ui32DstH ) {
				m_fbResampled.Reset();
				m_ivResampledView.Reset();
				if ( m_pdmResampledMemory.get() ) { m_pdmResampledMemory->Reset(); }
				if ( m_piResampled.get() ) { m_piResampled->Reset(); }

				m_piResampled = std::make_unique<CVulkanImage>();
				VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
				iciInfo.imageType = VK_IMAGE_TYPE_2D;
				iciInfo.extent = { ui32DstW, ui32DstH, 1 };
				iciInfo.mipLevels = 1;
				iciInfo.arrayLayers = 1;
				iciInfo.format = fmtRt;
				iciInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				iciInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				iciInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				iciInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				m_piResampled->CreateImage( _pvkDevice->GetDevice(), &iciInfo );

				VkMemoryRequirements mrReq;
				CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), m_piResampled->Get(), &mrReq );

				VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
				maiAlloc.allocationSize = mrReq.size;
				maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

				m_pdmResampledMemory = std::make_unique<CVulkanDeviceMemory>();
				m_pdmResampledMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc );
				CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), m_piResampled->Get(), m_pdmResampledMemory->Get(), 0 );

				VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
				ivciView.image = m_piResampled->Get();
				ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ivciView.format = fmtRt;
				ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				m_ivResampledView.Create( _pvkDevice->GetDevice(), &ivciView );

				VkFramebufferCreateInfo fbciFrame = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
				fbciFrame.renderPass = m_rpScalerPass.rpRenderPass;		// Shares identical layout signatures.
				fbciFrame.attachmentCount = 1;
				fbciFrame.pAttachments = &m_ivResampledView.ivImageView;
				fbciFrame.width = ui32DstW;
				fbciFrame.height = ui32DstH;
				fbciFrame.layers = 1;
				m_fbResampled.Create( _pvkDevice->GetDevice(), &fbciFrame );

				m_ui32ResampledTargetW = ui32DstW;
				m_ui32ResampledTargetH = ui32DstH;
			}

			if ( !m_rsResampler.EnsureResources( _pvkDevice, _pcbCommandBuffer, uiActualW, uiActualH, ui32DstW, ui32DstH, fmtRt, fmtRt ) ||
				 !m_rsResampler.EnsureShaders( _pvkDevice, vDummy, vDummy ) ) { return false; }

			if ( m_rsResampler.Render( _pvkDevice, _pcbCommandBuffer, ivCurrentSource, m_sPointSampler.sSampler, m_fbResampled.fbFramebuffer, _rOutput, false ) ) {
				ivCurrentSource = m_ivResampledView.ivImageView;
			}
		}

		UpdateDescriptorSet( m_dsRendererSet.dsDescriptorSet, ivCurrentSource, m_sLinearSampler.sSampler );

		// =========================================================================
		// MAGENTA CLEAR COLOR INJECTION (To verify rendering layer)
		// =========================================================================
		VkClearValue cvClear = {};
		cvClear.color.float32[0] = 1.0f; // R
		cvClear.color.float32[1] = 0.0f; // G
		cvClear.color.float32[2] = 1.0f; // B
		cvClear.color.float32[3] = 1.0f; // A

		VkRenderPassBeginInfo rpbiBackBuffer = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbiBackBuffer.renderPass = m_rpBackBufferPass.rpRenderPass;
		rpbiBackBuffer.framebuffer = m_vSwapFramebuffers[_ui32ImageIndex].fbFramebuffer;
		rpbiBackBuffer.renderArea.offset = { 0, 0 };
		rpbiBackBuffer.renderArea.extent = { ui32DstW, ui32DstH };
		/*rpbiBackBuffer.clearValueCount = 1;
		rpbiBackBuffer.pClearValues = &cvClear;*/
		
		CVulkan::m_pfCmdBeginRenderPass( cbCmd, &rpbiBackBuffer, VK_SUBPASS_CONTENTS_INLINE );
		m_trRenderer.Render( _pvkDevice, _pcbCommandBuffer, m_dsRendererSet.dsDescriptorSet, _rOutput );
		CVulkan::m_pfCmdEndRenderPass( cbCmd );

		return true;
	}

	/**
	 * Helper to construct a standard rendering pass format layout.
	 *
	 * \param _fFormat The image format expected for the attachment.
	 * \param _rpPass The wrapper object receiving the created render pass.
	 * \param _bIsBackBuffer True if the layout should transition to PRESENT_SRC_KHR at the end of the pass.
	 * \return Returns true if the pass created successfully.
	 */
	bool CVulkanFilterBase::CreateRenderPass( VkFormat _fFormat, CVulkan::LSN_RENDER_PASS &_rpPass, bool _bIsBackBuffer ) {
		VkAttachmentDescription adAttachment = {};
		adAttachment.format = _fFormat;
		adAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		// NOTE: Changed to CLEAR for the backbuffer to verify rendering.
		//adAttachment.loadOp = _bIsBackBuffer ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		adAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		adAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		adAttachment.finalLayout = _bIsBackBuffer ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference arColorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

		VkSubpassDescription sdSubpass = {};
		sdSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		sdSubpass.colorAttachmentCount = 1;
		sdSubpass.pColorAttachments = &arColorRef;

		VkRenderPassCreateInfo rpciInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		rpciInfo.attachmentCount = 1;
		rpciInfo.pAttachments = &adAttachment;
		rpciInfo.subpassCount = 1;
		rpciInfo.pSubpasses = &sdSubpass;

		return _rpPass.Create( m_pvkDevice->GetDevice(), &rpciInfo );
	}

	/**
	 * Binds an image view and a sampler dynamically to the specified descriptor set.
	 *
	 * \param _dsSet The target descriptor set to modify.
	 * \param _ivView The image view to bind to binding 0.
	 * \param _sSampler The sampler to bind to binding 0.
	 */
	void CVulkanFilterBase::UpdateDescriptorSet( VkDescriptorSet _dsSet, VkImageView _ivView, VkSampler _sSampler ) {
		VkDescriptorImageInfo diiInfo = { _sSampler, _ivView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkWriteDescriptorSet wdsWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		wdsWrite.dstSet = _dsSet;
		wdsWrite.dstBinding = 0;
		wdsWrite.dstArrayElement = 0;
		wdsWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wdsWrite.descriptorCount = 1;
		wdsWrite.pImageInfo = &diiInfo;
		CVulkan::m_pfUpdateDescriptorSets( m_pvkDevice->GetDevice(), 1, &wdsWrite, 0, nullptr );
	}

	/**
	 * Creates the point sampler, m_sPointSampler.
	 * 
	 * \param _dDevice The Vulkan device.
	 * \return Returns true if creating the point sampler m_sPointSampler succeeded.
	 **/
	bool CVulkanFilterBase::CreateSamplers( VkDevice _dDevice ) {
		if ( !m_sPointSampler.Valid() ) {
			VkSamplerCreateInfo sciSampler = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
			sciSampler.magFilter = VK_FILTER_NEAREST;
			sciSampler.minFilter = VK_FILTER_NEAREST;
			sciSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			sciSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sciSampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sciSampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sciSampler.maxAnisotropy = 1.0f;
			if ( !m_sPointSampler.Create( _dDevice, &sciSampler ) ) { return false; }

			sciSampler.magFilter = VK_FILTER_LINEAR;
			sciSampler.minFilter = VK_FILTER_LINEAR;
			if ( !m_sLinearSampler.Create( _dDevice, &sciSampler ) ) {
				m_sPointSampler.Reset();
				return false;
			}
		}
		return true;
	}

	// == LSN_VULKAN_GLOBAL_STATE ==
	/**
	 * Initializes the shared Vulkan device and swapchain.
	 *
	 * \return Returns true on success.
	 */
	bool CVulkanFilterBase::LSN_VULKAN_GLOBAL_STATE::CreateVulkan() {
		if ( vkDevice.GetDevice() ) {
			++i32RefCnt;
			return true;
		}
		bValidState = false;
		if ( !pwParent || !CVulkan::Supported() ) { return false; }
		if ( !CVulkanFilterBase::RegisterVulkanTargetClass() ) { return false; }
		if ( !CreateTargetChild() ) { return false; }
		LayoutTargetChild();

		std::string sAdapter;
#ifdef LSN_WINDOWS
		if ( vkDevice.Create( hWndTarget, sAdapter ) ) {
#else
		if ( vkDevice.Create( reinterpret_cast<HWND>(pvTarget), sAdapter ) ) {
#endif
			++i32RefCnt;
			bValidState = true;
			return true;
		}
		
		return false;
	}

	/**
	 * Creates the child window/view target for rendering.
	 *
	 * \return Returns true on success.
	 */
	bool CVulkanFilterBase::LSN_VULKAN_GLOBAL_STATE::CreateTargetChild() {
#ifdef LSN_WINDOWS
		if ( hWndTarget && ::IsWindow( hWndTarget ) ) { return true; }
		if ( !pwParent ) { return false; }
		constexpr DWORD dwStyle   = WS_CHILD | WS_VISIBLE;
		constexpr DWORD dwExStyle = WS_EX_NOPARENTNOTIFY;
		rScreenRect = pwParent->VirtualClientRect( nullptr );
		hWndTarget = ::CreateWindowExW(
			dwExStyle, CVulkanFilterBase::LSN_VULKAN_TARGET_CLASS, L"", dwStyle,
			rScreenRect.left, rScreenRect.top, rScreenRect.Width(), rScreenRect.Height(),
			pwParent->Wnd(), NULL, ::GetModuleHandleW( NULL ), NULL );

		::SetClassLongPtrW( hWndTarget, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(nullptr) );
		return hWndTarget != nullptr;
#elif defined( LSN_APPLE )
		if ( pvTarget ) { return true; }
		if ( !pwParent ) { return false; }
		// TODO: Extract CAMetalLayer from the parent widget/view.
		return true;
#else
		if ( pvTarget ) { return true; }
		if ( !pwParent ) { return false; }
		// TODO: Construct Xlib/Wayland window child surface.
		return true;
#endif
	}

	/**
	 * Destroys the shared Vulkan device if the reference count reaches zero.
	 */
	void CVulkanFilterBase::LSN_VULKAN_GLOBAL_STATE::DestroyVulkan() {
		if ( i32RefCnt ) {
			if ( --i32RefCnt == 0 ) {
				bValidState = false;
				vkDevice.Reset();
				
#ifdef LSN_WINDOWS
				if ( hWndTarget ) {
					::DestroyWindow( hWndTarget );
					hWndTarget = NULL;
				}
#else
				if ( pvTarget ) {
					pvTarget = nullptr;
				}
#endif
			}
		}
	}

	/**
	 * Updates the bounds of the child target window/view to match the parent.
	 *
	 * \return Returns true on success.
	 */
	bool CVulkanFilterBase::LSN_VULKAN_GLOBAL_STATE::LayoutTargetChild() {
#ifdef LSN_WINDOWS
		if ( !hWndTarget || !::IsWindow( hWndTarget ) ) { return false; }
		
		rScreenRect = pwParent->VirtualClientRect( nullptr );
		const int iW = int( std::max<LONG>( 1, rScreenRect.Width() ) );
		const int iH = int( std::max<LONG>( 1, rScreenRect.Height() ) );
		
		::SetWindowPos( hWndTarget, nullptr, rScreenRect.left, rScreenRect.top, iW, iH,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS );
		return true;
#else
		if ( !pvTarget ) { return false; }
		rScreenRect = pwParent->VirtualClientRect( nullptr );
		// TODO: Execute explicit Metal Layer or Xlib bounding dimension updates.
		return true;
#endif
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
