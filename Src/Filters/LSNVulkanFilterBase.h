#ifdef LSN_VULKAN1

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Vulkan 1.0 filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNGpuFilterBase.h"
#include "LSNNesPalette.h"
#include "LSNResamplerBase.h"

#include "../GPU/Vulkan/LSNVulkan.h"
#include "../GPU/Vulkan/LSNVulkanDevice.h"
#include "../GPU/Vulkan/LSNVulkanCommandBuffer.h"
#include "../GPU/Vulkan/LSNVulkanImage.h"
#include "../GPU/Vulkan/LSNVulkanDeviceMemory.h"
#include "../GPU/Vulkan/LSNVulkanDescriptorPool.h"
#include "../GPU/Vulkan/LSNVulkanDescriptorSetLayout.h"

#include "../GPU/Vulkan/LSNVulkanCommandPool.h"
#include "../GPU/Vulkan/LSNVulkanPhosphor.h"
#include "../GPU/Vulkan/LSNVulkanResampler.h"
#include "../GPU/Vulkan/LSNVulkanTextureGamma.h"
#include "../GPU/Vulkan/LSNVulkanTexturePixelScaler.h"
#include "../GPU/Vulkan/LSNVulkanTextureRenderer.h"
#include "../Utilities/LSNUtilities.h"

#include <Widget/LSWWidget.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>


namespace lsn {

	/**
	 * Class CVulkanFilterBase
	 * \brief The base class for Vulkan 1.0 filters.
	 *
	 * Description: Manages a shared Vulkan device and swap-chain context for all Vulkan-based
	 *	filters. Derived classes build their pipelines using this common device.
	 */
	class CVulkanFilterBase : public CGpuFilterBase {
	public :
		CVulkanFilterBase();
		virtual ~CVulkanFilterBase();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT				Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) override;
		
		/**
		 * Gets the GPU API targeted by this filter.
		 *
		 * \return Returns LSN_GA_VULKAN.
		 */
		virtual LSN_GRAPHICS_API								GpuApi() const override { return LSN_GA_VULKAN; }
		
		/**
		 * Called when the filter is about to become active.
		 */
		virtual void											Activate() override;
		
		/**
		 * Called when the filter is no longer active.
		 */
		virtual void											DeActivate() override;

		/**
		 * Sets the window parent for the Vulkan rendering child target.
		 *
		 * \param _pwParent A pointer to the parent widget.
		 * \return Returns true if the parent was successfully set.
		 */
		static bool												SetRenderWindowParent( lsw::CWidget * _pwParent ) {
			if ( s_vgsState.i32RefCnt != 0 ) { return false; }
			s_vgsState.pwParent = _pwParent;
			return true;
		}

		/**
		 * Check cooperative level and recover from device loss when possible.
		 *
		 * \return Returns true if the device is usable for this frame.
		 */
		static bool												HandleDeviceLoss() {
			if ( !s_vgsState.i32RefCnt || !s_vgsState.hWndTarget || !s_vgsState.vkDevice.GetDevice() ) { return false; }
			return true;
		}

		/**
		 * Gets a reference to the shared Vulkan device.
		 *
		 * \return Returns a reference to the global CVulkanDevice instance.
		 */
		static CVulkanDevice &									Device() { return s_vgsState.vkDevice; }

		// == Common Pipeline Accessors ==
		/**
		 * Sets the vertical sharpness factor.
		 *
		 * \param _ui32Factor The multiplier for the vertical dimension before resampling.
		 */
		inline void												SetVertSharpness( uint32_t _ui32Factor ) { m_ui32VertSharpness = std::max<uint32_t>( 1, _ui32Factor ); }
		
		/**
		 * Gets the vertical sharpness factor.
		 *
		 * \return Returns the vertical sharpness factor.
		 */
		inline uint32_t											GetVertSharpness() const { return m_ui32VertSharpness; }
		
		/**
		 * Sets the horizontal sharpness factor.
		 *
		 * \param _ui32Factor The multiplier for the horizontal dimension before resampling.
		 */
		inline void												SetHorSharpness( uint32_t _ui32Factor ) { m_ui32HorSharness = std::max<uint32_t>( 1, _ui32Factor ); }
		
		/**
		 * Gets the horizontal sharpness factor.
		 *
		 * \return Returns the horizontal sharpness factor.
		 */
		inline uint32_t											GetHorSharpness() const { return m_ui32HorSharness; }

		/**
		 * Gets the effective vertical sharpness factor limited by the target window height.
		 *
		 * \param _uiOutH The height of the output window.
		 * \return Returns the clamped vertical sharpness factor.
		 */
		inline uint32_t											GetActualVertSharpness( uint32_t _uiOutH ) const {
			return m_ui32RsrcH ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( _uiOutH / double( m_ui32RsrcH ) )), GetVertSharpness() ) : 1;
		}
		
		/**
		 * Gets the effective horizontal sharpness factor limited by the target window width.
		 *
		 * \param _uiOutW The width of the output window.
		 * \return Returns the clamped horizontal sharpness factor.
		 */
		inline uint32_t											GetActualHorSharpness( uint32_t _uiOutW ) const {
			return m_ui32RsrcW ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( _uiOutW / double( m_ui32RsrcW ) )), GetHorSharpness() ) : 1;
		}

		/**
		 * Sets whether to use a 16-bit floating point format for the initial render target.
		 *
		 * \param _bUse16Bit If true, uses a 16-bit float format instead of 32-bit.
		 */
		inline void												Use16Target( bool _bUse16Bit ) { m_bUse16BitInitialTarget = _bUse16Bit; }
		
		/**
		 * Enables or disables the high-quality 2-pass resampler.
		 *
		 * \param _bUse If true, enables the high-quality resampler.
		 */
		inline void												SetUseHighQualityResampler( bool _bUse ) { m_bUseHighQualityResampler = _bUse; }
		
		/**
		 * Gets whether the high-quality resampler is enabled.
		 *
		 * \return Returns true if the high-quality resampler is enabled.
		 */
		inline bool												GetUseHighQualityResampler() const { return m_bUseHighQualityResampler; }
		
		/**
		 * Sets the gamma curve to be applied in the pipeline.
		 *
		 * \param _gGamma The gamma preset.
		 */
		inline void												SetGamma( CNesPalette::LSN_GAMMA _gGamma ) { m_gGamma = _gGamma; }
		
		/**
		 * Gets the specified gamma curve for the pipeline.
		 *
		 * \return Returns the currently set gamma preset.
		 */
		inline CNesPalette::LSN_GAMMA							GetGamma() const { return m_gGamma; }
		
		/**
		 * Resolves auto-gamma settings to a concrete gamma curve.
		 *
		 * \return Returns the effective gamma curve to use during rendering.
		 */
		virtual inline CNesPalette::LSN_GAMMA					GetEffectiveGamma() const { return m_gGamma == CNesPalette::LSN_G_AUTO ? CNesPalette::LSN_G_CRT2 : m_gGamma; }
		
		/**
		 * Enables or disables the phosphor decay effect.
		 *
		 * \param _bEnable If true, phosphor decay is simulated.
		 */
		inline void												SetPhosphorDecayEnable( bool _bEnable ) { m_bEnablePhosphorDecay = _bEnable; }
		
		/**
		 * Gets whether the phosphor decay effect is enabled.
		 *
		 * \return Returns true if phosphor decay is enabled.
		 */
		inline bool												GetPhosphorDecayEnable() const { return m_bEnablePhosphorDecay; }

		/**
		 * Sets the base time period for phosphor decay calculations.
		 *
		 * \param _fTime The decay period in seconds.
		 */
		void													SetPhosphorDecayPeriod( float _fTime = 1.79113161563873291015625f ) {
			m_fPhosphorDecayTime = _fTime;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Sets the framerate used for calculating the per-frame phosphor decay multipliers.
		 *
		 * \param _fFps The framerate in frames per second.
		 */
		void													SetFps( float _fFps = 60.098812103271484375f ) {
			m_fFps = _fFps;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Sets the initial decay level applied immediately when a pixel loses intensity.
		 *
		 * \param _fLevel The decay scalar.
		 */
		void													SetPhosphorDecayLevel( float _fLevel = 0.25f ) {
			m_fInitPhosphorDecay = _fLevel;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Determines the optimal convolution filter based on the target resolution scaling.
		 *
		 * \param _ui32Width The target width.
		 * \param _ui32Height The target height.
		 * \return Returns the suggested filter function.
		 */
		virtual inline CResamplerBase::LSN_FILTER_FUNCS			GetPreferredConvolutionFilter( uint32_t _ui32Width, uint32_t _ui32Height ) {
			float fResolutionFactor = std::min( static_cast<float>(_ui32Width) / static_cast<float>(m_ui32RsrcW ? m_ui32RsrcW : 256), static_cast<float>(_ui32Height) / static_cast<float>(m_ui32RsrcH ? m_ui32RsrcH : 240) );
			if ( fResolutionFactor < 2.5 ) { return CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM; }
			if ( fResolutionFactor < 3.5 ) { return CResamplerBase::LSN_FF_ROBIDOUXSHARP; }
			return CResamplerBase::LSN_FF_LINEAR;
		}

	protected :
		// == Types.
#pragma pack( push, 1 )
		/** A standard 2D vertex with position and UV coordinates. */
		struct LSN_XYZRHWTEX1 {
			float fX;					/**< The X position. */
			float fY;					/**< The Y position. */
			float fZ;					/**< The Z position. */
			float fW;					/**< The W coordinate. */
			float fU;					/**< The U texture coordinate. */
			float fV;					/**< The V texture coordinate. */
		};
#pragma pack( pop )

		/** Global state structure for managing the shared Vulkan device and target window. */
		struct LSN_VULKAN_GLOBAL_STATE {
			lsw::CWidget *										pwParent = nullptr;			/**< The parent widget container. */
#ifdef LSN_WINDOWS
			HWND												hWndTarget = NULL;			/**< The child window handle used for Vulkan rendering on Windows. */
#else
			void *												pvTarget = nullptr;			/**< The opaque target handle for non-Windows platforms (e.g., CAMetalLayer). */
#endif
			CVulkanDevice										vkDevice;					/**< The shared Vulkan logical device. */
			int32_t												i32RefCnt = 0;				/**< The reference count tracking active filters using the device. */
			lsw::LSW_RECT										rScreenRect;				/**< The client area bounds. */
			bool												bValidState = false;		/**< True if the device and swapchain are successfully initialized. */

			/**
			 * Initializes the shared Vulkan device and swapchain.
			 *
			 * \return Returns true on success.
			 */
			bool												CreateVulkan();
			
			/**
			 * Creates the child window/view target for rendering.
			 *
			 * \return Returns true on success.
			 */
			bool												CreateTargetChild();
			
			/**
			 * Destroys the shared Vulkan device if the reference count reaches zero.
			 */
			void												DestroyVulkan();
			
			/**
			 * Updates the bounds of the child target window/view to match the parent.
			 *
			 * \return Returns true on success.
			 */
			bool												LayoutTargetChild();

			~LSN_VULKAN_GLOBAL_STATE() {
				if ( i32RefCnt ) {
					lsn::DebugA( "\r\n* * * * * LSN_VULKAN_GLOBAL_STATE::~LSN_VULKAN_GLOBAL_STATE():i32RefCnt NOT 0 * * * * *\r\n\r\n" );
				}
#ifdef LSN_WINDOWS
				if ( hWndTarget ) {
					lsn::DebugA( "\r\n* * * * * LSN_VULKAN_GLOBAL_STATE::~LSN_VULKAN_GLOBAL_STATE():hWndTarget NOT DESTROYED * * * * *\r\n\r\n" );
					::DestroyWindow( hWndTarget );
					hWndTarget = NULL;
				}
#else
				if ( pvTarget ) {
					lsn::DebugA( stderr, "\r\n* * * * * LSN_VULKAN_GLOBAL_STATE::~LSN_VULKAN_GLOBAL_STATE():pvTarget NOT DESTROYED * * * * *\r\n\r\n" );
					pvTarget = nullptr;
				}
#endif
			}
		};


		// == Members.
#ifdef LSN_WINDOWS
		static const wchar_t *									LSN_VULKAN_TARGET_CLASS;			/**< Global: window class name for the Vulkan child target (Windows only). */
#endif
		static LSN_VULKAN_GLOBAL_STATE							s_vgsState;							/**< The global Vulkan state containing the shared device. */

		CVulkanDevice *											m_pvkDevice = nullptr;				/**< Pointer to the shared Vulkan device. */

		// == Common Pipeline Components ==
		CVulkanTextureGamma										m_tgGamma;							/**< The gamma correction hardware pass. */
		CVulkanPhosphor											m_pPhosphor;						/**< The phosphor decay simulation pass. */
		CVulkanTexturePixelScaler								m_tpsScaler;						/**< The nearest-neighbor integer scaler pass. */
		CVulkanResampler										m_rsResampler;						/**< The high-quality 2-pass resampler. */
		CVulkanTextureRenderer									m_trRenderer;						/**< The final backbuffer copy pass. */

		// == Framebuffers and Memory Targets ==
		std::unique_ptr<CVulkanImage>							m_piGamma;							/**< The target image for the gamma pass. */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmGammaMemory;					/**< The memory backing the gamma image. */
		CVulkan::LSN_IMAGE_VIEW									m_ivGammaView;						/**< The image view for the gamma pass target. */
		CVulkan::LSN_FRAMEBUFFER								m_fbGamma;							/**< The framebuffer for the gamma pass. */

		std::unique_ptr<CVulkanImage>							m_piResampled;						/**< The target image for the resampler pass. */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmResampledMemory;				/**< The memory backing the resampler image. */
		CVulkan::LSN_IMAGE_VIEW									m_ivResampledView;					/**< The image view for the resampler target. */
		CVulkan::LSN_FRAMEBUFFER								m_fbResampled;						/**< The framebuffer for the resampler pass. */
		
		CVulkan::LSN_FRAMEBUFFER								m_fbScaler;							/**< The framebuffer constructed around the scaler's internal image. */

		std::vector<VkImage>									m_vSwapImages;						/**< The array of swapchain images. */
		std::vector<CVulkan::LSN_IMAGE_VIEW>					m_vSwapViews;						/**< The array of swapchain image views. */
		std::vector<CVulkan::LSN_FRAMEBUFFER>					m_vSwapFramebuffers;				/**< The array of swapchain framebuffers. */

		// == Render Passes & Descriptors ==
		CVulkan::LSN_RENDER_PASS								m_rpGammaPass;						/**< The render pass format utilized by the gamma step. */
		CVulkan::LSN_RENDER_PASS								m_rpScalerPass;						/**< The render pass format utilized by the integer scaler and resampler steps. */
		CVulkan::LSN_RENDER_PASS								m_rpBackBufferPass;					/**< The render pass format that outputs to the swapchain backbuffer. */

		CVulkan::LSN_SAMPLER									m_sPointSampler;					/**< A shared nearest-neighbor sampler. */
		CVulkan::LSN_SAMPLER									m_sLinearSampler;					/**< A shared bilinear sampler. */

		CVulkanDescriptorSetLayout								m_dslCommonLayout;					/**< The shared descriptor set layout for single-texture passes. */
		std::unique_ptr<CVulkanDescriptorPool>					m_pdpDescriptorPool;				/**< The pool allocating descriptor sets for the pipeline linkage. */
		
		CVulkan::LSN_DESCRIPTOR_SET								m_dsGammaSet;						/**< The descriptor set linking input to the gamma pass. */
		CVulkan::LSN_DESCRIPTOR_SET								m_dsScalerSet;						/**< The descriptor set linking input to the scaler pass. */
		CVulkan::LSN_DESCRIPTOR_SET								m_dsRendererSet;					/**< The descriptor set linking input to the final renderer pass. */

		// Settings...
		CNesPalette::LSN_GAMMA									m_gGamma = CNesPalette::LSN_G_AUTO;	/**< The requested gamma curve. */
		float													m_fFps = 60.098812103271484375f;	/**< The refresh rate used for timing. */
		float													m_fInitPhosphorDecay = 0.25f;		/**< The base decay scalar applied immediately. */
		float													m_fPhosphorDecayRateRed = 0.401767850f;	/**< The frame-to-frame red decay multiplier. */
		float													m_fPhosphorDecayRateGreen = 0.663420439f;	/**< The frame-to-frame green decay multiplier. */
		float													m_fPhosphorDecayRateBlue = 0.193711475f;	/**< The frame-to-frame blue decay multiplier. */
		float													m_fPhosphorDecayTime = 1.79113161563873291015625f;	/**< The decay curve half-life time base. */
		bool													m_bEnablePhosphorDecay = true;		/**< True if the phosphor pass should execute. */

		uint32_t												m_ui32RsrcW = 0;					/**< The base tracked width of the source textures. */
		uint32_t												m_ui32RsrcH = 0;					/**< The base tracked height of the source textures. */
		uint32_t												m_ui32ResampledTargetW = 0;			/**< The tracked width of the resampler target. */
		uint32_t												m_ui32ResampledTargetH = 0;			/**< The tracked height of the resampler target. */
		uint32_t												m_ui32VertSharpness = 3;			/**< The vertical prescale integer factor. */
		uint32_t												m_ui32HorSharness = 1;				/**< The horizontal prescale integer factor. */
		bool													m_bUse16BitInitialTarget = true;	/**< Uses 16-bit intermediate formats if true. */
		bool													m_bUseHighQualityResampler = true;	/**< Executes the 2-pass convolution resampler if true. */


		// == Functions.
		/**
		 * Registers the custom Win32 child window class used for the rendering surface.
		 *
		 * \return Returns true if the class registered successfully.
		 */
		static bool LSN_FASTCALL								RegisterVulkanTargetClass();
		
		/**
		 * Triggers a layout update and swapchain resize.
		 *
		 * \return Returns true on success.
		 */
		static bool												OnSizeVulkan();

		/**
		 * Ensures that the intermediate images and framebuffers are sized correctly to match the incoming data.
		 *
		 * \param _pvkDevice The active Vulkan device.
		 * \param _ui32NativeW The unscaled native output width.
		 * \param _ui32NativeH The unscaled native output height.
		 * \return Returns true on success.
		 */
		bool													EnsureBaseSizeAndResources( CVulkanDevice * _pvkDevice, uint32_t _ui32NativeW, uint32_t _ui32NativeH );
		
		/**
		 * Releases base intermediate resources depending on the native source size.
		 */
		void													ReleaseBaseSizeDependents();
		
		/**
		 * Releases resources tied directly to the backbuffer count and extent of the swapchain.
		 */
		void													ReleaseSwapchainResources();

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
		bool													RenderBase( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandBuffer, VkImageView _ivSrc, VkSampler _sSrcSampler, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, uint32_t _ui32ImageIndex, bool _bFlipY = false );

		/**
		 * Helper to locate an appropriate memory type index for device or host allocations.
		 *
		 * \param _pdDevice The Vulkan physical device.
		 * \param _ui32TypeFilter The acceptable memory types bitmask.
		 * \param _mpfProperties The requested memory property flags.
		 * \return Returns the index of the memory type, or 0 if none is found.
		 */
		uint32_t												FindMemoryType( VkPhysicalDevice _pdDevice, uint32_t _ui32TypeFilter, VkMemoryPropertyFlags _mpfProperties );
		
		/**
		 * Helper to construct a standard rendering pass format layout.
		 *
		 * \param _fFormat The image format expected for the attachment.
		 * \param _rpPass The wrapper object receiving the created render pass.
		 * \param _bIsBackBuffer True if the layout should transition to PRESENT_SRC_KHR at the end of the pass.
		 * \return Returns true if the pass created successfully.
		 */
		bool													CreateRenderPass( VkFormat _fFormat, CVulkan::LSN_RENDER_PASS &_rpPass, bool _bIsBackBuffer = false );
		
		/**
		 * Binds an image view and a sampler dynamically to the specified descriptor set.
		 *
		 * \param _dsSet The target descriptor set to modify.
		 * \param _ivView The image view to bind to binding 0.
		 * \param _sSampler The sampler to bind to binding 0.
		 */
		void													UpdateDescriptorSet( VkDescriptorSet _dsSet, VkImageView _ivView, VkSampler _sSampler );

		/**
		 * Creates the point sampler, m_sPointSampler.
		 * 
		 * \param _dDevice The Vulkan device.
		 * \return Returns true if creating the point sampler m_sPointSampler succeeded.
		 **/
		bool													CreateSamplers( VkDevice _dDevice );

		/**
		 * \brief Compiles a GLSL shader to SPIR-V using the Vulkan SDK's glslc command line tool.
		 *
		 * \param _pcszSource Null-terminated GLSL source code.
		 * \param _pcszStage The shader stage (e.g., "vertex" or "fragment").
		 * \param _vOutByteCode Output vector to receive the compiled SPIR-V bytecode.
		 * \return Returns true if compilation succeeded and bytecode was produced.
		 */
		static bool												CompileGlslToSpirv( const char * _pcszSource, const char * _pcszStage, std::vector<uint32_t> &_vOutByteCode );
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
