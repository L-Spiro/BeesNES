/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles all emulator functionality.
 */

#include "LSNBeesNes.h"
#include "../Cheats/LSNCheatManager.h"
#include "../File/LSNStdFile.h"
#include "../Filters/LSNGpuFilterBase.h"
#include "../Peripherals/LSNStdController.h"

#include <cmath>
#include <ctime>
#include <cwchar>
#include <filesystem>
#include <format>


namespace lsn {

	/** The tag that starts the video-filter section of the settings files ("LSVF"). */
	static constexpr uint32_t LSN_VIDEO_FILTER_SECTION_TAG = 0x4656534C;

	/** The layout version of the video-filter section of the settings files. */
	static constexpr uint32_t LSN_VIDEO_FILTER_SECTION_VERSION = 2;

	/** The tag that starts the host-display section of the settings file ("LSHD"). */
	static constexpr uint32_t LSN_HOST_DISPLAY_SECTION_TAG = 0x4448534C;

	/** The layout version of the host-display section of the settings file. */
	static constexpr uint32_t LSN_HOST_DISPLAY_SECTION_VERSION = 1;

	/** The raw WAV stream. */
	CWavFile CBeesNes::m_wfRawStream;
	
	/** The output-capture WAV stream. */
	CWavFile CBeesNes::m_wfOutStream;

	CBeesNes::CBeesNes( CDisplayHost * _pdhDisplayHost, CInputPoller * _pipPoller ) :
		m_dScale( 3.0 ),
		m_dRatio( 4.0 / 3.0 ),
		m_dRatioActual( 4.0 / 3.0 ),
		m_pdhDisplayHost( _pdhDisplayHost ),
		m_pipPoller( _pipPoller ),
		m_ui32RecentLimit( 13 * 4 ),
		m_hmHotkeyManager( 0x1000 ) {
		
		m_cmCheatManager.InitializeCheatLibrary( DefaultCheatsFolder() + L"Cheats.zip", &m_hmHotkeyManager );

		//CUtilities::GenGaussianNoise( 0.0225f );
		//m_trTemperatureReader.SetBuffer( reinterpret_cast<float *>(CUtilities::m_fNoiseBuffers), std::size( CUtilities::m_fNoiseBuffers ) );
		CAudio::SetOutputSettings( Options().aoGlobalAudioOptions );
		
		//m_pfbFilterTable
		CFilterBase * pfbTmp[CFilterBase::LSN_F_TOTAL][LSN_PM_CONSOLE_TOTAL] = {
			//LSN_PM_NTSC_M							LSN_PM_NTSC_J							LSN_PM_PAL								LSN_PM_DENDY							LSN_PM_PALM								LSN_PM_PALN
			{ &m_r24fRgb24Filter,					&m_r24fRgb24Filter,						&m_r24fRgb24Filter,						&m_r24fRgb24Filter,						&m_r24fRgb24Filter,						&m_r24fRgb24Filter },					// LSN_F_INDEXED
			{ &m_nbfBlarggNtscFilter,				&m_nbfBlarggNtscFilter,					&m_nbfBlarggNtscFilter,					&m_nbfBlarggNtscFilter,					&m_nbfBlarggNtscFilter,					&m_nbfBlarggNtscFilter },				// LSN_F_NTSC_BLARGG
			{ &m_nbfLSpiroNtscFilter,				&m_nbfLSpiroNtscFilter,					&m_nbfLSpiroNtscFilter,					&m_nbfLSpiroNtscFilter,					&m_nbfLSpiroNtscFilter,					&m_nbfLSpiroNtscFilter },				// LSN_F_NTSC_LSPIRO
			{ &m_nbfLSpiroPalFilter,				&m_nbfLSpiroPalFilter,					&m_nbfLSpiroPalFilter,					&m_nbfLSpiroPalFilter,					&m_nbfLSpiroPalFilter,					&m_nbfLSpiroPalFilter },				// LSN_F_PAL_LSPIRO
			{ &m_nbfLSpiroDendyFilter,				&m_nbfLSpiroDendyFilter,				&m_nbfLSpiroDendyFilter,				&m_nbfLSpiroDendyFilter,				&m_nbfLSpiroDendyFilter,				&m_nbfLSpiroDendyFilter },				// LSN_F_DENDY_LSPIRO
			{ &m_nbfLSpiroPalMFilter,				&m_nbfLSpiroPalMFilter,					&m_nbfLSpiroPalMFilter,					&m_nbfLSpiroPalMFilter,					&m_nbfLSpiroPalMFilter,					&m_nbfLSpiroPalMFilter },				// LSN_F_PALM_LSPIRO
			{ &m_nbfLSpiroPalNFilter,				&m_nbfLSpiroPalNFilter,					&m_nbfLSpiroPalNFilter,					&m_nbfLSpiroPalNFilter,					&m_nbfLSpiroPalNFilter,					&m_nbfLSpiroPalNFilter },				// LSN_F_PALN_LSPIRO
			{ &m_ncfEmmirNtscFullFilter,			&m_ncfEmmirNtscFullFilter,				&m_ncfEmmirNtscFullFilter,				&m_ncfEmmirNtscFullFilter,				&m_ncfEmmirNtscFullFilter,				&m_ncfEmmirNtscFullFilter },			// LSN_F_NTSC_CRT_FULL
			{ &m_ncfEmmirPalFullFilter,				&m_ncfEmmirPalFullFilter,				&m_ncfEmmirPalFullFilter,				&m_ncfEmmirPalFullFilter,				&m_ncfEmmirPalFullFilter,				&m_ncfEmmirPalFullFilter },				// LSN_F_PAL_CRT_FULL
			{ &m_ncfEmmirNtscFullFilter,			&m_ncfEmmirNtscFullFilter,				&m_ncfEmmirPalFullFilter,				&m_ncfEmmirDendyFullFilter,				&m_ncfEmmirPalFullFilter,				&m_ncfEmmirPalFullFilter },				// LSN_F_AUTO_CRT_FULL
			{ &m_nbfLSpiroNtscFilter,				&m_nbfLSpiroNtscFilter,					&m_nbfLSpiroPalFilter,					&m_nbfLSpiroDendyFilter,				&m_nbfLSpiroPalMFilter,					&m_nbfLSpiroPalNFilter },				// LSN_F_AUTO_LSPIRO
			{ &m_ncfEmmirDendyFullFilter,			&m_ncfEmmirDendyFullFilter,			&m_ncfEmmirDendyFullFilter,			&m_ncfEmmirDendyFullFilter,			&m_ncfEmmirDendyFullFilter,			&m_ncfEmmirDendyFullFilter },		// LSN_F_DENDY_CRT_FULL
#ifdef LSN_DX9
			{ &m_d9pfDx9Palette,					&m_d9pfDx9Palette,						&m_d9pfDx9Palette,						&m_d9pfDx9Palette,						&m_d9pfDx9Palette,						&m_d9pfDx9Palette },					// LSN_F_INDEXED_DX9
			{ &m_d9nbfDx9BlarggNtscFilter,			&m_d9nbfDx9BlarggNtscFilter,			&m_d9nbfDx9BlarggNtscFilter,			&m_d9nbfDx9BlarggNtscFilter,			&m_d9nbfDx9BlarggNtscFilter,			&m_d9nbfDx9BlarggNtscFilter },			// LSN_F_NTSC_BLARGG_DX9
			{ &m_d9ncfDx9EmmirNtscFullFilter,		&m_d9ncfDx9EmmirNtscFullFilter,			&m_d9ncfDx9EmmirNtscFullFilter,			&m_d9ncfDx9EmmirNtscFullFilter,			&m_d9ncfDx9EmmirNtscFullFilter,			&m_d9ncfDx9EmmirNtscFullFilter },		// LSN_F_NTSC_CRT_FULL_DX9
			{ &m_d9ncfDx9EmmirPalFullFilter,		&m_d9ncfDx9EmmirPalFullFilter,			&m_d9ncfDx9EmmirPalFullFilter,			&m_d9ncfDx9EmmirPalFullFilter,			&m_d9ncfDx9EmmirPalFullFilter,			&m_d9ncfDx9EmmirPalFullFilter },		// LSN_F_PAL_CRT_FULL_DX9

			{ &m_d9nlsfDx9LSpiroNtsc,				&m_d9nlsfDx9LSpiroNtsc,					&m_d9nlsfDx9LSpiroNtsc,					&m_d9nlsfDx9LSpiroNtsc,					&m_d9nlsfDx9LSpiroNtsc,					&m_d9nlsfDx9LSpiroNtsc },				// LSN_F_LSPIRONTSC_DX9

			{ &m_d9plsfDx9LSpiroPal,				&m_d9plsfDx9LSpiroPal,					&m_d9plsfDx9LSpiroPal,					&m_d9plsfDx9LSpiroPal,					&m_d9plsfDx9LSpiroPal,					&m_d9plsfDx9LSpiroPal },				// LSN_F_LSPIROPAL_DX9
			{ &m_d9plsfDx9LSpiroDendy,				&m_d9plsfDx9LSpiroDendy,				&m_d9plsfDx9LSpiroDendy,				&m_d9plsfDx9LSpiroDendy,				&m_d9plsfDx9LSpiroDendy,				&m_d9plsfDx9LSpiroDendy },				// LSN_F_LSPIRODENDY_DX9
			{ &m_d9plsfDx9LSpiroPalM,				&m_d9plsfDx9LSpiroPalM,					&m_d9plsfDx9LSpiroPalM,					&m_d9plsfDx9LSpiroPalM,					&m_d9plsfDx9LSpiroPalM,					&m_d9plsfDx9LSpiroPalM },				// LSN_F_LSPIROPALM_DX9
			{ &m_d9plsfDx9LSpiroPalN,				&m_d9plsfDx9LSpiroPalN,					&m_d9plsfDx9LSpiroPalN,					&m_d9plsfDx9LSpiroPalN,					&m_d9plsfDx9LSpiroPalN,					&m_d9plsfDx9LSpiroPalN },				// LSN_F_LSPIROPALN_DX9
			
			{ &m_d9ncfDx9EmmirNtscFullFilter,		&m_d9ncfDx9EmmirNtscJFullFilter,		&m_d9ncfDx9EmmirPalFullFilter,			&m_d9ncfDx9EmmirDendyFullFilter,			&m_d9ncfDx9EmmirPalFullFilter,			&m_d9ncfDx9EmmirPalFullFilter },		// LSN_F_AUTO_CRT_FULL_DX9
			{ &m_d9nlsfDx9LSpiroNtsc,				&m_d9nlsfDx9LSpiroNtscJ,				&m_d9plsfDx9LSpiroPal,					&m_d9plsfDx9LSpiroDendy,				&m_d9plsfDx9LSpiroPalM,					&m_d9plsfDx9LSpiroPalN },				// LSN_F_LSPIRO_AUTO_DX9

			{ &m_d9nbfDx9BlarggNtscJFilter,			&m_d9nbfDx9BlarggNtscJFilter,			&m_d9nbfDx9BlarggNtscJFilter,			&m_d9nbfDx9BlarggNtscJFilter,			&m_d9nbfDx9BlarggNtscJFilter,			&m_d9nbfDx9BlarggNtscJFilter },			// LSN_F_NTSC_J_BLARGG_DX9
			{ &m_d9ncfDx9EmmirNtscJFullFilter,		&m_d9ncfDx9EmmirNtscJFullFilter,		&m_d9ncfDx9EmmirNtscJFullFilter,		&m_d9ncfDx9EmmirNtscJFullFilter,		&m_d9ncfDx9EmmirNtscJFullFilter,		&m_d9ncfDx9EmmirNtscJFullFilter },		// LSN_F_NTSC_J_CRT_FULL_DX9
			{ &m_d9nlsfDx9LSpiroNtscJ,				&m_d9nlsfDx9LSpiroNtscJ,				&m_d9nlsfDx9LSpiroNtscJ,				&m_d9nlsfDx9LSpiroNtscJ,				&m_d9nlsfDx9LSpiroNtscJ,				&m_d9nlsfDx9LSpiroNtscJ },				// LSN_F_LSPIRONTSC_J_DX9
			{ &m_d9ncfDx9EmmirDendyFullFilter,	&m_d9ncfDx9EmmirDendyFullFilter,	&m_d9ncfDx9EmmirDendyFullFilter,	&m_d9ncfDx9EmmirDendyFullFilter,	&m_d9ncfDx9EmmirDendyFullFilter,	&m_d9ncfDx9EmmirDendyFullFilter },	// LSN_F_DENDY_CRT_FULL_DX9
#endif	// #ifdef LSN_DX9
#ifdef LSN_DX12
			{ &m_d12pfDx12Palette,					&m_d12pfDx12Palette,					&m_d12pfDx12Palette,					&m_d12pfDx12Palette,					&m_d12pfDx12Palette,					&m_d12pfDx12Palette },					// LSN_F_INDEXED_DX12
			{ &m_d12nbfDx12BlarggNtscFilter,		&m_d12nbfDx12BlarggNtscFilter,			&m_d12nbfDx12BlarggNtscFilter,			&m_d12nbfDx12BlarggNtscFilter,			&m_d12nbfDx12BlarggNtscFilter,			&m_d12nbfDx12BlarggNtscFilter },		// LSN_F_NTSC_BLARGG_DX12
			{ &m_d12ncfDx12EmmirNtscFullFilter,		&m_d12ncfDx12EmmirNtscFullFilter,		&m_d12ncfDx12EmmirNtscFullFilter,		&m_d12ncfDx12EmmirNtscFullFilter,		&m_d12ncfDx12EmmirNtscFullFilter,		&m_d12ncfDx12EmmirNtscFullFilter },		// LSN_F_NTSC_CRT_FULL_DX12
			{ &m_d12ncfDx12EmmirPalFullFilter,		&m_d12ncfDx12EmmirPalFullFilter,		&m_d12ncfDx12EmmirPalFullFilter,		&m_d12ncfDx12EmmirPalFullFilter,		&m_d12ncfDx12EmmirPalFullFilter,		&m_d12ncfDx12EmmirPalFullFilter },		// LSN_F_PAL_CRT_FULL_DX12

			{ &m_d12nlsfDx12LSpiroNtsc,				&m_d12nlsfDx12LSpiroNtsc,				&m_d12nlsfDx12LSpiroNtsc,				&m_d12nlsfDx12LSpiroNtsc,				&m_d12nlsfDx12LSpiroNtsc,				&m_d12nlsfDx12LSpiroNtsc },				// LSN_F_LSPIRONTSC_DX12

			{ &m_d12plsfDx12LSpiroPal,				&m_d12plsfDx12LSpiroPal,				&m_d12plsfDx12LSpiroPal,				&m_d12plsfDx12LSpiroPal,				&m_d12plsfDx12LSpiroPal,				&m_d12plsfDx12LSpiroPal },				// LSN_F_LSPIROPAL_DX12
			{ &m_d12plsfDx12LSpiroDendy,			&m_d12plsfDx12LSpiroDendy,				&m_d12plsfDx12LSpiroDendy,				&m_d12plsfDx12LSpiroDendy,				&m_d12plsfDx12LSpiroDendy,				&m_d12plsfDx12LSpiroDendy },			// LSN_F_LSPIRODENDY_DX12
			{ &m_d12plsfDx12LSpiroPalM,				&m_d12plsfDx12LSpiroPalM,				&m_d12plsfDx12LSpiroPalM,				&m_d12plsfDx12LSpiroPalM,				&m_d12plsfDx12LSpiroPalM,				&m_d12plsfDx12LSpiroPalM },				// LSN_F_LSPIROPALM_DX12
			{ &m_d12plsfDx12LSpiroPalN,				&m_d12plsfDx12LSpiroPalN,				&m_d12plsfDx12LSpiroPalN,				&m_d12plsfDx12LSpiroPalN,				&m_d12plsfDx12LSpiroPalN,				&m_d12plsfDx12LSpiroPalN },				// LSN_F_LSPIROPALN_DX12

			{ &m_d12ncfDx12EmmirNtscFullFilter,		&m_d12ncfDx12EmmirNtscJFullFilter,		&m_d12ncfDx12EmmirPalFullFilter,		&m_d12ncfDx12EmmirDendyFullFilter,		&m_d12ncfDx12EmmirPalFullFilter,		&m_d12ncfDx12EmmirPalFullFilter },		// LSN_F_AUTO_CRT_FULL_DX12
			{ &m_d12nlsfDx12LSpiroNtsc,				&m_d12nlsfDx12LSpiroNtscJ,				&m_d12plsfDx12LSpiroPal,				&m_d12plsfDx12LSpiroDendy,				&m_d12plsfDx12LSpiroPalM,				&m_d12plsfDx12LSpiroPalN },				// LSN_F_LSPIRO_AUTO_DX12

			{ &m_d12nbfDx12BlarggNtscJFilter,		&m_d12nbfDx12BlarggNtscJFilter,			&m_d12nbfDx12BlarggNtscJFilter,			&m_d12nbfDx12BlarggNtscJFilter,			&m_d12nbfDx12BlarggNtscJFilter,			&m_d12nbfDx12BlarggNtscJFilter },		// LSN_F_NTSC_J_BLARGG_DX12
			{ &m_d12ncfDx12EmmirNtscJFullFilter,	&m_d12ncfDx12EmmirNtscJFullFilter,		&m_d12ncfDx12EmmirNtscJFullFilter,		&m_d12ncfDx12EmmirNtscJFullFilter,		&m_d12ncfDx12EmmirNtscJFullFilter,		&m_d12ncfDx12EmmirNtscJFullFilter },	// LSN_F_NTSC_J_CRT_FULL_DX12
			{ &m_d12nlsfDx12LSpiroNtscJ,			&m_d12nlsfDx12LSpiroNtscJ,				&m_d12nlsfDx12LSpiroNtscJ,				&m_d12nlsfDx12LSpiroNtscJ,				&m_d12nlsfDx12LSpiroNtscJ,				&m_d12nlsfDx12LSpiroNtscJ },			// LSN_F_LSPIRONTSC_J_DX12
			{ &m_d12ncfDx12EmmirDendyFullFilter,	&m_d12ncfDx12EmmirDendyFullFilter,	&m_d12ncfDx12EmmirDendyFullFilter,	&m_d12ncfDx12EmmirDendyFullFilter,	&m_d12ncfDx12EmmirDendyFullFilter,	&m_d12ncfDx12EmmirDendyFullFilter },	// LSN_F_DENDY_CRT_FULL_DX12
#endif	// #ifdef LSN_DX12
#ifdef LSN_VULKAN1
			{ &m_vpfVulkanPalette,					&m_vpfVulkanPalette,					&m_vpfVulkanPalette,					&m_vpfVulkanPalette,					&m_vpfVulkanPalette,					&m_vpfVulkanPalette },					// LSN_F_INDEXED_VULKAN1
			{ &m_vnbfVulkanBlarggNtscFilter,		&m_vnbfVulkanBlarggNtscFilter,			&m_vnbfVulkanBlarggNtscFilter,			&m_vnbfVulkanBlarggNtscFilter,			&m_vnbfVulkanBlarggNtscFilter,			&m_vnbfVulkanBlarggNtscFilter },		// LSN_F_NTSC_BLARGG_VULKAN1
			{ &m_vncfVulkanEmmirNtscFullFilter,		&m_vncfVulkanEmmirNtscFullFilter,		&m_vncfVulkanEmmirNtscFullFilter,		&m_vncfVulkanEmmirNtscFullFilter,		&m_vncfVulkanEmmirNtscFullFilter,		&m_vncfVulkanEmmirNtscFullFilter },		// LSN_F_NTSC_CRT_FULL_VULKAN1
			{ &m_vncfVulkanEmmirPalFullFilter,		&m_vncfVulkanEmmirPalFullFilter,		&m_vncfVulkanEmmirPalFullFilter,		&m_vncfVulkanEmmirPalFullFilter,		&m_vncfVulkanEmmirPalFullFilter,		&m_vncfVulkanEmmirPalFullFilter },		// LSN_F_PAL_CRT_FULL_VULKAN1

			{ &m_vnlsfVulkanLSpiroNtsc,				&m_vnlsfVulkanLSpiroNtsc,				&m_vnlsfVulkanLSpiroNtsc,				&m_vnlsfVulkanLSpiroNtsc,				&m_vnlsfVulkanLSpiroNtsc,				&m_vnlsfVulkanLSpiroNtsc },				// LSN_F_LSPIRONTSC_VULKAN1

			{ &m_vplsfVulkanLSpiroPal,				&m_vplsfVulkanLSpiroPal,				&m_vplsfVulkanLSpiroPal,				&m_vplsfVulkanLSpiroPal,				&m_vplsfVulkanLSpiroPal,				&m_vplsfVulkanLSpiroPal },				// LSN_F_LSPIROPAL_VULKAN1
			{ &m_vplsfVulkanLSpiroDendy,			&m_vplsfVulkanLSpiroDendy,				&m_vplsfVulkanLSpiroDendy,				&m_vplsfVulkanLSpiroDendy,				&m_vplsfVulkanLSpiroDendy,				&m_vplsfVulkanLSpiroDendy },			// LSN_F_LSPIRODENDY_VULKAN1
			{ &m_vplsfVulkanLSpiroPalM,				&m_vplsfVulkanLSpiroPalM,				&m_vplsfVulkanLSpiroPalM,				&m_vplsfVulkanLSpiroPalM,				&m_vplsfVulkanLSpiroPalM,				&m_vplsfVulkanLSpiroPalM },				// LSN_F_LSPIROPALM_VULKAN1
			{ &m_vplsfVulkanLSpiroPalN,				&m_vplsfVulkanLSpiroPalN,				&m_vplsfVulkanLSpiroPalN,				&m_vplsfVulkanLSpiroPalN,				&m_vplsfVulkanLSpiroPalN,				&m_vplsfVulkanLSpiroPalN },				// LSN_F_LSPIROPALN_VULKAN1

			{ &m_vncfVulkanEmmirNtscFullFilter,		&m_vncfVulkanEmmirNtscJFullFilter,		&m_vncfVulkanEmmirPalFullFilter,		&m_vncfVulkanEmmirDendyFullFilter,		&m_vncfVulkanEmmirPalFullFilter,		&m_vncfVulkanEmmirPalFullFilter },		// LSN_F_AUTO_CRT_FULL_VULKAN1
			{ &m_vnlsfVulkanLSpiroNtsc,				&m_vnlsfVulkanLSpiroNtscJ,				&m_vplsfVulkanLSpiroPal,				&m_vplsfVulkanLSpiroDendy,				&m_vplsfVulkanLSpiroPalM,				&m_vplsfVulkanLSpiroPalN },				// LSN_F_LSPIRO_AUTO_VULKAN1

			{ &m_vnbfVulkanBlarggNtscJFilter,		&m_vnbfVulkanBlarggNtscJFilter,			&m_vnbfVulkanBlarggNtscJFilter,			&m_vnbfVulkanBlarggNtscJFilter,			&m_vnbfVulkanBlarggNtscJFilter,			&m_vnbfVulkanBlarggNtscJFilter },		// LSN_F_NTSC_J_BLARGG_VULKAN1
			{ &m_vncfVulkanEmmirNtscJFullFilter,	&m_vncfVulkanEmmirNtscJFullFilter,		&m_vncfVulkanEmmirNtscJFullFilter,		&m_vncfVulkanEmmirNtscJFullFilter,		&m_vncfVulkanEmmirNtscJFullFilter,		&m_vncfVulkanEmmirNtscJFullFilter },	// LSN_F_NTSC_J_CRT_FULL_VULKAN1
			{ &m_vnlsfVulkanLSpiroNtscJ,			&m_vnlsfVulkanLSpiroNtscJ,				&m_vnlsfVulkanLSpiroNtscJ,				&m_vnlsfVulkanLSpiroNtscJ,				&m_vnlsfVulkanLSpiroNtscJ,				&m_vnlsfVulkanLSpiroNtscJ },			// LSN_F_LSPIRONTSC_J_VULKAN1
			{ &m_vncfVulkanEmmirDendyFullFilter,	&m_vncfVulkanEmmirDendyFullFilter,	&m_vncfVulkanEmmirDendyFullFilter,	&m_vncfVulkanEmmirDendyFullFilter,	&m_vncfVulkanEmmirDendyFullFilter,	&m_vncfVulkanEmmirDendyFullFilter },	// LSN_F_DENDY_CRT_FULL_VULKAN1
#endif	// #ifdef LSN_VULKAN1
		};
		// FPS settings (these come from the hardware, not the user).
#ifdef LSN_DX9
		m_d9nlsfDx9LSpiroNtsc.CDx9FilterBase::SetFps( 60.098813897440515529533511098629f );
		m_d9nlsfDx9LSpiroNtscJ.CDx9FilterBase::SetFps( 60.098813897440515529533511098629f );
		m_d9plsfDx9LSpiroPal.CDx9FilterBase::SetFps( 50.006978908188585607940446650124f );
		m_d9plsfDx9LSpiroDendy.CDx9FilterBase::SetFps( 50.006978908188585607940446650124f );
		m_d9plsfDx9LSpiroPalM.CDx9FilterBase::SetFps( 60.032435273083568398202053145976f );
		m_d9plsfDx9LSpiroPalN.CDx9FilterBase::SetFps( 50.502710495150011279043537108053f );
		m_d9ncfDx9EmmirPalFullFilter.SetFps( 50.006978908188585607940446650124f );
		m_d9ncfDx9EmmirDendyFullFilter.SetFps( 50.006978908188585607940446650124f );
#endif	// #ifdef LSN_DX9
#ifdef LSN_DX12
		m_d12nlsfDx12LSpiroNtsc.CDx12FilterBase::SetFps( 60.098813897440515529533511098629f );
		m_d12nlsfDx12LSpiroNtscJ.CDx12FilterBase::SetFps( 60.098813897440515529533511098629f );
		m_d12plsfDx12LSpiroPal.CDx12FilterBase::SetFps( 50.006978908188585607940446650124f );
		m_d12plsfDx12LSpiroDendy.CDx12FilterBase::SetFps( 50.006978908188585607940446650124f );
		m_d12plsfDx12LSpiroPalM.CDx12FilterBase::SetFps( 60.032435273083568398202053145976f );
		m_d12plsfDx12LSpiroPalN.CDx12FilterBase::SetFps( 50.502710495150011279043537108053f );
		m_d12ncfDx12EmmirPalFullFilter.SetFps( 50.006978908188585607940446650124f );
		m_d12ncfDx12EmmirDendyFullFilter.SetFps( 50.006978908188585607940446650124f );
#endif	// #ifdef LSN_DX12
#ifdef LSN_VULKAN1
		m_vnlsfVulkanLSpiroNtsc.CVulkanFilterBase::SetFps( 60.098813897440515529533511098629f );
		m_vnlsfVulkanLSpiroNtscJ.CVulkanFilterBase::SetFps( 60.098813897440515529533511098629f );
		m_vplsfVulkanLSpiroPal.CVulkanFilterBase::SetFps( 50.006978908188585607940446650124f );
		m_vplsfVulkanLSpiroDendy.CVulkanFilterBase::SetFps( 50.006978908188585607940446650124f );
		m_vplsfVulkanLSpiroPalM.CVulkanFilterBase::SetFps( 60.032435273083568398202053145976f );
		m_vplsfVulkanLSpiroPalN.CVulkanFilterBase::SetFps( 50.502710495150011279043537108053f );
		m_vncfVulkanEmmirPalFullFilter.SetFps( 50.006978908188585607940446650124f );
		m_vncfVulkanEmmirDendyFullFilter.SetFps( 50.006978908188585607940446650124f );
#endif	// #ifdef LSN_VULKAN1
		m_nbfLSpiroNtscFilter.SetFps( 60.098813897440515529533511098629f );
		m_nbfLSpiroPalFilter.SetFps( 50.006978908188585607940446650124f );
		m_nbfLSpiroDendyFilter.SetFps( 50.006978908188585607940446650124f );
		m_nbfLSpiroPalMFilter.SetFps( 60.032435273083568398202053145976f );
		m_nbfLSpiroPalNFilter.SetFps( 50.502710495150011279043537108053f );

		// Everything else the user can adjust comes from the options (LSN_VIDEO_FILTER_OPTIONS), whose defaults are the settings that used to
		//	be hard-coded here.
		ApplyFilterOptions();

		static_assert( sizeof( m_pfbFilterTable ) == sizeof( pfbTmp ) );
		std::memcpy( m_pfbFilterTable, pfbTmp, sizeof( pfbTmp ) );

		m_pppbPostTable[CPostProcessBase::LSN_PP_NONE] = &m_ppbNoPostProcessing;
		m_pppbPostTable[CPostProcessBase::LSN_PP_BLEED] = &m_bppBleedPostProcessing;
		m_pppbPostTable[CPostProcessBase::LSN_PP_BILINEAR] = &m_blppBiLinearPost;
		m_pppbPostTable[CPostProcessBase::LSN_PP_SRGB] = &m_sppLinearTosRGBPost;
		//m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_BLEED );
		m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_BILINEAR );
		//m_vPostProcesses.push_back( CPostProcessBase::LSN_PP_SRGB );


		m_psbSystems[LSN_PM_NTSC_M] = &m_nsNtscSystem;
		m_psbSystems[LSN_PM_NTSC_J] = &m_nsNtscJSystem;
		m_psbSystems[LSN_PM_PAL] = &m_nsPalSystem;
		m_psbSystems[LSN_PM_DENDY] = &m_nsDendySystem;
		m_psbSystems[LSN_PM_PALM] = &m_nsPalMSystem;
		m_psbSystems[LSN_PM_PALN] = &m_nsPalNSystem;

		//CNesPalette								m_npPalettes[LSN_PM_CONSOLE_TOTAL]


		m_psbSystem = m_psbSystems[m_pmSystem];


		const size_t stBuffers = 3;
		m_r24fRgb24Filter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroPalFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroDendyFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroPalMFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfLSpiroPalNFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_nbfBlarggNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirNtscFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirPalFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_ncfEmmirDendyFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );

#ifdef LSN_DX9
		m_d9pfDx9Palette.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9nbfDx9BlarggNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9nbfDx9BlarggNtscJFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9ncfDx9EmmirNtscFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9ncfDx9EmmirNtscJFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9ncfDx9EmmirPalFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9ncfDx9EmmirDendyFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9nlsfDx9LSpiroNtsc.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9nlsfDx9LSpiroNtscJ.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );

		m_d9plsfDx9LSpiroPal.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9plsfDx9LSpiroDendy.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9plsfDx9LSpiroPalM.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d9plsfDx9LSpiroPalN.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
#endif	// #ifdef LSN_DX9

#ifdef LSN_DX12
		m_d12pfDx12Palette.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12nbfDx12BlarggNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12nbfDx12BlarggNtscJFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12ncfDx12EmmirNtscFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12ncfDx12EmmirNtscJFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12ncfDx12EmmirPalFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12ncfDx12EmmirDendyFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12nlsfDx12LSpiroNtsc.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12nlsfDx12LSpiroNtscJ.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );

		m_d12plsfDx12LSpiroPal.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12plsfDx12LSpiroDendy.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12plsfDx12LSpiroPalM.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_d12plsfDx12LSpiroPalN.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
#endif	// #ifdef LSN_DX12

#ifdef LSN_VULKAN1
		m_vpfVulkanPalette.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vnbfVulkanBlarggNtscFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vnbfVulkanBlarggNtscJFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vncfVulkanEmmirNtscFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vncfVulkanEmmirNtscJFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vncfVulkanEmmirPalFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vncfVulkanEmmirDendyFullFilter.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vnlsfVulkanLSpiroNtsc.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vnlsfVulkanLSpiroNtscJ.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );

		m_vplsfVulkanLSpiroPal.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vplsfVulkanLSpiroDendy.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vplsfVulkanLSpiroPalM.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
		m_vplsfVulkanLSpiroPalN.Init( stBuffers, uint16_t( RenderTargetWidth() ), uint16_t( RenderTargetHeight() ) );
#endif	// #ifdef LSN_VULKAN1

		// The NTSC-J (Famicom) GPU filters get the color space of a Japanese television (with its hue rotation) from their default options
		//	(see LSN_FILTER_REGION_OPTIONS::Defaults()), which ApplyFilterOptions() applied above.

		UpdateCurrentSystem();
	}
	CBeesNes::~CBeesNes() {
		m_cmCheatManager.DestroyCheatLibrary();
	}

	// == Functions.
	/**
	 * Loads the settings file.
	 *
	 * \return Returns true if the settings file was loaded.
	 */
	bool CBeesNes::LoadSettings() {
		std::wstring wsPath = m_wsFolder + L"Settings.lsn";
		CStdFile sfFile;
		if ( sfFile.Open( reinterpret_cast<const char16_t *>(wsPath.c_str()) ) ) {
			std::vector<uint8_t> vFile;
			if ( sfFile.LoadToMemory( vFile ) ) {
				sfFile.Close();
				CStream sStream( vFile );
				bool bRet = LoadSettings( sStream );
				ApplyFilterOptions();
				return bRet;
			}
		}
		return false;
	}

	/**
	 * Saves the settings file.
	 *
	 * \return Returns true if the settings file was created.
	 */
	bool CBeesNes::SaveSettings() {
		std::vector<uint8_t> vFile;
		CStream sStream( vFile );
		if ( !SaveSettings( sStream ) ) { return false; }
		std::wstring wsPath = m_wsFolder + L"Settings.lsn";
		CStdFile sfFile;
		if ( !sfFile.Create( reinterpret_cast<const char16_t *>(wsPath.c_str()) ) ) { return false; }
		return sfFile.WriteToFile( vFile );
	}

	/**
	 * Loads the per-game settings from the given path.
	 * 
	 * \param _u16Path The path from which to load per-game settings.
	 * \return Returns true if the file was found and the settings were loaded from it.
	 **/
	bool CBeesNes::LoadPerGameSettings( const std::u16string &_u16Path ) {
		Options().aoThisGameAudioOptions = Options().aoGlobalAudioOptions;
		Options().aoThisGameAudioOptions.bUseGlobal = true;
		Options().ioThisGameInputOptions = Options().ioGlobalInputOptions;
		Options().ioThisGameInputOptions.bUseGlobal = true;

		Options().ioThisGameInputOptions = Options().ioGlobalInputOptions;
		Options().ioThisGameInputOptions.bUseGlobal = true;
		Options().vfoThisGameFilters = Options().vfoGlobalFilters;
		for ( auto & froThis : Options().vfoThisGameFilters.froRegions ) {
			froThis.bUseGlobal = true;
		}
		if ( _u16Path.size() ) {
			CStdFile sfFile;
			if ( sfFile.Open( _u16Path.c_str() ) ) {
				std::vector<uint8_t> vFile;
				if ( sfFile.LoadToMemory( vFile ) ) {
					sfFile.Close();
					CStream sStream( vFile );
					uint32_t ui32Version = 0;
					if ( !sStream.Read( ui32Version ) ) { return false; }
					if ( !LoadAudioSettings( ui32Version, sStream, Options().aoThisGameAudioOptions ) ) { return false; }

					// Per-game files were always version 0 before NTSC-J was added (so version 0 has 5 regions).
					if ( !LoadRegionPaletteSettings( ui32Version, sStream, Options().poThisGamePalette, Options().poGlobalPalettes ) ) { return false; }
					
					if ( !LoadInputSettings( ui32Version, sStream, m_oOptions.ioThisGameInputOptions ) ) { return false; }
					if ( ui32Version >= 3 ) {
						if ( !LoadVideoFilterSettings( ui32Version, sStream, m_oOptions.vfoThisGameFilters ) ) { return false; }
					}
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * Saves the per-game settings to a given path.
	 * 
	 * \param _u16Path The path to which to save per-game settings.
	 * \return Returns true if the file was created and fully written.
	 **/
	bool CBeesNes::SavePerGameSettings( const std::u16string &_u16Path ) {
		std::vector<uint8_t> vFile;
		CStream sStream( vFile );
		uint32_t ui32Version = LSN_BEESNES_SETTINGS_VERSION;
		if ( !sStream.Write( ui32Version ) ) { return false; }
		if ( !SaveAudioSettings( sStream, Options().aoThisGameAudioOptions ) ) { return false; }

		for ( size_t I = 0; I < std::size( m_oOptions.poThisGamePalette ); ++I ) {
			if ( !SavePaletteSettings( sStream, Options().poThisGamePalette[I] ) ) { return false; }
		}

		if ( !SaveInputSettings( sStream, m_oOptions.ioThisGameInputOptions ) ) { return false; }
		if ( !SaveVideoFilterSettings( sStream, m_oOptions.vfoThisGameFilters ) ) { return false; }

		CStdFile sfFile;
		if ( !sfFile.Create( _u16Path.c_str() ) ) { return false; }
		return sfFile.WriteToFile( vFile );
	}

	/**
	 * Gets the filter options that apply to a given region:  the per-game options if a game is loaded and its options for that region
	 *	are not set to use the global options, otherwise the global options.
	 *
	 * \param _pmRegion The region whose options are to be returned.
	 * \return Returns the filter options that apply to the given region.
	 */
	const LSN_FILTER_REGION_OPTIONS & CBeesNes::FilterOptions( LSN_PPU_METRICS _pmRegion ) const {
		if ( size_t( _pmRegion ) >= LSN_PM_CONSOLE_TOTAL ) { _pmRegion = LSN_PM_NTSC_M; }
		const LSN_FILTER_REGION_OPTIONS & froGame = m_oOptions.vfoThisGameFilters.froRegions[_pmRegion];
		if ( m_u16PerGameSettings.size() && !froGame.bUseGlobal ) { return froGame; }
		return m_oOptions.vfoGlobalFilters.froRegions[_pmRegion];
	}

	/**
	 * Applies the filter options (see FilterOptions()) and the host-display options to every filter.  A filter object made for one region
	 *	(the L. Spiro filters and the NTSC-J GPU filters) receives the settings of that region.  A filter object shared by several regions
	 *	receives the preset nearest the active region (m_pmSystem; see LSN_FILTER_REGION_OPTIONS).  Some settings reallocate buffers or
	 *	regenerate tables, so this should not be called while a frame is being filtered or rendered.  The palette's color space and CRT1
	 *	parameters and the host-display settings are baked into the palettes, which ApplyPaletteOptions() rebuilds.
	 */
	void CBeesNes::ApplyFilterOptions() {
		const LSN_PPU_METRICS pmActive = size_t( m_pmSystem ) < LSN_PM_CONSOLE_TOTAL ? m_pmSystem : LSN_PM_NTSC_M;
		const LSN_PPU_METRICS pmNtsc = (pmActive == LSN_PM_NTSC_J) ? LSN_PM_NTSC_J : LSN_PM_NTSC_M;
		const LSN_FILTER_REGION_OPTIONS & froActive = FilterOptions( pmActive );
		const LSN_FILTER_REGION_OPTIONS & froNtsc = FilterOptions( pmNtsc );
		const LSN_FILTER_REGION_OPTIONS & froNtscM = FilterOptions( LSN_PM_NTSC_M );
		const LSN_FILTER_REGION_OPTIONS & froNtscJ = FilterOptions( LSN_PM_NTSC_J );
		const LSN_FILTER_REGION_OPTIONS & froPal = FilterOptions( LSN_PM_PAL );
		const LSN_FILTER_REGION_OPTIONS & froDendy = FilterOptions( LSN_PM_DENDY );
		const LSN_FILTER_REGION_OPTIONS & froPalM = FilterOptions( LSN_PM_PALM );
		const LSN_FILTER_REGION_OPTIONS & froPalN = FilterOptions( LSN_PM_PALN );
		const CColorSpace::LSN_DISPLAY_SETTINGS & dsDisplay = m_oOptions.hdoHostDisplay.dsDisplay;

		// == Software filters.
		m_nbfBlarggNtscFilter.SetSetup( froNtsc.boBlarggNtsc.ToSetup() );
		m_ncfEmmirNtscFullFilter.SetSettings( froNtsc.lsLmpNtscCpu );
		m_ncfEmmirPalFullFilter.SetSettings( froPal.lsLmpPalCpu );
		m_ncfEmmirDendyFullFilter.SetSettings( froDendy.lsLmpPalCpu );
		ApplyLSpiroOptions( m_nbfLSpiroNtscFilter, froNtsc.lsoLSpiroCpu, true );
		ApplyLSpiroOptions( m_nbfLSpiroPalFilter, froPal.lsoLSpiroCpu, true );
		ApplyLSpiroOptions( m_nbfLSpiroDendyFilter, froDendy.lsoLSpiroCpu, true );
		ApplyLSpiroOptions( m_nbfLSpiroPalMFilter, froPalM.lsoLSpiroCpu, true );
		ApplyLSpiroOptions( m_nbfLSpiroPalNFilter, froPalN.lsoLSpiroCpu, true );
		m_nbfLSpiroNtscFilter.SetSoftwarePhosphorDecay( froNtsc.gfoLSpiro.bPhosphorDecay );
		m_nbfLSpiroPalFilter.SetSoftwarePhosphorDecay( froPal.gfoLSpiro.bPhosphorDecay );
		m_nbfLSpiroDendyFilter.SetSoftwarePhosphorDecay( froDendy.gfoLSpiro.bPhosphorDecay );
		m_nbfLSpiroPalMFilter.SetSoftwarePhosphorDecay( froPalM.gfoLSpiro.bPhosphorDecay );
		m_nbfLSpiroPalNFilter.SetSoftwarePhosphorDecay( froPalN.gfoLSpiro.bPhosphorDecay );

		// == GPU filters.  My filters are called through their bases because they inherit two sets of similarly named functions.
		//	The palette has its CRT gamma curve and color space baked in (see BakedPalette()), so the palette filters get the monitor's color
		//	space and no gamma curve, which skips their gamma passes.
#define LSN_APPLY_GPU_FILTERS( PALETTE, BLARGG_NTSC, BLARGG_NTSC_J, LMP_NTSC, LMP_NTSC_J, LMP_PAL, LMP_DENDY, LSPIRO_NTSC, LSPIRO_NTSC_J, LSPIRO_PAL, LSPIRO_DENDY, LSPIRO_PALM, LSPIRO_PALN )		\
		ApplyGpuFilterOptions( PALETTE, froActive.gfoPalette, dsDisplay );																												\
		PALETTE.SetColorSpace( dsDisplay.cspColorSpace );																																\
		PALETTE.SetGamma( CNesPalette::LSN_G_NONE );																																	\
		BLARGG_NTSC.SetSetup( froNtscM.boBlarggNtsc.ToSetup() );																														\
		ApplyGpuFilterOptions( BLARGG_NTSC, froNtscM.gfoBlargg, dsDisplay );																											\
		BLARGG_NTSC_J.SetSetup( froNtscJ.boBlarggNtsc.ToSetup() );																														\
		ApplyGpuFilterOptions( BLARGG_NTSC_J, froNtscJ.gfoBlargg, dsDisplay );																											\
		LMP_NTSC.SetSettings( froNtscM.lsLmpNtscGpu );																																	\
		ApplyGpuFilterOptions( LMP_NTSC, froNtscM.gfoLmpNtsc, dsDisplay );																												\
		LMP_NTSC_J.SetSettings( froNtscJ.lsLmpNtscGpu );																																\
		ApplyGpuFilterOptions( LMP_NTSC_J, froNtscJ.gfoLmpNtsc, dsDisplay );																											\
		LMP_PAL.SetSettings( froPal.lsLmpPalGpu );																																		\
		ApplyGpuFilterOptions( LMP_PAL, froPal.gfoLmpPal, dsDisplay );																													\
		LMP_DENDY.SetSettings( froDendy.lsLmpPalGpu );																																	\
		ApplyGpuFilterOptions( LMP_DENDY, froDendy.gfoLmpPal, dsDisplay );																												\
		ApplyLSpiroOptions( static_cast<CLSpiroNtscFilterBase &>(LSPIRO_NTSC), froNtscM.lsoLSpiroGpu, false );																			\
		ApplyGpuFilterOptions( static_cast<CGpuFilterBase &>(LSPIRO_NTSC), froNtscM.gfoLSpiro, dsDisplay );																				\
		ApplyLSpiroOptions( static_cast<CLSpiroNtscFilterBase &>(LSPIRO_NTSC_J), froNtscJ.lsoLSpiroGpu, false );																		\
		ApplyGpuFilterOptions( static_cast<CGpuFilterBase &>(LSPIRO_NTSC_J), froNtscJ.gfoLSpiro, dsDisplay );																			\
		ApplyLSpiroOptions( static_cast<CLSpiroPalFilterBase &>(LSPIRO_PAL), froPal.lsoLSpiroGpu, false );																				\
		ApplyGpuFilterOptions( static_cast<CGpuFilterBase &>(LSPIRO_PAL), froPal.gfoLSpiro, dsDisplay );																				\
		ApplyLSpiroOptions( static_cast<CLSpiroPalFilterBase &>(LSPIRO_DENDY), froDendy.lsoLSpiroGpu, false );																			\
		ApplyGpuFilterOptions( static_cast<CGpuFilterBase &>(LSPIRO_DENDY), froDendy.gfoLSpiro, dsDisplay );																			\
		ApplyLSpiroOptions( static_cast<CLSpiroPalFilterBase &>(LSPIRO_PALM), froPalM.lsoLSpiroGpu, false );																			\
		ApplyGpuFilterOptions( static_cast<CGpuFilterBase &>(LSPIRO_PALM), froPalM.gfoLSpiro, dsDisplay );																				\
		ApplyLSpiroOptions( static_cast<CLSpiroPalFilterBase &>(LSPIRO_PALN), froPalN.lsoLSpiroGpu, false );																			\
		ApplyGpuFilterOptions( static_cast<CGpuFilterBase &>(LSPIRO_PALN), froPalN.gfoLSpiro, dsDisplay );

#ifdef LSN_DX9
		LSN_APPLY_GPU_FILTERS( m_d9pfDx9Palette, m_d9nbfDx9BlarggNtscFilter, m_d9nbfDx9BlarggNtscJFilter,
			m_d9ncfDx9EmmirNtscFullFilter, m_d9ncfDx9EmmirNtscJFullFilter, m_d9ncfDx9EmmirPalFullFilter, m_d9ncfDx9EmmirDendyFullFilter,
			m_d9nlsfDx9LSpiroNtsc, m_d9nlsfDx9LSpiroNtscJ, m_d9plsfDx9LSpiroPal, m_d9plsfDx9LSpiroDendy, m_d9plsfDx9LSpiroPalM, m_d9plsfDx9LSpiroPalN )
#endif	// #ifdef LSN_DX9
#ifdef LSN_DX12
		LSN_APPLY_GPU_FILTERS( m_d12pfDx12Palette, m_d12nbfDx12BlarggNtscFilter, m_d12nbfDx12BlarggNtscJFilter,
			m_d12ncfDx12EmmirNtscFullFilter, m_d12ncfDx12EmmirNtscJFullFilter, m_d12ncfDx12EmmirPalFullFilter, m_d12ncfDx12EmmirDendyFullFilter,
			m_d12nlsfDx12LSpiroNtsc, m_d12nlsfDx12LSpiroNtscJ, m_d12plsfDx12LSpiroPal, m_d12plsfDx12LSpiroDendy, m_d12plsfDx12LSpiroPalM, m_d12plsfDx12LSpiroPalN )
#endif	// #ifdef LSN_DX12
#ifdef LSN_VULKAN1
		LSN_APPLY_GPU_FILTERS( m_vpfVulkanPalette, m_vnbfVulkanBlarggNtscFilter, m_vnbfVulkanBlarggNtscJFilter,
			m_vncfVulkanEmmirNtscFullFilter, m_vncfVulkanEmmirNtscJFullFilter, m_vncfVulkanEmmirPalFullFilter, m_vncfVulkanEmmirDendyFullFilter,
			m_vnlsfVulkanLSpiroNtsc, m_vnlsfVulkanLSpiroNtscJ, m_vplsfVulkanLSpiroPal, m_vplsfVulkanLSpiroDendy, m_vplsfVulkanLSpiroPalM, m_vplsfVulkanLSpiroPalN )
#endif	// #ifdef LSN_VULKAN1
#undef LSN_APPLY_GPU_FILTERS

		UpdateNoise();
	}

	/**
	 * Gets the L. Spiro settings that apply to a filter object, if it is one of the L. Spiro filters.
	 *
	 * \param _pfbFilter The filter object.
	 * \return Returns the L. Spiro settings of the filter or nullptr if it is not an L. Spiro filter.
	 */
	const LSN_LSPIRO_OPTIONS * CBeesNes::LSpiroOptionsOfFilter( const CFilterBase * _pfbFilter ) const {
		if ( !_pfbFilter ) { return nullptr; }
		const LSN_PPU_METRICS pmActive = size_t( m_pmSystem ) < LSN_PM_CONSOLE_TOTAL ? m_pmSystem : LSN_PM_NTSC_M;
		const LSN_PPU_METRICS pmNtsc = (pmActive == LSN_PM_NTSC_J) ? LSN_PM_NTSC_J : LSN_PM_NTSC_M;
		/** A filter and the region whose settings it uses. */
		struct LSN_FILTER_REGION {
			const CFilterBase *								pfbFilter;									/**< The filter. */
			LSN_PPU_METRICS									pmRegion;									/**< The region whose settings it uses. */
			bool											bGpu;										/**< If true, the GPU settings apply. */
		};
		const LSN_FILTER_REGION frFilters[] = {
			{ &m_nbfLSpiroNtscFilter, pmNtsc, false },
			{ &m_nbfLSpiroPalFilter, LSN_PM_PAL, false },
			{ &m_nbfLSpiroDendyFilter, LSN_PM_DENDY, false },
			{ &m_nbfLSpiroPalMFilter, LSN_PM_PALM, false },
			{ &m_nbfLSpiroPalNFilter, LSN_PM_PALN, false },
#ifdef LSN_DX9
			{ &m_d9nlsfDx9LSpiroNtsc, LSN_PM_NTSC_M, true },
			{ &m_d9nlsfDx9LSpiroNtscJ, LSN_PM_NTSC_J, true },
			{ &m_d9plsfDx9LSpiroPal, LSN_PM_PAL, true },
			{ &m_d9plsfDx9LSpiroDendy, LSN_PM_DENDY, true },
			{ &m_d9plsfDx9LSpiroPalM, LSN_PM_PALM, true },
			{ &m_d9plsfDx9LSpiroPalN, LSN_PM_PALN, true },
#endif	// #ifdef LSN_DX9
#ifdef LSN_DX12
			{ &m_d12nlsfDx12LSpiroNtsc, LSN_PM_NTSC_M, true },
			{ &m_d12nlsfDx12LSpiroNtscJ, LSN_PM_NTSC_J, true },
			{ &m_d12plsfDx12LSpiroPal, LSN_PM_PAL, true },
			{ &m_d12plsfDx12LSpiroDendy, LSN_PM_DENDY, true },
			{ &m_d12plsfDx12LSpiroPalM, LSN_PM_PALM, true },
			{ &m_d12plsfDx12LSpiroPalN, LSN_PM_PALN, true },
#endif	// #ifdef LSN_DX12
#ifdef LSN_VULKAN1
			{ &m_vnlsfVulkanLSpiroNtsc, LSN_PM_NTSC_M, true },
			{ &m_vnlsfVulkanLSpiroNtscJ, LSN_PM_NTSC_J, true },
			{ &m_vplsfVulkanLSpiroPal, LSN_PM_PAL, true },
			{ &m_vplsfVulkanLSpiroDendy, LSN_PM_DENDY, true },
			{ &m_vplsfVulkanLSpiroPalM, LSN_PM_PALM, true },
			{ &m_vplsfVulkanLSpiroPalN, LSN_PM_PALN, true },
#endif	// #ifdef LSN_VULKAN1
		};
		for ( const auto & frThis : frFilters ) {
			if ( frThis.pfbFilter == _pfbFilter ) {
				const LSN_FILTER_REGION_OPTIONS & froOptions = FilterOptions( frThis.pmRegion );
				return frThis.bGpu ? &froOptions.lsoLSpiroGpu : &froOptions.lsoLSpiroCpu;
			}
		}
		return nullptr;
	}

	/**
	 * Fills the signal-noise buffers (shared by all L. Spiro filters) according to the settings of the selected filter.  If the selected
	 *	filter is not an L. Spiro filter, nothing changes.
	 */
	void CBeesNes::UpdateNoise() {
		if ( size_t( m_oOptions.fFilter ) >= CFilterBase::LSN_F_TOTAL ) { return; }
		// Before the first system is created (during construction), the region is the default system's.
		const LSN_PPU_METRICS pmRegion = (m_psbSystem && m_psbSystem->GetDisplayClient()) ? GetCurPpuRegion() : m_pmSystem;
		if ( size_t( pmRegion ) >= LSN_PM_CONSOLE_TOTAL ) { return; }
		const LSN_LSPIRO_OPTIONS * plsoOptions = LSpiroOptionsOfFilter( m_pfbFilterTable[m_oOptions.fFilter][pmRegion] );
		if ( !plsoOptions ) { return; }
		switch ( plsoOptions->ntNoiseType ) {
			case LSN_NT_CUSTOM : {
				if ( plsoOptions->fNoiseAmount > 0.0f ) { CUtilities::GenGaussianNoise( plsoOptions->fNoiseAmount ); }
				else { CUtilities::ClearNoise(); }
				break;
			}
			case LSN_NT_PHYSICAL : {
				CUtilities::GenThermalNoise( m_trTemperatureReader.FilteredTemperature() );
				break;
			}
			default : {
				CUtilities::ClearNoise();
			}
		}
	}

	/**
	 * Applies the settings shared by all GPU filters (and the host-display settings) to a GPU filter.
	 *
	 * \param _gfbFilter The filter to which to apply the settings.
	 * \param _gfoOptions The settings to apply.
	 * \param _dsDisplay The settings of the host display.
	 */
	void CBeesNes::ApplyGpuFilterOptions( CGpuFilterBase &_gfbFilter, const LSN_GPU_FILTER_OPTIONS &_gfoOptions, const CColorSpace::LSN_DISPLAY_SETTINGS &_dsDisplay ) {
		_gfbFilter.SetDisplaySettings( _dsDisplay );
		_gfbFilter.SetColorSpace( _gfoOptions.cspColorSpace );
		_gfbFilter.SetHueOffset( _gfoOptions.fHueOffset );
		_gfbFilter.SetGamma( _gfoOptions.gGamma );
		_gfbFilter.SetCrt1Parms( _gfoOptions.fCrt1White, _gfoOptions.fCrt1Brightness );
		_gfbFilter.SetPhosphorDecayEnable( _gfoOptions.bPhosphorDecay );
		_gfbFilter.SetPhosphorDecayLevel( _gfoOptions.fPhosphorDecayLevel );
		_gfbFilter.SetPhosphorDecayPeriod( _gfoOptions.fPhosphorDecayPeriod );
		_gfbFilter.SetPhosphorDecayMultipliers( _gfoOptions.fPhosphorRedMultiplier, _gfoOptions.fPhosphorBlueMultiplier );
		_gfbFilter.SetVertSharpness( _gfoOptions.ui32VertSharpness );
		_gfbFilter.SetHorSharpness( _gfoOptions.ui32HorSharpness );
		_gfbFilter.SetDynamicScaling( _gfoOptions.bDynamicScaling );
		_gfbFilter.SetUseHighQualityResampler( _gfoOptions.bHighQualityResampler );
		for ( uint32_t I = 0; I < CGpuFilterBase::LSN_CB_TOTAL; ++I ) {
			_gfbFilter.SetConvolutionFilter( CGpuFilterBase::LSN_CONVOLUTION_BAND( I ), _gfoOptions.ffConvolution[I] );
		}
	}

	/**
	 * Applies L. Spiro settings to an L. Spiro NTSC filter.
	 *
	 * \param _lsnfbFilter The filter to which to apply the settings.
	 * \param _lsoOptions The settings to apply.
	 * \param _bSoftware If true, the software-only settings (phosphor decay) are also applied.
	 */
	void CBeesNes::ApplyLSpiroOptions( CLSpiroNtscFilterBase &_lsnfbFilter, const LSN_LSPIRO_OPTIONS &_lsoOptions, bool _bSoftware ) {
		_lsnfbFilter.SetWidthScale( _lsoOptions.ui16WidthScale );
		_lsnfbFilter.SetKernelSizeY( _lsoOptions.ui32KernelSizeY );
		_lsnfbFilter.SetKernelSizeChroma( _lsoOptions.ui32KernelSizeChroma );
		_lsnfbFilter.SetBleed( _lsoOptions.fBleed );
		_lsnfbFilter.SetFilterFuncY( _lsoOptions.ffFilterY, _lsoOptions.fFilterYParms[0], _lsoOptions.fFilterYParms[1] );
		_lsnfbFilter.SetFilterFuncChroma( _lsoOptions.ffFilterChroma, _lsoOptions.fFilterChromaParms[0], _lsoOptions.fFilterChromaParms[1] );
		_lsnfbFilter.SetGamma( _lsoOptions.fGamma );
		_lsnfbFilter.SetHue( _lsoOptions.fHue );
		_lsnfbFilter.SetBrightness( _lsoOptions.fBrightness );
		_lsnfbFilter.SetSaturation( _lsoOptions.fSaturation );
		_lsnfbFilter.SetContrast( _lsoOptions.fContrast );
		_lsnfbFilter.SetBlackLevel( _lsoOptions.fBlackLevel );
		_lsnfbFilter.SetWhiteLevel( _lsoOptions.fWhiteLevel );
		if ( _bSoftware ) {
			_lsnfbFilter.SetPhosphorDecayPeriod( _lsoOptions.fPhosphorDecayPeriod );
			_lsnfbFilter.SetPhosphorDecayLevel( _lsoOptions.fPhosphorDecayLevel );
		}
	}

	/**
	 * Applies L. Spiro settings to an L. Spiro PAL filter.
	 *
	 * \param _lspfbFilter The filter to which to apply the settings.
	 * \param _lsoOptions The settings to apply.
	 * \param _bSoftware If true, the software-only settings (phosphor decay) are also applied.
	 */
	void CBeesNes::ApplyLSpiroOptions( CLSpiroPalFilterBase &_lspfbFilter, const LSN_LSPIRO_OPTIONS &_lsoOptions, bool _bSoftware ) {
		_lspfbFilter.SetPixelToSignal( _lsoOptions.ui16PixelToSignal );
		_lspfbFilter.SetWidthScale( _lsoOptions.ui16WidthScale );
		_lspfbFilter.SetKernelSizeY( _lsoOptions.ui32KernelSizeY );
		_lspfbFilter.SetKernelSizeChroma( _lsoOptions.ui32KernelSizeChroma );
		_lspfbFilter.SetCombFilter( _lsoOptions.cfCombFilter );
		_lspfbFilter.SetCombWeight( _lsoOptions.fCombWeight );
		_lspfbFilter.SetBleed( _lsoOptions.fBleed );
		_lspfbFilter.SetFilterFuncY( _lsoOptions.ffFilterY, _lsoOptions.fFilterYParms[0], _lsoOptions.fFilterYParms[1] );
		_lspfbFilter.SetFilterFuncChroma( _lsoOptions.ffFilterChroma, _lsoOptions.fFilterChromaParms[0], _lsoOptions.fFilterChromaParms[1] );
		_lspfbFilter.SetGamma( _lsoOptions.fGamma );
		_lspfbFilter.SetHue( _lsoOptions.fHue );
		_lspfbFilter.SetBrightness( _lsoOptions.fBrightness );
		_lspfbFilter.SetSaturation( _lsoOptions.fSaturation );
		_lspfbFilter.SetContrast( _lsoOptions.fContrast );
		_lspfbFilter.SetBlackLevel( _lsoOptions.fBlackLevel );
		_lspfbFilter.SetWhiteLevel( _lsoOptions.fWhiteLevel );
		if ( _bSoftware ) {
			_lspfbFilter.SetPhosphorDecayPeriod( _lsoOptions.fPhosphorDecayPeriod );
			_lspfbFilter.SetPhosphorDecayLevel( _lsoOptions.fPhosphorDecayLevel );
		}
	}

	/**
	 * Sets the current filter.  Changes are not applied to rendering output until Swap() is called.
	 *
	 * \param _fFilter The new filter to be applied after the next Swap().
	 */
	void CBeesNes::SetCurFilter( CFilterBase::LSN_FILTERS _fFilter ) {
		if ( m_oOptions.fFilter != _fFilter || !m_bHaveSetFilter ) {

			CFilterBase * pfbPrev = m_pfbFilterTable[m_oOptions.fFilter][GetCurPpuRegion()];
			m_oOptions.fFilter = _fFilter;
			m_cfartCurFilterAndTargets.pfbNextFilter = m_pfbFilterTable[m_oOptions.fFilter][GetCurPpuRegion()];
			if ( !m_bHaveSetFilter ) {
				// First call.
				if ( m_cfartCurFilterAndTargets.pfbNextFilter ) {
					m_cfartCurFilterAndTargets.pfbNextFilter->Activate();
				}
				m_bHaveSetFilter = true;
			}
			else {
				if ( pfbPrev != m_cfartCurFilterAndTargets.pfbNextFilter ) {
					// If the new filter is the same as what has been queued up for deactivation, deactivate first and then activate.
					if ( m_cfartCurFilterAndTargets.pfbDeactivateMe == m_cfartCurFilterAndTargets.pfbNextFilter ) {
						m_cfartCurFilterAndTargets.DeActivate();
					}
				
					if ( m_cfartCurFilterAndTargets.pfbNextFilter ) {
						m_cfartCurFilterAndTargets.pfbNextFilter->Activate();
					}
					if ( pfbPrev ) {
						m_cfartCurFilterAndTargets.DeActivate();
						m_cfartCurFilterAndTargets.pfbDeactivateMe = pfbPrev;
					}
				}
			}
			// The noise follows the settings of the selected filter.
			UpdateNoise();
		}
	}

	/**
	 * Determines if a given filter is a Direct3D 9 filter.
	 * 
	 * \param _fFilter The filter to test.
	 * \return Returns true if Direct3D 9 is enabled and the filter is one of the Direct3D 9 filters.
	 **/
	bool CBeesNes::IsDirect3D9Filter( CFilterBase::LSN_FILTERS _fFilter ) {
		static_cast<void>(_fFilter);
#ifdef LSN_DX9
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_DX9 : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_BLARGG_DX9 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_CRT_FULL_DX9 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_PAL_CRT_FULL_DX9 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRONTSC_DX9 : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPAL_DX9 : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRODENDY_DX9 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALM_DX9 : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALN_DX9 : {}					LSN_FALLTHROUGH
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_DX9 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRO_AUTO_DX9 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_J_BLARGG_DX9 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_J_CRT_FULL_DX9 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_DENDY_CRT_FULL_DX9 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRONTSC_J_DX9 : { return true; }
		}
#endif	// #ifdef LSN_DX9
		return false;
	}

	/**
	 * Determines if a given filter is a Direct3D 12 filter.
	 * 
	 * \param _fFilter The filter to test.
	 * \return Returns true if Direct3D 12 is enabled and the filter is one of the Direct3D 12 filters.
	 **/
	bool CBeesNes::IsDirect3D12Filter( CFilterBase::LSN_FILTERS _fFilter ) {
		static_cast<void>(_fFilter);
#ifdef LSN_DX12
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_DX12 : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_BLARGG_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_CRT_FULL_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_PAL_CRT_FULL_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRONTSC_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPAL_DX12 : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRODENDY_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALM_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALN_DX12 : {}				LSN_FALLTHROUGH
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRO_AUTO_DX12 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_J_BLARGG_DX12 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_J_CRT_FULL_DX12 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_DENDY_CRT_FULL_DX12 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRONTSC_J_DX12 : { return true; }
		}
#endif	// #ifdef LSN_DX12
		return false;
	}

	/**
	 * Determines if a given filter is a Vulkan 1 filter.
	 * 
	 * \param _fFilter The filter to test.
	 * \return Returns true if Vulkan 1 is enabled and the filter is one of the Vulkan 1 filters.
	 **/
	bool CBeesNes::IsVulkan1Filter( CFilterBase::LSN_FILTERS _fFilter ) {
		static_cast<void>(_fFilter);
#ifdef LSN_VULKAN1
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_VULKAN1 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_BLARGG_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_CRT_FULL_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_PAL_CRT_FULL_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRONTSC_VULKAN1 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPAL_VULKAN1 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRODENDY_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALM_VULKAN1 : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALN_VULKAN1 : {}				LSN_FALLTHROUGH
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRO_AUTO_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_J_BLARGG_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_J_CRT_FULL_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_DENDY_CRT_FULL_VULKAN1 : {}			LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRONTSC_J_VULKAN1 : { return true; }
		}
#endif	// #ifdef LSN_VULKAN1
		return false;
	}

	/**
	 * Determines if a given filter is a Metal filter.
	 * 
	 * \param _fFilter The filter to test.
	 * \return Returns true if Metal is enabled and the filter is one of the Metal filters.
	 **/
	bool CBeesNes::IsMetalFilter( CFilterBase::LSN_FILTERS _fFilter ) {
		static_cast<void>(_fFilter);
#ifdef LSN_METAL
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_METAL : {}						LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_BLARGG_METAL : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_NTSC_CRT_FULL_METAL : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_PAL_CRT_FULL_METAL : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRONTSC_METAL : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPAL_METAL : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRODENDY_METAL : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALM_METAL : {}					LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIROPALN_METAL : {}					LSN_FALLTHROUGH
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_METAL : {}				LSN_FALLTHROUGH
			case CFilterBase::LSN_F_LSPIRO_AUTO_METAL : { return true; }
		}
#endif	// #ifdef LSN_METAL
		return false;
	}

	/**
	 * Converts a Direct3d 9 filter to a software filter.
	 * 
	 * \param _fFilter The filter to convert.
	 * \return Returns the software implementation for the given filter.
	 **/
	CFilterBase::LSN_FILTERS CBeesNes::Direct3D9FilterToSoftware( CFilterBase::LSN_FILTERS _fFilter ) {
#ifdef LSN_DX9
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_DX9 : { return CFilterBase::LSN_F_INDEXED; }
			case CFilterBase::LSN_F_NTSC_BLARGG_DX9 : { return CFilterBase::LSN_F_NTSC_BLARGG; }
			case CFilterBase::LSN_F_NTSC_CRT_FULL_DX9 : { return CFilterBase::LSN_F_NTSC_CRT_FULL; }
			case CFilterBase::LSN_F_PAL_CRT_FULL_DX9 : { return CFilterBase::LSN_F_PAL_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRONTSC_DX9 : { return CFilterBase::LSN_F_NTSC_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPAL_DX9 : { return CFilterBase::LSN_F_PAL_LSPIRO; }
			case CFilterBase::LSN_F_LSPIRODENDY_DX9 : { return CFilterBase::LSN_F_DENDY_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALM_DX9 : { return CFilterBase::LSN_F_PALM_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALN_DX9 : { return CFilterBase::LSN_F_PALN_LSPIRO; }
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_DX9 : { return CFilterBase::LSN_F_AUTO_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRO_AUTO_DX9 : { return CFilterBase::LSN_F_AUTO_LSPIRO; }
			// The NTSC-J filters exist only on the GPU (the NTSC-J color space is too heavy for the CPU), so they become their NTSC-M versions.
			case CFilterBase::LSN_F_NTSC_J_BLARGG_DX9 : { return CFilterBase::LSN_F_NTSC_BLARGG; }
			case CFilterBase::LSN_F_NTSC_J_CRT_FULL_DX9 : { return CFilterBase::LSN_F_NTSC_CRT_FULL; }
			case CFilterBase::LSN_F_DENDY_CRT_FULL_DX9 : { return CFilterBase::LSN_F_DENDY_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRONTSC_J_DX9 : { return CFilterBase::LSN_F_NTSC_LSPIRO; }
		}
#endif	// #ifdef LSN_DX9
		return _fFilter;
	}

	/**
	 * Converts a Direct3d 12 filter to a software filter.
	 * 
	 * \param _fFilter The filter to convert.
	 * \return Returns the software implementation for the given filter.
	 **/
	CFilterBase::LSN_FILTERS CBeesNes::Direct3D12FilterToSoftware( CFilterBase::LSN_FILTERS _fFilter ) {
#ifdef LSN_DX12
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_DX12 : { return CFilterBase::LSN_F_INDEXED; }
			case CFilterBase::LSN_F_NTSC_BLARGG_DX12 : { return CFilterBase::LSN_F_NTSC_BLARGG; }
			case CFilterBase::LSN_F_NTSC_CRT_FULL_DX12 : { return CFilterBase::LSN_F_NTSC_CRT_FULL; }
			case CFilterBase::LSN_F_PAL_CRT_FULL_DX12 : { return CFilterBase::LSN_F_PAL_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRONTSC_DX12 : { return CFilterBase::LSN_F_NTSC_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPAL_DX12 : { return CFilterBase::LSN_F_PAL_LSPIRO; }
			case CFilterBase::LSN_F_LSPIRODENDY_DX12 : { return CFilterBase::LSN_F_DENDY_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALM_DX12 : { return CFilterBase::LSN_F_PALM_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALN_DX12 : { return CFilterBase::LSN_F_PALN_LSPIRO; }
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_DX12 : { return CFilterBase::LSN_F_AUTO_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRO_AUTO_DX12 : { return CFilterBase::LSN_F_AUTO_LSPIRO; }
			// The NTSC-J filters exist only on the GPU (the NTSC-J color space is too heavy for the CPU), so they become their NTSC-M versions.
			case CFilterBase::LSN_F_NTSC_J_BLARGG_DX12 : { return CFilterBase::LSN_F_NTSC_BLARGG; }
			case CFilterBase::LSN_F_NTSC_J_CRT_FULL_DX12 : { return CFilterBase::LSN_F_NTSC_CRT_FULL; }
			case CFilterBase::LSN_F_DENDY_CRT_FULL_DX12 : { return CFilterBase::LSN_F_DENDY_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRONTSC_J_DX12 : { return CFilterBase::LSN_F_NTSC_LSPIRO; }
		}
#endif	// #ifdef LSN_DX12
		return _fFilter;
	}

	/**
	 * Converts a Vulkan 1 filter to a software filter.
	 * 
	 * \param _fFilter The filter to convert.
	 * \return Returns the software implementation for the given filter.
	 **/
	CFilterBase::LSN_FILTERS CBeesNes::Vulkan1FilterToSoftware( CFilterBase::LSN_FILTERS _fFilter ) {
#ifdef LSN_VULKAN1
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_VULKAN1 : { return CFilterBase::LSN_F_INDEXED; }
			case CFilterBase::LSN_F_NTSC_BLARGG_VULKAN1 : { return CFilterBase::LSN_F_NTSC_BLARGG; }
			case CFilterBase::LSN_F_NTSC_CRT_FULL_VULKAN1 : { return CFilterBase::LSN_F_NTSC_CRT_FULL; }
			case CFilterBase::LSN_F_PAL_CRT_FULL_VULKAN1 : { return CFilterBase::LSN_F_PAL_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRONTSC_VULKAN1 : { return CFilterBase::LSN_F_NTSC_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPAL_VULKAN1 : { return CFilterBase::LSN_F_PAL_LSPIRO; }
			case CFilterBase::LSN_F_LSPIRODENDY_VULKAN1 : { return CFilterBase::LSN_F_DENDY_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALM_VULKAN1 : { return CFilterBase::LSN_F_PALM_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALN_VULKAN1 : { return CFilterBase::LSN_F_PALN_LSPIRO; }
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_VULKAN1 : { return CFilterBase::LSN_F_AUTO_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRO_AUTO_VULKAN1 : { return CFilterBase::LSN_F_AUTO_LSPIRO; }
			// The NTSC-J filters exist only on the GPU (the NTSC-J color space is too heavy for the CPU), so they become their NTSC-M versions.
			case CFilterBase::LSN_F_NTSC_J_BLARGG_VULKAN1 : { return CFilterBase::LSN_F_NTSC_BLARGG; }
			case CFilterBase::LSN_F_NTSC_J_CRT_FULL_VULKAN1 : { return CFilterBase::LSN_F_NTSC_CRT_FULL; }
			case CFilterBase::LSN_F_DENDY_CRT_FULL_VULKAN1 : { return CFilterBase::LSN_F_DENDY_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRONTSC_J_VULKAN1 : { return CFilterBase::LSN_F_NTSC_LSPIRO; }
		}
#endif	// #ifdef LSN_VULKAN1
		return _fFilter;
	}

	/**
	 * Converts a Metal filter to a software filter.
	 * 
	 * \param _fFilter The filter to convert.
	 * \return Returns the software implementation for the given filter.
	 **/
	CFilterBase::LSN_FILTERS CBeesNes::MetalFilterToSoftware( CFilterBase::LSN_FILTERS _fFilter ) {
#ifdef LSN_METAL
		switch ( _fFilter ) {
			case CFilterBase::LSN_F_INDEXED_METAL : { return CFilterBase::LSN_F_INDEXED; }
			case CFilterBase::LSN_F_NTSC_BLARGG_METAL : { return CFilterBase::LSN_F_NTSC_BLARGG; }
			case CFilterBase::LSN_F_NTSC_CRT_FULL_METAL : { return CFilterBase::LSN_F_NTSC_CRT_FULL; }
			case CFilterBase::LSN_F_PAL_CRT_FULL_METAL : { return CFilterBase::LSN_F_PAL_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRONTSC_METAL : { return CFilterBase::LSN_F_NTSC_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPAL_METAL : { return CFilterBase::LSN_F_PAL_LSPIRO; }
			case CFilterBase::LSN_F_LSPIRODENDY_METAL : { return CFilterBase::LSN_F_DENDY_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALM_METAL : { return CFilterBase::LSN_F_PALM_LSPIRO; }
			case CFilterBase::LSN_F_LSPIROPALN_METAL : { return CFilterBase::LSN_F_PALN_LSPIRO; }
			
			case CFilterBase::LSN_F_AUTO_CRT_FULL_METAL : { return CFilterBase::LSN_F_AUTO_CRT_FULL; }
			case CFilterBase::LSN_F_LSPIRO_AUTO_METAL : { return CFilterBase::LSN_F_AUTO_LSPIRO; }
		}
#endif	// #ifdef LSN_METAL
		return _fFilter;
	}

	/**
	 * Gets the current region.
	 *
	 * \return Returns the current region.
	 */
	LSN_PPU_METRICS CBeesNes::GetCurPpuRegion() const {
		return m_psbSystem->GetDisplayClient()->PpuRegion();
	}

	/**
	 * Informs the current filter of a window resize.  Only sent to GPU filters.
	 **/
	void CBeesNes::WindowResized() {
		CFilterBase * pfbCur = m_pfbFilterTable[m_oOptions.fFilter][GetCurPpuRegion()];
		if ( pfbCur->IsGpuFilter() ) {
			CGpuFilterBase * pgfbThis = static_cast<CGpuFilterBase *>(pfbCur);
			pgfbThis->FrameResize();
		}
	}

	/**
	 * Determines whether a ROM is loaded or not.
	 *
	 * \return Returns true if the system pointer is valid and the system has a loaded ROM file.
	 */
	bool CBeesNes::IsRomLoaded() const {
		if LSN_LIKELY( m_psbSystem ) {
			return m_psbSystem->IsRomLoaded();
		}
		return false;
	}

	/**
	 * If the render buffer is dirty, a render is performed (PPU buffer -> Filters -> Post-Processing).
	 *	This should be called inside the same critical section/mutex that calls Swap().
	 *
	 * \param _i32Left The left of the display region (GPU only).
	 * \param _i32Top The top of the display region (GPU only).
	 * \param _ui32FinalW The final display width.
	 * \param _ui32FinalH The final display height.
	 */
	void CBeesNes::Render( int32_t _i32Left, int32_t _i32Top, uint32_t _ui32FinalW, uint32_t _ui32FinalH ) {
		if ( m_cfartCurFilterAndTargets.bDirty && m_cfartCurFilterAndTargets.pfbPrevFilter ) {
			m_cfartCurFilterAndTargets.bDirty = false;
			if ( m_veExporter.IsActive() ) {
				// FinishRenderedFrame() tells the emulation thread that this frame has been rendered.
				m_ui64ExportRenderedId = m_aui64ExportSwapId;
				m_bExportRenderPending = true;
			}

			if ( m_cfartCurFilterAndTargets.pfbPrevFilter->IsGpuFilter() ) {
				CGpuFilterBase * pgfbThis = static_cast<CGpuFilterBase *>(m_cfartCurFilterAndTargets.pfbPrevFilter);
				// GPU filters capture from the GPU (their returned buffer is not what is shown).
				pgfbThis->SetCaptureTarget( m_bCaptureVideo ? &m_cbVideoCapture : nullptr, m_cfartCurFilterAndTargets.ui64Frame );

				m_cfartCurFilterAndTargets.pui8LastFilteredResult = pgfbThis->ApplyFilter( m_cfartCurFilterAndTargets.pui8CurRenderTarget,
					m_cfartCurFilterAndTargets.ui32Width, m_cfartCurFilterAndTargets.ui32Height, m_cfartCurFilterAndTargets.ui16Bits, m_cfartCurFilterAndTargets.ui32Stride,
					m_cfartCurFilterAndTargets.ui64Frame, m_cfartCurFilterAndTargets.ui64RenderStartCycle,
					_i32Left, _i32Top, _ui32FinalW, _ui32FinalH );
			}
			else {
				// Pure software rendering.
				m_cfartCurFilterAndTargets.pui8LastFilteredResult = m_cfartCurFilterAndTargets.pfbPrevFilter->ApplyFilter( m_cfartCurFilterAndTargets.pui8CurRenderTarget,
					m_cfartCurFilterAndTargets.ui32Width, m_cfartCurFilterAndTargets.ui32Height, m_cfartCurFilterAndTargets.ui16Bits, m_cfartCurFilterAndTargets.ui32Stride,
					m_cfartCurFilterAndTargets.ui64Frame, m_cfartCurFilterAndTargets.ui64RenderStartCycle );

			
				for ( size_t I = 0; I < m_vPostProcesses.size(); ++I ) {
					m_cfartCurFilterAndTargets.pui8LastFilteredResult = m_pppbPostTable[m_vPostProcesses[I]]->ApplyFilter( m_cfartCurFilterAndTargets.pui8LastFilteredResult,
						_ui32FinalW, _ui32FinalH, m_cfartCurFilterAndTargets.bMirrored,
						m_cfartCurFilterAndTargets.ui32Width, m_cfartCurFilterAndTargets.ui32Height, m_cfartCurFilterAndTargets.ui16Bits, m_cfartCurFilterAndTargets.ui32Stride,
						m_cfartCurFilterAndTargets.ui64Frame, m_cfartCurFilterAndTargets.ui64RenderStartCycle );
				}

				if ( m_bCaptureVideo ) {
					// Capture exactly what gets blitted:  a DIB with DWORD-aligned rows, stored bottom-up when bMirrored is set.
					const uint32_t ui32Bits = m_cfartCurFilterAndTargets.ui16Bits;
					m_cbVideoCapture.Copy( m_cfartCurFilterAndTargets.pui8LastFilteredResult,
						m_cfartCurFilterAndTargets.ui32Width, m_cfartCurFilterAndTargets.ui32Height,
						CFilterBase::RowStride( m_cfartCurFilterAndTargets.ui32Width, ui32Bits ), LSN_CAPTURE_BUFFER::DibFormat( ui32Bits ),
						m_cfartCurFilterAndTargets.bMirrored, m_cfartCurFilterAndTargets.ui64Frame );
				}
			}
		}
	}

	/**
	 * Swaps PPU render targets and switches to the next filter if a new filter has been set.
	 *	Should be called inside the same citical section/mutex as Render().
	 *
	 * \param _bActuallySwap If true, the source buffer is swapped.  Set to false to re-render the previous source buffer.
	 */
	void CBeesNes::Swap( bool _bActuallySwap ) {
		bool bMirrired = m_cfartCurFilterAndTargets.bMirrored;
		auto pfbPrev = m_cfartCurFilterAndTargets.pfbPrevFilter;
		m_cfartCurFilterAndTargets.pfbPrevFilter = m_cfartCurFilterAndTargets.pfbCurFilter;
		m_cfartCurFilterAndTargets.pui8CurRenderTarget = m_cfartCurFilterAndTargets.pfbCurFilter->OutputBuffer();
		m_cfartCurFilterAndTargets.ui16Bits = uint16_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputBits() );
		m_cfartCurFilterAndTargets.ui32Width = m_cfartCurFilterAndTargets.pfbCurFilter->OutputWidth();
		m_cfartCurFilterAndTargets.ui32Height = m_cfartCurFilterAndTargets.pfbCurFilter->OutputHeight();
		m_cfartCurFilterAndTargets.ui32Stride = uint32_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride() );
		m_cfartCurFilterAndTargets.ui64Frame = GetDisplayClient()->FrameCount();
		m_cfartCurFilterAndTargets.ui64RenderStartCycle = GetDisplayClient()->GetRenderStartCycle();
		m_cfartCurFilterAndTargets.bDirty = true;
		m_cfartCurFilterAndTargets.bMirrored = m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput();

		m_cfartCurFilterAndTargets.pfbCurFilter = m_cfartCurFilterAndTargets.pfbNextFilter;
		if ( _bActuallySwap && m_veExporter.IsActive() ) { ++m_aui64ExportSwapId; }
		
		if LSN_LIKELY( _bActuallySwap ) {
			m_cfartCurFilterAndTargets.pfbCurFilter->Swap();
		}
		else if ( pfbPrev ) {
			if ( m_cfartCurFilterAndTargets.bMirrored != bMirrired ) {
				std::memcpy( m_cfartCurFilterAndTargets.pfbCurFilter->CurTarget(), pfbPrev->CurTarget(),
					m_cfartCurFilterAndTargets.ui32Stride * m_cfartCurFilterAndTargets.ui32Height );
			}
			else {
				std::memcpy( m_cfartCurFilterAndTargets.pfbCurFilter->CurTarget(), pfbPrev->CurTarget(),
					m_cfartCurFilterAndTargets.ui32Stride * m_cfartCurFilterAndTargets.ui32Height );
			}
		}
		GetDisplayClient()->SetRenderTarget( m_cfartCurFilterAndTargets.pfbCurFilter->CurTarget(), m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride(), m_cfartCurFilterAndTargets.pfbCurFilter->InputFormat(), m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput() );
	}

	/**
	 * Determines if swapping is safe (the new filter has all of the same inputs as the current filter).
	 * 
	 * \return Returns true if the queued filter has all of the same inputs (format, bit depth, dimensions, etc.) as the current filter.
	 **/
	bool CBeesNes::SwapIsSafe() {
		if ( m_cfartCurFilterAndTargets.ui16Bits != uint16_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputBits() ) ) { return false; }
		if ( m_cfartCurFilterAndTargets.ui32Width != m_cfartCurFilterAndTargets.pfbCurFilter->OutputWidth() ) { return false; }
		if ( m_cfartCurFilterAndTargets.ui32Height != m_cfartCurFilterAndTargets.pfbCurFilter->OutputHeight() ) { return false; }
		if ( m_cfartCurFilterAndTargets.ui32Stride != uint32_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride() ) ) { return false; }
		return true;
	}

	/**
	 * Loads a ROM.
	 *
	 * \param _vRom The in-memory ROM file.
	 * \param _s16Path The full path to the ROM.
	 * \param _pmRegion The region to use when loading the ROM.  If LSN_PM_UNKNOWN, the ROM data is used to determine the region.
	 * \return Returns true if the ROM was loaded successfully.
	 */
	bool CBeesNes::LoadRom( const std::vector<uint8_t> &_vRom, const std::u16string &_s16Path, LSN_PPU_METRICS _pmRegion ) {
		CloseRom();
		LSN_ROM rTmp;
		if ( m_u16PerGameSettings.size() ) {
			SavePerGameSettings( m_u16PerGameSettings );
			m_u16PerGameSettings.clear();
		}
		if ( CSystemBase::LoadRom( _vRom, rTmp, _s16Path ) ) {
			//m_psbSystem.reset();
			LSN_PPU_METRICS pmReg = _pmRegion;
			m_pmSystem = pmReg;
			if ( m_pmSystem == LSN_PPU_METRICS::LSN_PM_UNKNOWN ) {
				m_pmSystem = rTmp.riInfo.pmConsoleRegion;
			}
			if ( m_pmSystem == LSN_PPU_METRICS::LSN_PM_UNKNOWN ) {
				m_pmSystem = LSN_PPU_METRICS::LSN_PM_NTSC_M;
			}
			for ( auto I = std::size( m_psbSystems ); I--; ) {
				if ( I != size_t( m_pmSystem ) ) {
					m_psbSystems[I]->SetAsInactive();
				}
			}
			m_psbSystem = m_psbSystems[m_pmSystem];
			rTmp.wsSaveFolder = m_wsFolder + L"GameSettings\\";
			rTmp.u16SaveFilePrefix = CUtilities::PerRomSettingsPath( rTmp.wsSaveFolder, rTmp.riInfo.ui32Crc, rTmp.riInfo.s16RomName );
			m_u16PerGameSettings = rTmp.u16SaveFilePrefix;
			if ( rTmp.u16SaveFilePrefix.size() ) {
				std::filesystem::path pPath( rTmp.u16SaveFilePrefix );
				rTmp.u16SaveFilePrefix = pPath.replace_extension( "" ).generic_u16string();
			}
			
			LoadPerGameSettings( m_u16PerGameSettings );
			ApplyFilterOptions();
			UpdateCurrentSystem();
			if ( m_psbSystem->LoadRom( rTmp ) ) {
				if LSN_LIKELY( m_psbSystem->GetRom() ) {
					m_cmCheatManager.AutoSetCheatFilter( m_psbSystem->GetRom()->riInfo.s16RomName );
				}
				//m_psbSystem->ResetState( false );
				m_trTemperatureReader.StartThread();
				AddPath( _s16Path );
				return true;
			}
		}
		return false;
	}

	/**
	 * Closes the current ROM.
	 **/
	void CBeesNes::CloseRom() {
		if ( m_psbSystem && !m_psbSystem->CloseRom() ) {
		}
		m_trTemperatureReader.StopThread();
	}

	/**
	 * Pauses the current ROM.
	 **/
	void CBeesNes::PauseRom() {
		for ( auto I = std::size( m_psbSystems ); I--; ) {
			if LSN_LIKELY( m_psbSystems[I] ) {
				m_psbSystems[I]->PauseRom();
			}
		}
	}

	/**
	 * Unpauses the current ROM.
	 **/
	void CBeesNes::UnpauseRom() {
		for ( auto I = std::size( m_psbSystems ); I--; ) {
			if LSN_LIKELY( m_psbSystems[I] ) {
				m_psbSystems[I]->UnpauseRom();
			}
		}
	}

	/**
	 * Toggles the current ROM's pause state.
	 **/
	void CBeesNes::TogglePauseRom() {
		CCpuTemperatureReader ctrTemp;
		lsn::DebugA( std::format( "Temperature: {}\r\n\r\n", ctrTemp.GetTemperature() ).c_str() );
		for ( auto I = std::size( m_psbSystems ); I--; ) {
			if LSN_LIKELY( m_psbSystems[I] ) {
				m_psbSystems[I]->TogglePauseRom();
			}
		}
	}

	/**
	 * Determines whether the ROM is paused or not.
	 * 
	 * \return Returns true if the ROM is paused.
	 **/
	bool CBeesNes::RomIsPaused() const {
		if LSN_LIKELY( m_psbSystem ) {
			return m_psbSystem->RomIsPaused();
		}
		return true;
	}

	/**
	 * Resets the ROM.
	 **/
	void CBeesNes::ResetRom() {
		if LSN_LIKELY( m_psbSystem ) {
			m_psbSystem->ResetRom();
		}
	}

	/**
	 * Resets the ROM.
	 **/
	void CBeesNes::PowerCycle() {
		if LSN_LIKELY( m_psbSystem ) {
			m_psbSystem->PowerCycle();
		}
	}

	/**
	 * Applies controller settings to the applied input peripherals.  Pointers to the given USB controllers can be stored but
	 *	must be cleared when DestroyControllers() is called.
	 * 
	 * \param _vControllers The registers controllers.
	 * \param PARM DESC
	 * \return Returns true if all inputs were assigned correctly.
	 **/
	bool CBeesNes::ApplyInputs( const std::vector<lsn::CUsbControllerBase *> &_vControllers ) {
		IPeripheralBase::LSN_INPUT_CONFIGURATION_APPLICATION_RECORD icarRecord;
		bool bSuccess = true;
		// Apply inputs with a 2-pass approach.
		// Pass 1: Peripherals look for specifically the USB controller they are assigned.
		for ( size_t I = 0; I < m_vPeripherals.size(); ++I ) {
			if LSN_UNLIKELY( m_vPeripherals[I].get() ) {
				auto * pieEvents = m_oOptions.ioThisGameInputOptions.bUseGlobal ? &m_oOptions.ioGlobalInputOptions.ieButtonMap[I] : &m_oOptions.ioThisGameInputOptions.ieButtonMap[I];
				auto * pieTurboEvents = m_oOptions.ioThisGameInputOptions.bUseGlobal ? &m_oOptions.ioGlobalInputOptions.ieTurboButtonMap[I] : &m_oOptions.ioThisGameInputOptions.ieTurboButtonMap[I];
				if ( !m_vPeripherals[I]->ApplyConfiguration( (*pieEvents), (*pieTurboEvents), _vControllers, icarRecord, true ) ) {
					bSuccess = false;
				}
			}
		}

		// Pass 2: Peripherals look for any USB controller that is of the same type as when controls were assigned.
		for ( size_t I = 0; I < m_vPeripherals.size(); ++I ) {
			if LSN_UNLIKELY( m_vPeripherals[I].get() ) {
				auto * pieEvents = m_oOptions.ioThisGameInputOptions.bUseGlobal ? &m_oOptions.ioGlobalInputOptions.ieButtonMap[I] : &m_oOptions.ioThisGameInputOptions.ieButtonMap[I];
				auto * pieTurboEvents = m_oOptions.ioThisGameInputOptions.bUseGlobal ? &m_oOptions.ioGlobalInputOptions.ieTurboButtonMap[I] : &m_oOptions.ioThisGameInputOptions.ieTurboButtonMap[I];
				if ( !m_vPeripherals[I]->ApplyConfiguration( (*pieEvents), (*pieTurboEvents), _vControllers, icarRecord, false ) ) {
					bSuccess = false;
				}
			}
		}
		return bSuccess;
	}

	/**
	 * Called when the source controller storage is about to be deleted.  Any references to them should be removed.
	 **/
	void CBeesNes::DestroyControllers() {
		for ( auto I = m_vPeripherals.size(); I--; ) {
			if LSN_UNLIKELY( m_vPeripherals[I].get() ) {
				m_vPeripherals[I]->ControllerDetached();
			}
		}
	}

	/**
	 * Updates inputs based off settings. Peripherals are created and buttons are assigned (ApplyInputs() is called).
	 * 
	 * \param _vControllers The registered controllers.
	 * \return Returns true if the peripherals were allocated and buttons assigned.
	 **/
	bool CBeesNes::UpdatePeripherals( const std::vector<lsn::CUsbControllerBase *> &_vControllers ) {
		auto * pioOptions = m_oOptions.ioThisGameInputOptions.bUseGlobal ? &m_oOptions.ioGlobalInputOptions : &m_oOptions.ioThisGameInputOptions;
		try {
			m_vPeripherals.clear();	// Remove existing controllers.
			m_vPeripherals.resize( std::size( pioOptions->ui8Player ) );
			for ( size_t I = 0; I < std::size( pioOptions->ui8Player ); ++I ) {
				switch ( pioOptions->ui8Player[I] ) {
					case LSN_CT_STANDARD : {
						m_vPeripherals[I] = std::make_unique<CStdController>();
						break;
					}
					default : {}
				}
			}
			return ApplyInputs( _vControllers );
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Handles writes to $4016.
	 * 
	 * \param _ui8Value The value being written.  If it is 1, inputs are polled.
	 * \param _bPoll If false, the inputs are not read and the peripherals report that nothing is pressed.
	 **/
	void CBeesNes::Write4016( uint8_t _ui8Value, bool _bPoll ) {
		if LSN_LIKELY( m_vPeripherals[0].get() ) {
			m_vPeripherals[0]->Write( _ui8Value, _bPoll );
		}
		if ( m_vPeripherals[1].get() ) {
			m_vPeripherals[1]->Write( _ui8Value, _bPoll );
		}
	}

	 /**
	  * Handles reads from $4016. Passes off to peripherals to get the return value.
	  * 
	  * \return Returns the value to be returned from reading register $4016.
	  **/
	uint8_t CBeesNes::Read4016() {
		if LSN_LIKELY( m_vPeripherals[0].get() ) {
			return m_vPeripherals[0]->Read();
		}
		return 0;
	}

	/**
	  * Handles reads from $4017. Passes off to peripherals to get the return value.
	  * 
	  * \return Returns the value to be returned from reading register $4017.
	  **/
	uint8_t CBeesNes::Read4017() {
		if ( m_vPeripherals[1].get() ) {
			return m_vPeripherals[1]->Read();
		}
		return 0;
	}

	/**
	 * Applies the current audio options.
	 **/
	void CBeesNes::ApplyAudioOptions() {
		LSN_AUDIO_OPTIONS & aoOptions = Options().aoThisGameAudioOptions.bUseGlobal ? Options().aoGlobalAudioOptions : Options().aoThisGameAudioOptions;
		CAudio::SetOutputSettings( aoOptions );
		if ( m_psbSystem ) {
			Options().stfStreamOptionsOutCapture.ui32Hz = aoOptions.ui32OutputHz;
			Options().stfStreamOptionsRaw.ui32Hz = uint32_t( std::ceil( m_psbSystem->GetApuHz() ) );
			Options().stfStreamOptionsOutCapture.wsMetaPath = Options().stfStreamOptionsOutCapture.wsPath + L".txt";
			Options().stfStreamOptionsRaw.wsMetaPath = Options().stfStreamOptionsRaw.wsPath + L".txt";

			auto pfTmp = Options().stfStreamOptionsOutCapture.pfMetaFunc;
			Options().stfStreamOptionsOutCapture.pfMetaFunc = nullptr;
			Options().stfStreamOptionsOutCapture.pfMetaThreadFunc = nullptr;
			//m_psbSystem->SetMetaDataStreamParms( Options().stfStreamOptionsOutCapture.pvMetaParm, Options().stfStreamOptionsOutCapture.pfMetaFunc, pfTmp, Options().stfStreamOptionsOutCapture.pfMetaThreadFunc );
			m_psbSystem->SetMetaDataStreamParms( Options().stfStreamOptionsRaw.pvMetaParm, pfTmp, Options().stfStreamOptionsRaw.pfMetaFunc, Options().stfStreamOptionsRaw.pfMetaThreadFunc );
			m_psbSystem->SetAudioOptions( aoOptions );
			SetStreamToFileOptions<true>( Options().stfStreamOptionsRaw );
			SetStreamToFileOptions<false>( Options().stfStreamOptionsOutCapture );

			m_psbSystem->SetRawStream( Options().stfStreamOptionsRaw.bEnabled ? &m_wfRawStream : nullptr );
			m_psbSystem->SetOutStream( Options().stfStreamOptionsOutCapture.bEnabled ? &m_wfOutStream : nullptr );
		}

		if ( m_u16PerGameSettings.size() ) {
			SavePerGameSettings( m_u16PerGameSettings );
		}
	}

	/**
	 * Gets the default folder for cheats.
	 * 
	 * \return Returns the default folder where default cheats can be found.
	 **/
	std::wstring CBeesNes::DefaultCheatsFolder() {
#if defined( _WIN32 )
		std::wstring wsBuffer;
		const DWORD dwSize = 0xFFFF;
		wsBuffer.resize( dwSize + 1 );
		::GetModuleFileNameW( NULL, wsBuffer.data(), dwSize );
		PWSTR pwsEnd = std::wcsrchr( wsBuffer.data(), L'\\' ) + 1;
		std::wstring wsRoot = wsBuffer.substr( 0, pwsEnd - wsBuffer.data() );
		return wsRoot + L"Cheats\\";
#else
		char szBuffer[PATH_MAX];
		std::string sPathStr;

#if defined( __APPLE__ )
		uint32_t uiSize = sizeof( szBuffer );
		if ( ::_NSGetExecutablePath( szBuffer, &uiSize ) == 0 ) {
			char szRealPath[PATH_MAX];
			if ( ::realpath( szBuffer, szRealPath ) != nullptr ) {
				sPathStr = szRealPath;
			}
			else {
				sPathStr = szBuffer;
			}
		}
#elif defined( __linux__ )
		ssize_t sLen = ::readlink( "/proc/self/exe", szBuffer, sizeof( szBuffer ) - 1 );
		if ( sLen != -1 ) {
			szBuffer[sLen] = '\0';
			sPathStr = szBuffer;
		}
#endif

		size_t stPos = sPathStr.find_last_of( '/' );
		std::string sDir = ( stPos != std::string::npos ) ? sPathStr.substr( 0, stPos + 1 ) : "";
		sDir += "Cheats/";

		return std::wstring( sDir.begin(), sDir.end() );
#endif
	}

	/**
	 * Applies the current palette.
	 * 
	 * \return Returns true if a palette was opened.
	 **/
	bool CBeesNes::ApplyPaletteOptions() {
		const LSN_PALETTE_OPTIONS * ppoOptions = Options().poThisGamePalette[m_pmSystem].bUseGlobal ? &Options().poGlobalPalettes[m_pmSystem] : &Options().poThisGamePalette[m_pmSystem];

		if ( m_psbSystem ) {
			std::wstring wsPath = ppoOptions->wsPath;
			if ( wsPath.empty() || !m_npPalette.LoadFromFile( wsPath ) ) {
				wsPath.clear();
				switch ( m_pmSystem ) {
					case LSN_PM_NTSC_M : {}				LSN_FALLTHROUGH
					case LSN_PM_NTSC_J : {
						wsPath = DefaultNtscPalette();
						//wsPath = DefaultPaletteFolder() + L"SonyCXA2060BS_JP.pal";
						break;
					}
					case LSN_PM_PAL : {
						wsPath = DefaultPalPalette();
						break;
					}
					case LSN_PM_DENDY : {
						wsPath = DefaultPalPalette();
						break;
					}
					case LSN_PM_PALM : {
						wsPath = DefaultPalPalette();
						break;
					}
					case LSN_PM_PALN : {
						wsPath = DefaultPalPalette();
						break;
					}
				}
				if ( wsPath.empty() || !m_npPalette.LoadFromFile( wsPath ) ) {
					// If this is a per-game option, try the global option.
					if ( !Options().poThisGamePalette[m_pmSystem].bUseGlobal ) {
						ppoOptions = &Options().poGlobalPalettes[m_pmSystem];
						wsPath = ppoOptions->wsPath;
						if ( wsPath.empty() || !m_npPalette.LoadFromFile( wsPath ) ) {
							// Defaults were already attempted.  Nothing we can do.
							return false;
						}
					}
					else { return false; }
				}
			}
			// Success path.  Put the palette where it needs to go.
			std::memcpy( m_gCrtGamma, ppoOptions->gCrtGamma, sizeof( m_gCrtGamma ) );
			// The monitor's gamma curve is a host-display setting (LSN_PALETTE_OPTIONS::gMonitorGamma is no longer used).
			m_gMonitorGamma = m_oOptions.hdoHostDisplay.dsDisplay.gGamma;

			// The CRT gamma curve, the CRT color space, and the monitor gamma curve are baked into the software palette.
			lsn::LSN_PALETTE * ppPal = m_psbSystem->Palette();
			bool bRet = true;
			if ( ppPal ) {
				CNesPalette::PaletteF64 pBaked = BakedPalette( true );
				if ( pBaked.size() != 512 ) { bRet = false; }
				else {
					for ( size_t I = 0; I < 512; ++I ) {
						ppPal->uVals[I].sRgb.ui8R = uint8_t( std::round( std::clamp( pBaked[I].x[2], 0.0, 1.0 ) * 255.0 ) );
						ppPal->uVals[I].sRgb.ui8G = uint8_t( std::round( std::clamp( pBaked[I].x[1], 0.0, 1.0 ) * 255.0 ) );
						ppPal->uVals[I].sRgb.ui8B = uint8_t( std::round( std::clamp( pBaked[I].x[0], 0.0, 1.0 ) * 255.0 ) );
					}
				}
			}
			// The GPU palette filters get the same palette in linear light.
			UpdateGpuPalette();

			return bRet;
		}
		return false;
	}

	/**
	 * Gets the loaded palette with the CRT gamma curve (and its CRT1 parameters), the CRT color space, and (optionally) the monitor
	 *	gamma curve baked in, so that the palette filters need no gamma or color-space pass at run time.
	 * 
	 * \param _bEncode If true, the colors are encoded with the monitor gamma curve (for the software palette); otherwise they are
	 *	left in linear light (for the GPU palette filters, which encode them at the end of their pipelines).
	 * \return Returns the 512 colors of the palette in the color space of the monitor, or an empty vector if no palette is loaded.
	 **/
	CNesPalette::PaletteF64 CBeesNes::BakedPalette( bool _bEncode ) {
		const CNesPalette::PaletteF64 & pSrc = m_npPalette.Palette();
		if ( pSrc.empty() ) { return CNesPalette::PaletteF64(); }
		const LSN_PPU_METRICS pmRegion = size_t( m_pmSystem ) < LSN_PM_CONSOLE_TOTAL ? m_pmSystem : LSN_PM_NTSC_M;
		const LSN_GPU_FILTER_OPTIONS & gfoPalette = FilterOptions( pmRegion ).gfoPalette;
		const CColorSpace::LSN_DISPLAY_SETTINGS & dsDisplay = m_oOptions.hdoHostDisplay.dsDisplay;

		// From the CRT's color space to the monitor's.
		const CColorSpace::LSN_COLOR_MATRIX cmMatrix = CColorSpace::ConversionMatrix( gfoPalette.cspColorSpace, dsDisplay.cspColorSpace, dsDisplay.waWhiteAdaptation );
		float fLuma[3];
		CColorSpace::LuminanceCoefficients( dsDisplay.cspColorSpace, fLuma );
		// Automatic means sRGB for the monitor (as in CColorSpace::OutputTransformStatement()).
		const CNesPalette::LSN_GAMMA gMonitor = CColorSpace::IsMonitorGamma( dsDisplay.gGamma ) ? dsDisplay.gGamma : CNesPalette::LSN_G_sRGB;
		// The CRT1 curve's parameters come from the palette filter's settings.
		m_npPalette.SetCrt1Parms( gfoPalette.fCrt1White, gfoPalette.fCrt1Brightness );

		CNesPalette::PaletteF64 vRet;
		try {
			vRet.resize( 512 );
			for ( size_t I = 0; I < 512; ++I ) {
				// The CRT gamma curve (to linear light).
				CNesPalette::Float64_3 fColor = m_npPalette.UpdateEntry( pSrc[I%pSrc.size()], m_gCrtGamma[pmRegion], CNesPalette::LSN_G_NONE );
				// The color space (and gamut mapping).
				CColorSpace::ConvertLinear( cmMatrix, fLuma, dsDisplay.gmGamutMapping, fColor.x );
				if ( _bEncode ) {
					// The monitor's gamma curve.
					for ( auto & dThis : fColor.x ) { dThis = std::clamp( dThis, 0.0, 1.0 ); }
					fColor = m_npPalette.UpdateEntry( fColor, CNesPalette::LSN_G_NONE, gMonitor );
				}
				vRet[I] = fColor;
			}
		}
		catch ( ... ) { return CNesPalette::PaletteF64(); }
		return vRet;
	}

	/**
	 * Gets the default folder for palettes.
	 * 
	 * \return Returns the default folder where default palettes can be found.
	 **/
	std::wstring CBeesNes::DefaultPaletteFolder() {
#if defined( _WIN32 )
		std::wstring wsBuffer;
		const DWORD dwSize = 0xFFFF;
		wsBuffer.resize( dwSize + 1 );
		::GetModuleFileNameW( NULL, wsBuffer.data(), dwSize );
		PWSTR pwsEnd = std::wcsrchr( wsBuffer.data(), L'\\' ) + 1;
		std::wstring wsRoot = wsBuffer.substr( 0, pwsEnd - wsBuffer.data() );
		return wsRoot + L"Palettes\\";
#else
		char szBuffer[PATH_MAX];
		std::string sPathStr;

#if defined( __APPLE__ )
		uint32_t uiSize = sizeof( szBuffer );
		if ( ::_NSGetExecutablePath( szBuffer, &uiSize ) == 0 ) {
			char szRealPath[PATH_MAX];
			if ( ::realpath( szBuffer, szRealPath ) != nullptr ) {
				sPathStr = szRealPath;
			}
			else {
				sPathStr = szBuffer;
			}
		}
#elif defined( __linux__ )
		ssize_t sLen = ::readlink( "/proc/self/exe", szBuffer, sizeof( szBuffer ) - 1 );
		if ( sLen != -1 ) {
			szBuffer[sLen] = '\0';
			sPathStr = szBuffer;
		}
#endif

		size_t stPos = sPathStr.find_last_of( '/' );
		std::string sDir = ( stPos != std::string::npos ) ? sPathStr.substr( 0, stPos + 1 ) : "";
		sDir += "Palettes/";

		return std::wstring( sDir.begin(), sDir.end() );
#endif
	}

	/**
	 * Gets the default folder for exported movies.
	 * 
	 * \return Returns the default folder into which movies are exported.
	 **/
	std::wstring CBeesNes::DefaultMoviesFolder() {
#if defined( _WIN32 )
		std::wstring wsBuffer;
		const DWORD dwSize = 0xFFFF;
		wsBuffer.resize( dwSize + 1 );
		::GetModuleFileNameW( NULL, wsBuffer.data(), dwSize );
		PWSTR pwsEnd = std::wcsrchr( wsBuffer.data(), L'\\' ) + 1;
		std::wstring wsRoot = wsBuffer.substr( 0, pwsEnd - wsBuffer.data() );
		return wsRoot + L"Movies\\";
#else
		char szBuffer[PATH_MAX];
		std::string sPathStr;

#if defined( __APPLE__ )
		uint32_t uiSize = sizeof( szBuffer );
		if ( ::_NSGetExecutablePath( szBuffer, &uiSize ) == 0 ) {
			char szRealPath[PATH_MAX];
			if ( ::realpath( szBuffer, szRealPath ) != nullptr ) {
				sPathStr = szRealPath;
			}
			else {
				sPathStr = szBuffer;
			}
		}
#elif defined( __linux__ )
		ssize_t sLen = ::readlink( "/proc/self/exe", szBuffer, sizeof( szBuffer ) - 1 );
		if ( sLen != -1 ) {
			szBuffer[sLen] = '\0';
			sPathStr = szBuffer;
		}
#endif

		size_t stPos = sPathStr.find_last_of( '/' );
		std::string sDir = ( stPos != std::string::npos ) ? sPathStr.substr( 0, stPos + 1 ) : "";
		sDir += "Movies/";

		return std::wstring( sDir.begin(), sDir.end() );
#endif
	}

	/**
	 * Gets the folder into which movies are exported:  the folder set in the video-export options, or the default Movies folder if none
	 *	is set.
	 * 
	 * \return Returns the movie folder, including the trailing separator.
	 **/
	std::wstring CBeesNes::MovieFolder() const {
		const std::wstring &wsFolder = m_oOptions.veoVideoExport.wsOutputFolder;
		if ( wsFolder.empty() ) { return DefaultMoviesFolder(); }
		wchar_t wcLast = wsFolder.back();
		if ( wcLast == L'\\' || wcLast == L'/' ) { return wsFolder; }
#if defined( _WIN32 )
		return wsFolder + L"\\";
#else
		return wsFolder + L"/";
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Gets the frame rate of the current console.
	 * 
	 * \return Returns the frame rate of the current console as a fraction.
	 **/
	CVideoExporter::LSN_RATE CBeesNes::ConsoleFrameRate() const {
		return CVideoExporter::RegionFrameRate( m_pmSystem );
	}

	/**
	 * Generates a movie file name from the current local date and time, in the form BeesNES_DD-MM-YYYY_HH-MM-SS.  The name has no
	 *	folder and no extension.
	 * 
	 * \return Returns the generated movie file name.
	 **/
	std::wstring CBeesNes::MovieFileName() const {
		std::time_t tNow = std::time( nullptr );
		std::tm tLocal = {};
#if defined( _WIN32 )
		::localtime_s( &tLocal, &tNow );
#else
		::localtime_r( &tNow, &tLocal );
#endif	// #if defined( _WIN32 )

		wchar_t wszName[64];
		std::swprintf( wszName, std::size( wszName ), L"BeesNES_%02d-%02d-%04d_%02d-%02d-%02d",
			tLocal.tm_mday, tLocal.tm_mon + 1, tLocal.tm_year + 1900,
			tLocal.tm_hour, tLocal.tm_min, tLocal.tm_sec );
		return std::wstring( wszName );
	}

	/**
	 * Prepares the path to which to export a new movie.  The output folder of the current export (see MovieFolder()) is created if it
	 *	does not exist, and the returned path (folder, MovieFileName(), and the extension of the current export's video format) names a
	 *	file that does not yet exist.  Called on the emulation thread, so it uses only the copies of the options made when exporting started.
	 * 
	 * \return Returns the full path of the new movie file or an empty string if the output folder could not be created.
	 **/
	std::wstring CBeesNes::PrepareMoviePath() const {
		const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( m_vfExportFormat );
		if ( !pvfdData ) { pvfdData = CVideoFormat::Data( LSN_VF_H264 ); }

		const std::wstring &wsFolder = m_wsExportFolder;
		if ( wsFolder.empty() ) { return std::wstring(); }
		std::error_code ecErr;
		std::filesystem::create_directories( std::filesystem::path( wsFolder ), ecErr );
		if ( !std::filesystem::is_directory( std::filesystem::path( wsFolder ), ecErr ) ) { return std::wstring(); }

		// Two recordings started within the same second get a suffix.
		std::wstring wsBase = wsFolder + MovieFileName();
		std::wstring wsPath = wsBase + (*pvfdData).pwcFileExtension;
		for ( uint32_t I = 2; std::filesystem::exists( std::filesystem::path( wsPath ), ecErr ) && I < 1000; ++I ) {
			wsPath = wsBase + L"_" + std::to_wstring( I ) + (*pvfdData).pwcFileExtension;
		}
		return wsPath;
	}

	/**
	 * Starts exporting a movie with the current video-export options:  checks FFmpeg and the selected format, starts capturing
	 *	rendered frames, and starts receiving the APU's output samples.  Must be called while the emulation thread is stopped, from the
	 *	thread that calls Render().
	 * 
	 * \param _wsError Receives an error message on failure.
	 * \return Returns true if exporting started.
	 **/
	bool CBeesNes::StartVideoExport( std::wstring &_wsError ) {
		_wsError.clear();
		if ( m_veExporter.IsActive() ) { return true; }

		// Copy everything needed while exporting so that the options can change meanwhile.
		const LSN_VIDEO_EXPORT_OPTIONS &veoOptions = m_oOptions.veoVideoExport;
		try {
			m_wsExportFolder = MovieFolder();
		}
		catch ( ... ) {
			_wsError = LSN_LSTR( LSN_OUT_OF_MEMORY );
			return false;
		}
		m_vfExportFormat = veoOptions.vfFormat;
		m_scExportStart = veoOptions.scStart < LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_TOTAL ? veoOptions.scStart : LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_NONE;
		m_ecExportEnd = veoOptions.ecEnd < LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_TOTAL ? veoOptions.ecEnd : LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_NONE;
		m_ui64ExportStartFrame = veoOptions.ui64StartFrame;
		m_ui64ExportEndFrame = veoOptions.ui64EndFrame;
		m_ui64ExportDurationFrames = 0;
		if ( m_ecExportEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_DURATION ) {
			// The length is measured in emulated frames:  changing the output frame rate does not change the length of a movie.
			double dFrames = std::round( veoOptions.dEndSeconds * CVideoExporter::RegionFrameRate( m_pmSystem ).Value() );
			m_ui64ExportDurationFrames = dFrames >= 1.0 ? (dFrames < 1.0e18 ? uint64_t( dFrames ) : uint64_t( 1.0e18 )) : 1;
		}
		m_ui64ExportSubmitted = 0;
		m_bExportVideoStarted = false;
		m_abExportStarted = m_scExportStart == LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_NONE;
		m_abExportEndReached = false;

		if ( !m_veExporter.Start( veoOptions, CVideoExporter::RegionFrameRate( m_pmSystem ), AudioOutputHz(),
			ExportMoviePath, this, _wsError ) ) { return false; }
		m_aui64ExportSwapId = 0;
		m_aui64ExportDoneId = 0;
		m_ui64ExportRenderedId = 0;
		m_bExportRenderPending = false;
		m_ui32ExportMissedCaptures = 0;
		m_ui64ExportDroppedFrames = 0;
		m_ui64ExportFirstDropped = 0;
		m_abExportAbort = false;
		SetVideoCapture( true );
		m_ui64ExportLastCapture = m_cbVideoCapture.ui64Captures;
		m_ui64ExportLastPpuFrame = ~0ULL;
		AttachExportApu( true );
		return true;
	}

	/**
	 * Stops exporting a movie and waits for FFmpeg to finish the file.  Must be called while the emulation thread is stopped, from the
	 *	thread that calls Render().
	 * 
	 * \param _wsError Receives an error message on failure (including an earlier failure that ended the export).
	 * \return Returns true if every movie file was finished without error.
	 **/
	bool CBeesNes::StopVideoExport( std::wstring &_wsError ) {
		_wsError.clear();
		if ( !m_veExporter.IsActive() ) { return true; }
		AttachExportApu( false );
		// GPU captures can arrive a frame late; get the last rendered frame before capturing stops.
		if ( m_cfartCurFilterAndTargets.pfbPrevFilter && m_cfartCurFilterAndTargets.pfbPrevFilter->IsGpuFilter() ) {
			static_cast<CGpuFilterBase *>(m_cfartCurFilterAndTargets.pfbPrevFilter)->FlushCapture();
			ExportLatestCapture();
		}
		SetVideoCapture( false );
		m_bExportRenderPending = false;
		bool bRet = m_veExporter.Stop( _wsError );
		if ( bRet && m_ui64ExportDroppedFrames ) {
			// Should never happen; if it does, say so rather than leave a gap in the movie unnoticed.
			_wsError = std::format( LSN_LSTR( LSN_VE_FRAMES_DROPPED ), m_ui64ExportDroppedFrames, m_ui64ExportFirstDropped );
		}
		return bRet;
	}

	/**
	 * Determines whether the emulation thread is waiting for a frame to be rendered so that it can be exported.
	 * 
	 * \return Returns true if exporting and a swapped frame has not yet been rendered.
	 **/
	bool CBeesNes::ExportFramePending() const {
		return m_veExporter.IsActive() && !m_veExporter.Failed() && m_aui64ExportDoneId < m_aui64ExportSwapId;
	}

	/**
	 * Gives the most recently captured frame to the exporter if it is new, and keeps track of any PPU frames that were never captured.
	 * 
	 * \return Returns true if a new capture was found.
	 **/
	bool CBeesNes::ExportLatestCapture() {
		if ( m_cbVideoCapture.ui64Captures == m_ui64ExportLastCapture ) { return false; }
		m_ui64ExportLastCapture = m_cbVideoCapture.ui64Captures;
		// GPU captures can arrive a frame late and a re-render of the same frame captures it again; each PPU frame is exported once.
		uint64_t ui64Frame = m_cbVideoCapture.ui64PpuFrame;
		if ( ui64Frame != m_ui64ExportLastPpuFrame && ExportFrameWanted( ui64Frame ) ) {
			// Every frame the PPU makes is exported, so frame numbers must be consecutive (they restart from 0 when a ROM is loaded or reset).
			if ( m_ui64ExportLastPpuFrame != ~0ULL && ui64Frame > m_ui64ExportLastPpuFrame + 1 ) {
				if ( !m_ui64ExportDroppedFrames ) { m_ui64ExportFirstDropped = m_ui64ExportLastPpuFrame + 1; }
				m_ui64ExportDroppedFrames += ui64Frame - m_ui64ExportLastPpuFrame - 1;
			}
			m_ui64ExportLastPpuFrame = ui64Frame;
			m_veExporter.SubmitFrame( m_cbVideoCapture );
			m_bExportVideoStarted = true;
			++m_ui64ExportSubmitted;
			if ( m_ecExportEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_DURATION && m_ui64ExportSubmitted >= m_ui64ExportDurationFrames ) {
				m_abExportEndReached = true;
			}
		}
		return true;
	}

	/**
	 * Determines whether a captured frame belongs in the movie according to the start and end conditions.  Called on the rendering
	 *	thread for each new captured frame.  Sets m_abExportEndReached once the end frame is reached.
	 * 
	 * \param _ui64Frame The PPU frame that was captured.
	 * \return Returns true if the frame is to be exported.
	 **/
	bool CBeesNes::ExportFrameWanted( uint64_t _ui64Frame ) {
		if ( m_abExportEndReached ) { return false; }
		if ( !m_bExportVideoStarted ) {
			// The emulation thread decides when the start frame has been reached (it discards the audio until then).  A GPU capture
			//	that arrives a frame late can belong to the frame before the start frame, so the frame number is checked too.
			if ( !m_abExportStarted ) { return false; }
			if ( m_scExportStart == LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_START_AT_FRAME && _ui64Frame < m_ui64ExportStartFrame ) { return false; }
		}
		if ( m_ecExportEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_END_AT_FRAME && _ui64Frame >= m_ui64ExportEndFrame ) {
			m_abExportEndReached = true;
			return false;
		}
		return true;
	}

	/**
	 * Called on the emulation thread at the end of each frame (before the frame is swapped) while exporting.  Waits until the previous
	 *	frame has been rendered and captured (so that no frame is ever skipped), then passes the captured frames and audio to FFmpeg,
	 *	waiting if FFmpeg is behind.
	 * 
	 * \return Returns false if the export has failed or its end condition has been met; call StopVideoExport() (from the rendering
	 *	thread) to finish it and get any error.
	 **/
	bool CBeesNes::VideoExportFrameBoundary() {
		if ( !m_veExporter.IsActive() ) { return true; }
		if ( m_veExporter.Failed() ) { return false; }

		// Normally the renderer only shows the newest frame, skipping any that were replaced before it got to them.  While exporting, each
		//	frame must be rendered (and captured) before the next one replaces it, so the emulation waits here instead.
		{
			std::unique_lock<std::mutex> ulLock( m_mExportWait );
			while ( m_aui64ExportDoneId < m_aui64ExportSwapId && !m_abExportAbort ) {
				m_cvExportWait.wait_for( ulLock, std::chrono::milliseconds( 10 ) );
			}
		}

		// The system (and so the APU) changes when a ROM for another region is loaded.
		if ( m_psbSystem && m_psbSystem->GetApu() != m_pabExportApu ) { AttachExportApu( true ); }

		if ( !m_veExporter.FrameBoundary( CVideoExporter::RegionFrameRate( m_pmSystem ), AudioOutputHz() ) ) {
			// Stop collecting samples; the rendering thread reports the error and stops the export.
			AttachExportApu( false );
			return false;
		}

		if ( !m_abExportStarted ) {
			// Waiting for the start frame.  The frame that just ended is the one about to be swapped and rendered.
			if ( GetDisplayClient() && GetDisplayClient()->FrameCount() >= m_ui64ExportStartFrame ) {
				m_abExportStarted = true;
			}
			else {
				// Its audio is not part of the movie.
				m_veExporter.DiscardStagedAudio();
			}
		}
		if ( m_abExportEndReached ) {
			// The end condition was met:  stop collecting samples; the rendering thread stops the export.
			AttachExportApu( false );
			return false;
		}
		return true;
	}

	/**
	 * Called on the rendering thread after each Render() (and after any capture that follows it, such as
	 *	CDx9FilterBase::CaptureBackBuffer()).  Passes the newly captured frame to the exporter and lets the emulation thread continue.
	 **/
	void CBeesNes::FinishRenderedFrame() {
		if ( !m_bExportRenderPending ) { return; }
		m_bExportRenderPending = false;
		if ( ExportLatestCapture() ) {
			m_ui32ExportMissedCaptures = 0;
		}
		else if ( m_ui32ExportMissedCaptures < 120 ) {
			if ( ++m_ui32ExportMissedCaptures == 120 ) {
				// Frames are being rendered but not captured.  That can happen for a while (such as when a minimized window leaves nothing to
				//	capture), but if nothing at all has been captured since the export started then capturing does not work with this renderer:
				//	report it (with the renderer's reason, if it gave one) rather than silently making nothing.
				std::wstring wsReason;
				if ( m_cfartCurFilterAndTargets.pfbPrevFilter && m_cfartCurFilterAndTargets.pfbPrevFilter->IsGpuFilter() ) {
					wsReason = static_cast<CGpuFilterBase *>(m_cfartCurFilterAndTargets.pfbPrevFilter)->CaptureError();
				}
				if ( m_ui64ExportLastPpuFrame == ~0ULL ) {
					std::wstring wsError = LSN_LSTR( LSN_VE_NO_CAPTURES );
					if ( !wsReason.empty() ) { wsError += L"\r\n\r\n" + wsReason; }
					m_veExporter.ReportError( wsError );
				}
			}
		}
		{
			std::lock_guard<std::mutex> lgLock( m_mExportWait );
			m_aui64ExportDoneId = m_ui64ExportRenderedId;
		}
		m_cvExportWait.notify_all();
	}

	/**
	 * Makes the waits in VideoExportFrameBoundary() return immediately (true) or restores normal waiting (false).  Set while stopping the
	 *	emulation thread.
	 * 
	 * \param _bAbort Whether to abort waits.
	 **/
	void CBeesNes::SetVideoExportAbort( bool _bAbort ) {
		{
			std::lock_guard<std::mutex> lgLock( m_mExportWait );
			m_abExportAbort = _bAbort;
		}
		m_cvExportWait.notify_all();
		m_veExporter.SetAbortWaits( _bAbort );
	}

	/**
	 * Gets the current audio output rate:  the sample box's output rate, or the audio device's rate if the sample box has not been
	 *	initialized.
	 * 
	 * \return Returns the audio output rate in Hz.
	 **/
	uint32_t CBeesNes::AudioOutputHz() {
		CApuBase * pabApu = m_psbSystem ? m_psbSystem->GetApu() : nullptr;
		uint32_t ui32Hz = pabApu ? pabApu->SampleBox().OutputHz() : 0;
		if ( !ui32Hz ) { ui32Hz = CAudio::GetOutputFrequency(); }
		return ui32Hz ? ui32Hz : 48000;
	}

	/**
	 * Generates the path of each new movie file for the exporter.
	 *
	 * \param _pvThis A pointer to this object.
	 * \return Returns the path of a new movie file, or an empty string if the Movies folder could not be created.
	 */
	std::wstring CBeesNes::ExportMoviePath( void * _pvThis ) {
		return static_cast<CBeesNes *>(_pvThis)->PrepareMoviePath();
	}

	/**
	 * Points the current system's APU's sample output at the exporter (or at nothing).
	 *
	 * \param _bAttach If true, the current APU's samples go to the exporter; if false, no APU's samples do.
	 */
	void CBeesNes::AttachExportApu( bool _bAttach ) {
		if ( m_pabExportApu ) {
			m_pabExportApu->SetSampleOutFunc( nullptr, nullptr );
			m_pabExportApu = nullptr;
		}
		if ( _bAttach && m_psbSystem ) {
			m_pabExportApu = m_psbSystem->GetApu();
			if ( m_pabExportApu ) { m_pabExportApu->SetSampleOutFunc( CVideoExporter::AddSample, &m_veExporter ); }
		}
	}

	/**
	 * Updates the GPU palette for GPU-enabled palette rendering.
	 **/
	void CBeesNes::UpdateGpuPalette() {
		// The palette is sent in linear light (the filters encode it for the monitor at the end of their pipelines) with its CRT gamma
		//	curve and color space already applied, so the GPU palette filters skip their gamma passes (see ApplyFilterOptions()).
		CNesPalette::PaletteF64 pBaked = BakedPalette( false );
		if ( pBaked.size() != 512 ) { return; }
		std::vector<CNesPalette::Float32_4> vLut;
		try {
			vLut.resize( 512 );
		}
		catch ( ... ) { return; }
		for ( size_t I = 0; I < 512; ++I ) {
			vLut[I].x[0] = float( pBaked[I].x[0] );
			vLut[I].x[1] = float( pBaked[I].x[1] );
			vLut[I].x[2] = float( pBaked[I].x[2] );
			vLut[I].x[3] = 1.0f;
		}
#ifdef LSN_DX9
		m_d9pfDx9Palette.SetLut( vLut[0].x );
#endif	// #ifdef LSN_DX9

#ifdef LSN_DX12
		m_d12pfDx12Palette.SetLut( vLut[0].x );
#endif	// #ifdef LSN_DX12

#ifdef LSN_VULKAN1
		m_vpfVulkanPalette.SetLut( vLut[0].x );
#endif	// #ifdef LSN_VULKAN1
	}

	/**
	 * Shuts down the emulator.  Needs to happen outside of the destructor in order for some inheritance parts to work properly.
	 **/
	void CBeesNes::ShutDown() {
		if ( m_psbSystem && !m_psbSystem->CloseRom() ) {
		}
		auto pfbThis = m_pfbFilterTable[m_oOptions.fFilter][GetCurPpuRegion()];
		// If something was queued for deactivation, destroy it.
		m_cfartCurFilterAndTargets.DeActivate();

		// Deactivate the current filter.
		pfbThis->DeActivate();
	}

	/**
	 * Updates the current system with render information, display hosts, etc.
	 */
	void CBeesNes::UpdateCurrentSystem() {
		if ( !GetDisplayClient() ) { return; }
		GetDisplayClient()->SetDisplayHost( m_pdhDisplayHost );

		// Set ratios.
		m_dRatioActual = GetDisplayClient()->DisplayRatio();

		// Prepare filters.
		m_cfartCurFilterAndTargets.DeActivate();

		if ( m_cfartCurFilterAndTargets.pfbCurFilter != m_cfartCurFilterAndTargets.pfbNextFilter ) {
			if ( m_cfartCurFilterAndTargets.pfbNextFilter ) {
				m_cfartCurFilterAndTargets.pfbNextFilter->DeActivate();
				m_cfartCurFilterAndTargets.pfbCurFilter = nullptr;
			}
		}

		CFilterBase * pfbNew = m_pfbFilterTable[m_oOptions.fFilter][GetDisplayClient()->PpuRegion()];
		if ( m_cfartCurFilterAndTargets.pfbCurFilter != pfbNew ) {
			if ( m_cfartCurFilterAndTargets.pfbCurFilter ) {
				m_cfartCurFilterAndTargets.pfbCurFilter->DeActivate();
			}
			m_cfartCurFilterAndTargets.pfbCurFilter = pfbNew;
			if ( m_cfartCurFilterAndTargets.pfbCurFilter ) {
				m_cfartCurFilterAndTargets.pfbCurFilter->Activate();
			}
		}


		/*bool bActivate = m_cfartCurFilterAndTargets.pfbCurFilter == nullptr;
		m_cfartCurFilterAndTargets.pfbCurFilter = m_pfbFilterTable[m_oOptions.fFilter][GetDisplayClient()->PpuRegion()];
		if ( bActivate ) {
			m_cfartCurFilterAndTargets.pfbCurFilter->Activate();
		}*/
					
		m_cfartCurFilterAndTargets.pui8CurRenderTarget = m_cfartCurFilterAndTargets.pfbCurFilter->OutputBuffer();
		m_cfartCurFilterAndTargets.ui16Bits = uint16_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputBits() );
		m_cfartCurFilterAndTargets.ui32Width = m_cfartCurFilterAndTargets.pfbCurFilter->OutputWidth();
		m_cfartCurFilterAndTargets.ui32Height = m_cfartCurFilterAndTargets.pfbCurFilter->OutputHeight();
		m_cfartCurFilterAndTargets.ui32Stride = uint32_t( m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride() );
		m_cfartCurFilterAndTargets.ui64Frame = GetDisplayClient()->FrameCount();
		m_cfartCurFilterAndTargets.ui64RenderStartCycle = GetDisplayClient()->GetRenderStartCycle();
		m_cfartCurFilterAndTargets.bDirty = true;
		m_cfartCurFilterAndTargets.bMirrored = m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput();
		m_cfartCurFilterAndTargets.pui8LastFilteredResult = nullptr;

		m_cfartCurFilterAndTargets.pfbNextFilter = m_cfartCurFilterAndTargets.pfbCurFilter;
		m_cfartCurFilterAndTargets.pfbPrevFilter = m_cfartCurFilterAndTargets.pfbCurFilter;

		m_cfartCurFilterAndTargets.pfbCurFilter->Swap();
		GetDisplayClient()->SetRenderTarget( m_cfartCurFilterAndTargets.pfbCurFilter->CurTarget(), m_cfartCurFilterAndTargets.pfbCurFilter->OutputStride(), m_cfartCurFilterAndTargets.pfbCurFilter->InputFormat(), m_cfartCurFilterAndTargets.pfbCurFilter->FlipInput() );

		// Set up input.
		m_psbSystem->SetInputPoller( m_pipPoller );

		ApplyAudioOptions();
		ApplyPaletteOptions();
		
		UpdateNoise();
	}

	/**
	 * Loads the settings file.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings file was loaded.
	 */
	bool CBeesNes::LoadSettings( CStream &_sFile ) {
		// Defaults.
#ifdef LSN_WINDOWS
		m_oOptions.wpMainWindowPlacement = { 0 };
#endif	// #ifdef LSN_WINDOWS

		// Get the file version.
		uint32_t ui32Version;
		if ( !_sFile.ReadUi32( ui32Version ) ) { return false; }

		if ( !LoadInputSettings( ui32Version, _sFile, m_oOptions.ioGlobalInputOptions ) ) { return false; }
		if ( !LoadRecentFiles( ui32Version, _sFile ) ) { return false; }

		if ( !_sFile.ReadStringU16( m_oOptions.wsInRomInitPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wsOutRomInitPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wsPatchInitPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wsDefaultRomPath ) ) { return false; }

		if ( !_sFile.Read( m_oOptions.pmRegion ) ) { return false; }
		if ( ui32Version <= LSN_BEESNES_SETTINGS_VERSION_5_REGIONS ) {
			// NTSC-J was inserted after NTSC-M, so PAL, Dendy, PAL-M, and PAL-N moved up by 1.
			if ( m_oOptions.pmRegion >= 1 && m_oOptions.pmRegion <= 4 ) { m_oOptions.pmRegion = LSN_PPU_METRICS( m_oOptions.pmRegion + 1 ); }
		}
		if ( m_oOptions.pmRegion >= LSN_PM_CONSOLE_TOTAL && m_oOptions.pmRegion != LSN_PM_UNKNOWN ) { m_oOptions.pmRegion = LSN_PM_UNKNOWN; }

		if ( !LoadAudioSettings( ui32Version, _sFile, m_oOptions.aoGlobalAudioOptions ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wsRawAudioPath ) ) { return false; }
		if ( !_sFile.ReadStringU16( m_oOptions.wsOutAudioPath ) ) { return false; }

		if ( !LoadAudioStreamSettings( ui32Version, _sFile, m_oOptions.stfStreamOptionsRaw ) ) { return false; }
		if ( !LoadAudioStreamSettings( ui32Version, _sFile, m_oOptions.stfStreamOptionsOutCapture ) ) { return false; }
		
		try {
			auto aSize = m_oOptions.vRawStartHistory.size();
			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vRawStartHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vRawStartHistory[I] ) ) { return false; }
			}
			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vRawEndHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vRawEndHistory[I] ) ) { return false; }
			}

			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vOutStartHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vOutStartHistory[I] ) ) { return false; }
			}
			if ( !_sFile.Read( aSize ) ) { return false; }
			m_oOptions.vOutEndHistory.resize( aSize );
			for ( size_t I = 0; I < aSize; ++I ) {
				if ( !_sFile.ReadStringU16( m_oOptions.vOutEndHistory[I] ) ) { return false; }
			}
		}
		catch ( ... ) { return false; }

		auto fFilter = m_oOptions.fFilter;
		if ( !_sFile.Read( fFilter ) ) { return false; }
		SetCurFilter( fFilter );

#ifdef LSN_WINDOWS
		if ( !_sFile.Read( m_oOptions.wpMainWindowPlacement ) ) { return false; }
#endif	// #ifdef LSN_WINDOWS
		
		if ( !LoadWavEditorWindowSettings( ui32Version, _sFile, m_oOptions.wewoWavEditorWindow ) ) { return false; }

		if ( !LoadRegionPaletteSettings( ui32Version, _sFile, m_oOptions.poGlobalPalettes, nullptr ) ) { return false; }

		if ( !_sFile.ReadStringU16( m_oOptions.wsPatchOutFolder ) ) { return false; }

		if ( ui32Version >= 1 ) {
			if ( !LoadVideoExportSettings( ui32Version, _sFile, m_oOptions.veoVideoExport ) ) { return false; }
		}
		else {
			m_oOptions.veoVideoExport = LSN_VIDEO_EXPORT_OPTIONS();
		}
		if ( ui32Version >= 3 ) {
			if ( !LoadVideoFilterSettings( ui32Version, _sFile, m_oOptions.vfoGlobalFilters ) ) { return false; }
			if ( !LoadHostDisplaySettings( ui32Version, _sFile, m_oOptions.hdoHostDisplay ) ) { return false; }
		}

		return true;
	}

	/**
	 * Saves the settings file.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings file was saved.
	 */
	bool CBeesNes::SaveSettings( CStream &_sFile ) {
		if ( !_sFile.WriteUi32( LSN_BEESNES_SETTINGS_VERSION ) ) { return false; }
		if ( !SaveInputSettings( _sFile, m_oOptions.ioGlobalInputOptions ) ) { return false; }
		if ( !SaveRecentFiles( _sFile ) ) { return false; }

		if ( !_sFile.WriteStringU16( m_oOptions.wsInRomInitPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wsOutRomInitPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wsPatchInitPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wsDefaultRomPath ) ) { return false; }

		if ( !_sFile.Write( m_oOptions.pmRegion ) ) { return false; }

		if ( !SaveAudioSettings( _sFile, m_oOptions.aoGlobalAudioOptions ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wsRawAudioPath ) ) { return false; }
		if ( !_sFile.WriteStringU16( m_oOptions.wsOutAudioPath ) ) { return false; }
		
		if ( !SaveAudioStreamSettings( _sFile, m_oOptions.stfStreamOptionsRaw ) ) { return false; }
		if ( !SaveAudioStreamSettings( _sFile, m_oOptions.stfStreamOptionsOutCapture ) ) { return false; }
		
		if ( !_sFile.Write( m_oOptions.vRawStartHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vRawStartHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vRawStartHistory[I] ) ) { return false; }
		}
		if ( !_sFile.Write( m_oOptions.vRawEndHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vRawEndHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vRawEndHistory[I] ) ) { return false; }
		}

		if ( !_sFile.Write( m_oOptions.vOutStartHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vOutStartHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vOutStartHistory[I] ) ) { return false; }
		}
		if ( !_sFile.Write( m_oOptions.vOutEndHistory.size() ) ) { return false; }
		for ( size_t I = 0; I < m_oOptions.vOutEndHistory.size(); ++I ) {
			if ( !_sFile.WriteStringU16( m_oOptions.vOutEndHistory[I] ) ) { return false; }
		}

		if ( !_sFile.Write( m_oOptions.fFilter ) ) { return false; }

#ifdef LSN_WINDOWS
		if ( !_sFile.Write( m_oOptions.wpMainWindowPlacement ) ) { return false; }
#endif	// #ifdef LSN_WINDOWS
		
		if ( !SaveWavEditorWindowSettings( _sFile, m_oOptions.wewoWavEditorWindow ) ) { return false; }

		for ( size_t I = 0; I < std::size( m_oOptions.poGlobalPalettes ); ++I ) {
			if ( !SavePaletteSettings( _sFile, m_oOptions.poGlobalPalettes[I] ) ) { return false; }
		}
		
		if ( !_sFile.WriteStringU16( m_oOptions.wsPatchOutFolder ) ) { return false; }

		if ( !SaveVideoExportSettings( _sFile, m_oOptions.veoVideoExport ) ) { return false; }
		if ( !SaveVideoFilterSettings( _sFile, m_oOptions.vfoGlobalFilters ) ) { return false; }
		if ( !SaveHostDisplaySettings( _sFile, m_oOptions.hdoHostDisplay ) ) { return false; }

		return true;
	}

	/**
	 * Loads input settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _ioInputOptions The input options into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadInputSettings( uint32_t _ui32Version, CStream &_sFile, LSN_INPUT_OPTIONS &_ioInputOptions ) {
		std::memset( &_ioInputOptions, 0, sizeof( _ioInputOptions ) );
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8ConsoleType ) ) { return false; }
		if ( _ioInputOptions.ui8ConsoleType >= LSN_CT_TOTAL ) {
			_ioInputOptions.ui8ConsoleType = LSN_CT_NES;
			return false;
		}
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Expansion ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[0] ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[1] ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[2] ) ) { return false; }
		if ( !_sFile.ReadUi8( _ioInputOptions.ui8Player[3] ) ) { return false; }
		if ( !_sFile.ReadBool( _ioInputOptions.bUseFourScore ) ) { return false; }

		for ( size_t I = 0; I < std::size( _ioInputOptions.ieButtonMap ); ++I ) {
			for ( size_t J = 0; J < std::size( _ioInputOptions.ieButtonMap[0] ); ++J ) {
				for ( size_t K = 0; K < std::size( _ioInputOptions.ieButtonMap[0][0] ); ++K ) {
					if ( !LoadInputSettings( _ui32Version, _sFile, _ioInputOptions.ieButtonMap[I][J][K] ) ) { return false; }
				}
			}
		}
		for ( size_t I = 0; I < std::size( _ioInputOptions.ieTurboButtonMap ); ++I ) {
			for ( size_t J = 0; J < std::size( _ioInputOptions.ieTurboButtonMap[0] ); ++J ) {
				for ( size_t K = 0; K < std::size( _ioInputOptions.ieTurboButtonMap[0][0] ); ++K ) {
					if ( !LoadInputSettings( _ui32Version, _sFile, _ioInputOptions.ieTurboButtonMap[I][J][K] ) ) { return false; }
				}
			}
		}
		return true;
	}

	/**
	 * Saves input settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _ioInputOptions The input options to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveInputSettings( CStream &_sFile, const LSN_INPUT_OPTIONS &_ioInputOptions ) {
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8ConsoleType ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Expansion ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[0] ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[1] ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[2] ) ) { return false; }
		if ( !_sFile.WriteUi8( _ioInputOptions.ui8Player[3] ) ) { return false; }
		if ( !_sFile.WriteBool( _ioInputOptions.bUseFourScore ) ) { return false; }

		for ( size_t I = 0; I < std::size( _ioInputOptions.ieButtonMap ); ++I ) {
			for ( size_t J = 0; J < std::size( _ioInputOptions.ieButtonMap[0] ); ++J ) {
				for ( size_t K = 0; K < std::size( _ioInputOptions.ieButtonMap[0][0] ); ++K ) {
					if ( !SaveInputSettings( _sFile, _ioInputOptions.ieButtonMap[I][J][K] ) ) { return false; }
				}
			}
		}
		for ( size_t I = 0; I < std::size( _ioInputOptions.ieTurboButtonMap ); ++I ) {
			for ( size_t J = 0; J < std::size( _ioInputOptions.ieTurboButtonMap[0] ); ++J ) {
				for ( size_t K = 0; K < std::size( _ioInputOptions.ieTurboButtonMap[0][0] ); ++K ) {
					if ( !SaveInputSettings( _sFile, _ioInputOptions.ieTurboButtonMap[I][J][K] ) ) { return false; }
				}
			}
		}
		return true;
	}

	/**
	 * Loads input settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _ieInputOptions The input options into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadInputSettings( uint32_t /*_ui32Version*/, CStream &_sFile, lsn::LSN_INPUT_EVENT &_ieInputOptions ) {
		uint8_t ui8Val;
		if ( !_sFile.Read( ui8Val ) ) { return false; }
		_ieInputOptions.dtType = static_cast<LSN_INPUT_EVENT::LSN_DEVICE_TYPE>(ui8Val);
		switch ( _ieInputOptions.dtType ) {
			case LSN_INPUT_EVENT::LSN_DT_KEYBOARD : {
				if ( !_sFile.Read( _ieInputOptions.u.kb ) ) { return false; }
				break;
			}
			case LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER : {
				if ( !_sFile.Read( _ieInputOptions.u.cont ) ) { return false; }
				break;
			}
		}
		return true;
	}

	/**
	 * Saves input settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _ieInputOptions The input options to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveInputSettings( CStream &_sFile, const lsn::LSN_INPUT_EVENT &_ieInputOptions ) {
		if ( !_sFile.Write( static_cast<uint8_t>(_ieInputOptions.dtType) ) ) { return false; }
		switch ( _ieInputOptions.dtType ) {
			case LSN_INPUT_EVENT::LSN_DT_KEYBOARD : {
				if ( !_sFile.Write( _ieInputOptions.u.kb ) ) { return false; }
				break;
			}
			case LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER : {
				if ( !_sFile.Write( _ieInputOptions.u.cont ) ) { return false; }
				break;
			}
		}
		return true;
	}

	/**
	 * Loads the recent-files list.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadRecentFiles( uint32_t /*_ui32Version*/, CStream &_sFile ) {
		m_vRecentFiles.clear();
		uint8_t ui8Total = 0;
		if ( !_sFile.ReadUi8( ui8Total ) ) { return false; }
		m_ui32RecentLimit = ui8Total;
		if ( !_sFile.ReadUi8( ui8Total ) ) { return false; }
		std::u16string u16Tmp;
		for ( uint8_t I = 0; I < ui8Total; ++I ) {
			if ( !_sFile.ReadStringU16( u16Tmp ) ) { return false; }
			AddPath( u16Tmp );
		}
		return true;
	}

	/**
	 * Saves the recent-files list.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveRecentFiles( CStream &_sFile ) {
		uint8_t ui8Total = uint8_t( std::min( m_ui32RecentLimit, uint32_t( 100 ) ) );
		if ( !_sFile.WriteUi8( ui8Total ) ) { return false; }

		ui8Total = uint8_t( std::min( m_vRecentFiles.size(), size_t( ui8Total ) ) );
		if ( !_sFile.WriteUi8( ui8Total ) ) { return false; }
		for ( uint8_t I = ui8Total; I--; ) {
			if ( !_sFile.WriteStringU16( m_vRecentFiles[I] ) ) { return false; }
		}
		return true;
	}

	/**
	 * Loads audio settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _aoAudioOptions The audio options into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadAudioSettings( uint32_t /*_ui32Version*/, CStream &_sFile, LSN_AUDIO_OPTIONS &_aoAudioOptions ) {
		_aoAudioOptions = LSN_AUDIO_OPTIONS();	// Set default values.
		if ( !_sFile.Read( _aoAudioOptions.afFormat.sfFormat ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.ui32OutputHz ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.bUseGlobal ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.bEnabled ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.fVolume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.fBgVol ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bLpfEnable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fLpf ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fZero ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bHpf0Enable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fHpf0 ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bHpf1Enable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fHpf1 ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bHpf2Enable ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fHpf2 ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fVolume ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fP1Volume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fP2Volume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fTVolume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fNVolume ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fDmcVolume ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bNoise ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bInvert ) ) { return false; }
		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.bRp2A02 ) ) { return false; }

		if ( !_sFile.Read( _aoAudioOptions.apCharacteristics.fmFilterMode ) ) { return false; }
		return true;
	}

	/**
	 * Saves audio settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _aoAudioOptions The audio options to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveAudioSettings( CStream &_sFile, LSN_AUDIO_OPTIONS &_aoAudioOptions ) {
		if ( !_sFile.Write( _aoAudioOptions.afFormat.sfFormat ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.ui32OutputHz ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.bUseGlobal ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.bEnabled ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.fVolume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.fBgVol ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bLpfEnable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fLpf ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fZero ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bHpf0Enable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fHpf0 ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bHpf1Enable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fHpf1 ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bHpf2Enable ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fHpf2 ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fVolume ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fP1Volume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fP2Volume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fTVolume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fNVolume ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fDmcVolume ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bNoise ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bInvert ) ) { return false; }
		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.bRp2A02 ) ) { return false; }

		if ( !_sFile.Write( _aoAudioOptions.apCharacteristics.fmFilterMode ) ) { return false; }
		return true;
	}

	/**
	 * Loads audio stream-to-file settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _stfoAudioOptions The audio options into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadAudioStreamSettings( uint32_t /*_ui32Version*/, CStream &_sFile, CWavFile::LSN_STREAM_TO_FILE_OPTIONS &_stfoAudioOptions ) {
		_stfoAudioOptions = CWavFile::LSN_STREAM_TO_FILE_OPTIONS();
		if ( !_sFile.ReadStringU16( _stfoAudioOptions.wsPath ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.bEnabled ) ) { return false; }
		_stfoAudioOptions.bEnabled = false;	// It should not be enabled via loading of the settings file, only via the dialog.
		if ( !_sFile.Read( _stfoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.fFormat ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.ui32Bits ) ) { return false; }

		if ( !_sFile.Read( _stfoAudioOptions.scStartCondition ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.ui64StartParm ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.dStartParm ) ) { return false; }

		if ( !_sFile.Read( _stfoAudioOptions.seEndCondition ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.ui64EndParm ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.dEndParm ) ) { return false; }

		if ( !_sFile.Read( _stfoAudioOptions.ui64MetaParm ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.i32MetaFormat ) ) { return false; }
		if ( !_sFile.Read( _stfoAudioOptions.bMetaEnabled ) ) { return false; }
		return true;
	}

	/**
	 * Saves audio stream-to-file settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _stfoAudioOptions The audio options to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveAudioStreamSettings( CStream &_sFile, CWavFile::LSN_STREAM_TO_FILE_OPTIONS &_stfoAudioOptions ) {
		if ( !_sFile.WriteStringU16( _stfoAudioOptions.wsPath ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.bEnabled ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.bDither ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.fFormat ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.ui32Bits ) ) { return false; }

		if ( !_sFile.Write( _stfoAudioOptions.scStartCondition ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.ui64StartParm ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.dStartParm ) ) { return false; }

		if ( !_sFile.Write( _stfoAudioOptions.seEndCondition ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.ui64EndParm ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.dEndParm ) ) { return false; }

		if ( !_sFile.Write( _stfoAudioOptions.ui64MetaParm ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.i32MetaFormat ) ) { return false; }
		if ( !_sFile.Write( _stfoAudioOptions.bMetaEnabled ) ) { return false; }
		return true;
	}

	/**
	 * Loads WAV Editor window settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _wewoOptions The WAV Editor window settings into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadWavEditorWindowSettings( uint32_t /*_ui32Version*/, CStream &_sFile, LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions ) {
		_wewoOptions = LSN_WAV_EDITOR_WINDOW_OPTIONS();
		try {
			_wewoOptions.vPerFileOptions.resize( 1 );
		}
		catch ( ... ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsPreFadeDur ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsFadeDur ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsOpeningSilence ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsTrailingSilence ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsMainsHumVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsWhiteNoiseVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsGuassianNoiseBandwidth ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsGuassianNoiseTemperature ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsGuassianNoiseResistance ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsActualHz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharLpfHz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf0Hz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf1Hz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf2Hz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharLpfFall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf0Fall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf1Fall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsCharHpf2Fall ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsArtist ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsAlbum ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsYear ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.vPerFileOptions[0].wsComment ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsAbsoluteVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsNormalizeVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsLoudnessVolume ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsOutputHz ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsOutputFolder ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bLoop ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui32ActualHz ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui32CharPreset ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bInvert ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bLpf ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bHpf0 ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bHpf1 ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bHpf2 ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8LpfType ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8Hpf0Type ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8Hpf1Type ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].ui8Hpf2Type ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.bMainsHum ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bWhiteNoise ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bDither ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.bAbsolute ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bNormalize ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.bLoudness ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.ui32MainsHum ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32WhiteNoise ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32OutFormat ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32OutBits ) ) { return false; }
		if ( !_sFile.Read( _wewoOptions.ui32Stereo ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.bNumbered ) ) { return false; }

		if ( !_sFile.ReadStringU16( _wewoOptions.wsLastWavFolder ) ) { return false; }
		if ( !_sFile.ReadStringU16( _wewoOptions.wsLastMetaFolder ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bLockVol ) ) { return false; }

		if ( !_sFile.ReadStringU16( _wewoOptions.wsLastProjectsFolder ) ) { return false; }

		if ( !_sFile.Read( _wewoOptions.vPerFileOptions[0].bSunsoft5b ) ) { return false; }
		return true;
	}

	/**
	 * Saves WAV Editor window settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _wewoOptions The WAV Editor window settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveWavEditorWindowSettings( CStream &_sFile, const LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions ) {
		LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE pfTmp = LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE();
		if ( _wewoOptions.vPerFileOptions.size() ) {
			pfTmp = _wewoOptions.vPerFileOptions[0];
		}
		if ( !_sFile.WriteStringU16( pfTmp.wsPreFadeDur ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsFadeDur ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsOpeningSilence ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsTrailingSilence ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsMainsHumVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsWhiteNoiseVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsGuassianNoiseBandwidth ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsGuassianNoiseTemperature ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsGuassianNoiseResistance ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsActualHz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharLpfHz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf0Hz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf1Hz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf2Hz ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharLpfFall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf0Fall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf1Fall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsCharHpf2Fall ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsArtist ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsAlbum ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsYear ) ) { return false; }
		if ( !_sFile.WriteStringU16( pfTmp.wsComment ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsAbsoluteVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsNormalizeVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsLoudnessVolume ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsOutputHz ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsOutputFolder ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bLoop ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui32ActualHz ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui32CharPreset ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bInvert ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bLpf ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bHpf0 ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bHpf1 ) ) { return false; }
		if ( !_sFile.Write( pfTmp.bHpf2 ) ) { return false; }

		if ( !_sFile.Write( pfTmp.ui8LpfType ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui8Hpf0Type ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui8Hpf1Type ) ) { return false; }
		if ( !_sFile.Write( pfTmp.ui8Hpf2Type ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.bMainsHum ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bWhiteNoise ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bDither ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.bAbsolute ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bNormalize ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.bLoudness ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.ui32MainsHum ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32WhiteNoise ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32OutFormat ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32OutBits ) ) { return false; }
		if ( !_sFile.Write( _wewoOptions.ui32Stereo ) ) { return false; }

		if ( !_sFile.Write( _wewoOptions.bNumbered ) ) { return false; }

		if ( !_sFile.WriteStringU16( _wewoOptions.wsLastWavFolder ) ) { return false; }
		if ( !_sFile.WriteStringU16( _wewoOptions.wsLastMetaFolder ) ) { return false; }

		if ( !_sFile.Write( pfTmp.bLockVol ) ) { return false; }

		if ( !_sFile.WriteStringU16( _wewoOptions.wsLastProjectsFolder ) ) { return false; }

		if ( !_sFile.Write( pfTmp.bSunsoft5b ) ) { return false; }
		return true;
	}

	/**
	 * Loads palette settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _poOptions The palette settings into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadPaletteSettings( uint32_t _ui32Version, CStream &_sFile, LSN_PALETTE_OPTIONS &_poOptions ) {
		_poOptions = LSN_PALETTE_OPTIONS();

		if ( !_sFile.ReadStringU16( _poOptions.wsPath ) ) { return false; }
		CNesPalette::LSN_GAMMA gStored[LSN_PM_CONSOLE_TOTAL];
		const size_t stStored = StoredRegionCount( _ui32Version );
		for ( size_t I = 0; I < stStored; ++I ) {
			if ( !_sFile.Read( gStored[I] ) ) { return false; }
			if ( gStored[I] >= CNesPalette::LSN_G_TOTAL ) {
				gStored[I] = CNesPalette::LSN_G_CRT2;
			}
		}
		for ( size_t I = 0; I < std::size( _poOptions.gCrtGamma ); ++I ) {
			_poOptions.gCrtGamma[I] = gStored[StoredRegionIndex( _ui32Version, I )];
		}
		if ( !_sFile.Read( _poOptions.gMonitorGamma ) ) { return false; }
		if ( _poOptions.gMonitorGamma >= CNesPalette::LSN_G_TOTAL ) {
			_poOptions.gMonitorGamma = CNesPalette::LSN_G_sRGB;
		}
		if ( !_sFile.Read( _poOptions.bUseGlobal ) ) { return false; }
		return true;
	}

	/**
	 * Loads a palette setting for each region.  Files from before NTSC-J was added hold 5 regions; NTSC-J gets a copy of NTSC-M.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _ppoOptions The LSN_PM_CONSOLE_TOTAL palette settings into which to load the settings data.
	 * \param _ppoDefaults Optional LSN_PM_CONSOLE_TOTAL palette settings copied into _ppoOptions (with bUseGlobal set) before loading.
	 *	They remain if loading fails.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadRegionPaletteSettings( uint32_t _ui32Version, CStream &_sFile, LSN_PALETTE_OPTIONS * _ppoOptions, const LSN_PALETTE_OPTIONS * _ppoDefaults ) {
		if ( _ppoDefaults ) {
			// If loading fails, the defaults remain.
			for ( size_t I = 0; I < LSN_PM_CONSOLE_TOTAL; ++I ) {
				_ppoOptions[I] = _ppoDefaults[I];
				_ppoOptions[I].bUseGlobal = true;
			}
		}
		LSN_PALETTE_OPTIONS poStored[LSN_PM_CONSOLE_TOTAL];
		const size_t stStored = StoredRegionCount( _ui32Version );
		for ( size_t I = 0; I < stStored; ++I ) {
			if ( !LoadPaletteSettings( _ui32Version, _sFile, poStored[I] ) ) { return false; }
		}
		for ( size_t I = 0; I < LSN_PM_CONSOLE_TOTAL; ++I ) {
			_ppoOptions[I] = poStored[StoredRegionIndex( _ui32Version, I )];
		}
		return true;
	}

	/**
	 * Saves palette settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _poOptions The palette settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SavePaletteSettings( CStream &_sFile, const LSN_PALETTE_OPTIONS &_poOptions ) {
		if ( !_sFile.WriteStringU16( _poOptions.wsPath ) ) { return false; }
		for ( size_t I = 0; I < std::size( _poOptions.gCrtGamma ); ++I ) {
			if ( !_sFile.Write( _poOptions.gCrtGamma[I] ) ) { return false; }
		}
		if ( !_sFile.Write( _poOptions.gMonitorGamma ) ) { return false; }
		if ( !_sFile.Write( _poOptions.bUseGlobal ) ) { return false; }
		return true;
	}

	/**
	 * Loads video-export settings.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _veoOptions The video-export settings into which to load the settings data.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadVideoExportSettings( uint32_t _ui32Version, CStream &_sFile, LSN_VIDEO_EXPORT_OPTIONS &_veoOptions ) {
		_veoOptions = LSN_VIDEO_EXPORT_OPTIONS();

		// Formats are stored by name so that adding, removing, or reordering formats does not scramble the settings.
		std::string sId;
		if ( !_sFile.ReadString( sId ) ) { return false; }
		LSN_VIDEO_FORMAT vfFormat = CVideoFormat::FromIdStr( sId.c_str() );
		if ( vfFormat != LSN_VF_TOTAL ) { _veoOptions.vfFormat = vfFormat; }
		if ( !_sFile.Read( _veoOptions.bInterpolateFrames ) ) { return false; }

		if ( _ui32Version >= 2 ) {
			if ( !_sFile.ReadStringU16( _veoOptions.wsOutputFolder ) ) { return false; }
			if ( !_sFile.ReadStringU16( _veoOptions.wsFfmpegPath ) ) { return false; }
			uint8_t ui8Cond;
			if ( !_sFile.Read( ui8Cond ) ) { return false; }
			_veoOptions.scStart = ui8Cond < LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_TOTAL ? LSN_VIDEO_EXPORT_OPTIONS::LSN_START_CONDITION( ui8Cond ) : LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_NONE;
			if ( !_sFile.Read( _veoOptions.ui64StartFrame ) ) { return false; }
			if ( !_sFile.Read( ui8Cond ) ) { return false; }
			_veoOptions.ecEnd = ui8Cond < LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_TOTAL ? LSN_VIDEO_EXPORT_OPTIONS::LSN_END_CONDITION( ui8Cond ) : LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_NONE;
			if ( !_sFile.Read( _veoOptions.ui64EndFrame ) ) { return false; }
			if ( !_sFile.Read( _veoOptions.dEndSeconds ) ) { return false; }
			if ( !(_veoOptions.dEndSeconds >= 0.0 && _veoOptions.dEndSeconds < 1.0e9) ) { _veoOptions.dEndSeconds = 0.0; }
			for ( std::vector<std::wstring> * pvHistory : { &_veoOptions.vStartHistory, &_veoOptions.vEndHistory } ) {
				uint32_t ui32Size;
				if ( !_sFile.ReadUi32( ui32Size ) ) { return false; }
				if ( ui32Size > 1024 ) { return false; }
				try {
					(*pvHistory).resize( ui32Size );
				}
				catch ( ... ) { return false; }
				for ( uint32_t J = 0; J < ui32Size; ++J ) {
					if ( !_sFile.ReadStringU16( (*pvHistory)[J] ) ) { return false; }
				}
			}
		}

		uint32_t ui32Total;
		if ( !_sFile.ReadUi32( ui32Total ) ) { return false; }
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS foTmp;
			std::string sPixelFormat, sAudioCodec;
			if ( !_sFile.ReadString( sId ) ) { return false; }
			if ( _ui32Version >= 2 ) {
				if ( !_sFile.Read( foTmp.ui32FrameRateNum ) ) { return false; }
				if ( !_sFile.Read( foTmp.ui32FrameRateDen ) ) { return false; }
			}
			else {
				// Version 1 stored the frame rate as a double (0.0 = match the console).
				double dFrameRate;
				if ( !_sFile.Read( dFrameRate ) ) { return false; }
				CVideoExporter::LSN_RATE rRate = CVideoExporter::ToRate( dFrameRate, 0 );
				foTmp.ui32FrameRateNum = rRate.ui32Num;
				foTmp.ui32FrameRateDen = rRate.ui32Den;
			}
			if ( !_sFile.Read( foTmp.i32Quality ) ) { return false; }
			if ( !_sFile.Read( foTmp.i32AltQuality ) ) { return false; }
			if ( !_sFile.Read( foTmp.ui32Preset ) ) { return false; }
			if ( !_sFile.Read( foTmp.ui32AudioBitrate ) ) { return false; }
			if ( !_sFile.ReadString( sPixelFormat ) ) { return false; }
			if ( !_sFile.ReadString( sAudioCodec ) ) { return false; }
			if ( !_sFile.Read( foTmp.bUseAltQuality ) ) { return false; }
			if ( !_sFile.Read( foTmp.bLossless ) ) { return false; }

			vfFormat = CVideoFormat::FromIdStr( sId.c_str() );
			if ( vfFormat == LSN_VF_TOTAL ) { continue; }		// A format that no longer exists.
			foTmp.vpfPixelFormat = CVideoFormat::PixelFormatFromFfmpegName( sPixelFormat.c_str() );
			foTmp.vacAudioCodec = CVideoFormat::AudioCodecFromFfmpegName( sAudioCodec.c_str() );
			foTmp.Validate( vfFormat );
			_veoOptions.foFormat[vfFormat] = foTmp;
		}
		return true;
	}

	/**
	 * Saves video-export settings.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _veoOptions The video-export settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveVideoExportSettings( CStream &_sFile, const LSN_VIDEO_EXPORT_OPTIONS &_veoOptions ) {
		if ( !_sFile.WriteString( reinterpret_cast<const char8_t *>(CVideoFormat::IdStr( _veoOptions.vfFormat )) ) ) { return false; }
		if ( !_sFile.Write( _veoOptions.bInterpolateFrames ) ) { return false; }
		if ( !_sFile.WriteStringU16( _veoOptions.wsOutputFolder ) ) { return false; }
		if ( !_sFile.WriteStringU16( _veoOptions.wsFfmpegPath ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _veoOptions.scStart ) ) ) { return false; }
		if ( !_sFile.Write( _veoOptions.ui64StartFrame ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _veoOptions.ecEnd ) ) ) { return false; }
		if ( !_sFile.Write( _veoOptions.ui64EndFrame ) ) { return false; }
		if ( !_sFile.Write( _veoOptions.dEndSeconds ) ) { return false; }
		for ( const std::vector<std::wstring> * pvHistory : { &_veoOptions.vStartHistory, &_veoOptions.vEndHistory } ) {
			if ( !_sFile.WriteUi32( uint32_t( (*pvHistory).size() ) ) ) { return false; }
			for ( size_t J = 0; J < (*pvHistory).size(); ++J ) {
				if ( !_sFile.WriteStringU16( (*pvHistory)[J] ) ) { return false; }
			}
		}

		if ( !_sFile.WriteUi32( LSN_VF_TOTAL ) ) { return false; }
		for ( uint16_t I = 0; I < LSN_VF_TOTAL; ++I ) {
			const LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS &foThis = _veoOptions.foFormat[I];
			const LSN_VIDEO_PIXEL_FORMAT_DATA * pvpfdPixel = CVideoFormat::PixelFormatData( foThis.vpfPixelFormat );
			const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdAudio = CVideoFormat::AudioCodecData( foThis.vacAudioCodec );
			if ( !_sFile.WriteString( reinterpret_cast<const char8_t *>(CVideoFormat::IdStr( LSN_VIDEO_FORMAT( I ) )) ) ) { return false; }
			if ( !_sFile.Write( foThis.ui32FrameRateNum ) ) { return false; }
			if ( !_sFile.Write( foThis.ui32FrameRateDen ) ) { return false; }
			if ( !_sFile.Write( foThis.i32Quality ) ) { return false; }
			if ( !_sFile.Write( foThis.i32AltQuality ) ) { return false; }
			if ( !_sFile.Write( foThis.ui32Preset ) ) { return false; }
			if ( !_sFile.Write( foThis.ui32AudioBitrate ) ) { return false; }
			if ( !_sFile.WriteString( reinterpret_cast<const char8_t *>(pvpfdPixel ? (*pvpfdPixel).pcFfmpegName : "") ) ) { return false; }
			if ( !_sFile.WriteString( reinterpret_cast<const char8_t *>(pvacdAudio ? (*pvacdAudio).pcEncoder : "") ) ) { return false; }
			if ( !_sFile.Write( foThis.bUseAltQuality ) ) { return false; }
			if ( !_sFile.Write( foThis.bLossless ) ) { return false; }
		}
		return true;
	}

	/**
	 * Reads a floating-point value and stores it only if it is finite.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _tValue The value to update.  It is left unchanged if the stored value is not finite.
	 * \return Returns true if the value was read.
	 * \tparam _tType The floating-point type to read.
	 */
	template <typename _tType>
	static bool ReadFiniteSetting( CStream &_sFile, _tType &_tValue ) {
		_tType tTmp;
		if ( !_sFile.Read( tTmp ) ) { return false; }
		if ( std::isfinite( tTmp ) ) { _tValue = tTmp; }
		return true;
	}

	/**
	 * Reads an integer value and stores it only if it is within a given inclusive range.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _tValue The value to update.  It is left unchanged if the stored value is out of range.
	 * \param _i64Min The minimum allowed value.
	 * \param _i64Max The maximum allowed value.
	 * \return Returns true if the value was read.
	 * \tparam _tStored The type of the value as stored in the file.
	 * \tparam _tType The type of the value to update.
	 */
	template <typename _tStored, typename _tType>
	static bool ReadRangedSetting( CStream &_sFile, _tType &_tValue, int64_t _i64Min, int64_t _i64Max ) {
		_tStored tTmp;
		if ( !_sFile.Read( tTmp ) ) { return false; }
		if ( int64_t( tTmp ) >= _i64Min && int64_t( tTmp ) <= _i64Max ) { _tValue = _tType( tTmp ); }
		return true;
	}

	/**
	 * Reads a Boolean value.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _bValue Holds the returned value.
	 * \return Returns true if the value was read.
	 */
	static bool ReadBoolSetting( CStream &_sFile, bool &_bValue ) {
		uint8_t ui8Tmp;
		if ( !_sFile.Read( ui8Tmp ) ) { return false; }
		_bValue = ui8Tmp != 0;
		return true;
	}

	/**
	 * Loads the settings of every filter for every region.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _vfoOptions The filter settings into which to load the settings data.  Values that are missing or invalid are left unchanged.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadVideoFilterSettings( uint32_t _ui32Version, CStream &_sFile, LSN_VIDEO_FILTER_OPTIONS &_vfoOptions ) {
		// The section starts with a tag and its own version.  Data without the tag was written before the current layout and is ignored
		//	(the defaults remain).
		uint32_t ui32Tag, ui32SectionVersion;
		if ( !_sFile.ReadUi32( ui32Tag ) ) { return false; }
		if ( ui32Tag != LSN_VIDEO_FILTER_SECTION_TAG ) { return true; }
		if ( !_sFile.ReadUi32( ui32SectionVersion ) ) { return false; }
		if ( ui32SectionVersion != LSN_VIDEO_FILTER_SECTION_VERSION ) { return true; }

		// Each region is stored by its LSN_PPU_METRICS value; regions added later are left at their defaults and regions that no longer
		//	exist are skipped.
		uint32_t ui32Total;
		if ( !_sFile.ReadUi32( ui32Total ) ) { return false; }
		if ( ui32Total > 64 ) { return false; }
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			LSN_FILTER_REGION_OPTIONS froTmp = LSN_FILTER_REGION_OPTIONS::Defaults( LSN_PPU_METRICS( I < LSN_PM_CONSOLE_TOTAL ? I : 0 ) );
			LSN_FILTER_REGION_OPTIONS & froDst = I < LSN_PM_CONSOLE_TOTAL ? _vfoOptions.froRegions[I] : froTmp;
			if ( !LoadFilterRegionSettings( _ui32Version, _sFile, froDst ) ) { return false; }
		}
		return true;
	}

	/**
	 * Saves the settings of every filter for every region.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _vfoOptions The filter settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveVideoFilterSettings( CStream &_sFile, const LSN_VIDEO_FILTER_OPTIONS &_vfoOptions ) {
		if ( !_sFile.WriteUi32( LSN_VIDEO_FILTER_SECTION_TAG ) ) { return false; }
		if ( !_sFile.WriteUi32( LSN_VIDEO_FILTER_SECTION_VERSION ) ) { return false; }
		if ( !_sFile.WriteUi32( uint32_t( LSN_PM_CONSOLE_TOTAL ) ) ) { return false; }
		for ( size_t I = 0; I < LSN_PM_CONSOLE_TOTAL; ++I ) {
			if ( !SaveFilterRegionSettings( _sFile, _vfoOptions.froRegions[I] ) ) { return false; }
		}
		return true;
	}

	/**
	 * Loads the settings of every filter for a single region.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _froOptions The filter settings into which to load the settings data.  Values that are invalid are left unchanged.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadFilterRegionSettings( uint32_t /*_ui32Version*/, CStream &_sFile, LSN_FILTER_REGION_OPTIONS &_froOptions ) {
		if ( !ReadBoolSetting( _sFile, _froOptions.bUseGlobal ) ) { return false; }

		if ( !LoadGpuFilterSettings( _sFile, _froOptions.gfoPalette ) ) { return false; }

		if ( !LoadBlarggFilterSettings( _sFile, _froOptions.boBlarggNtsc ) ) { return false; }
		if ( !LoadGpuFilterSettings( _sFile, _froOptions.gfoBlargg ) ) { return false; }

		if ( !LoadLmp88959FilterSettings( _sFile, _froOptions.lsLmpNtscCpu ) ) { return false; }
		if ( !LoadLmp88959FilterSettings( _sFile, _froOptions.lsLmpNtscGpu ) ) { return false; }
		if ( !LoadGpuFilterSettings( _sFile, _froOptions.gfoLmpNtsc ) ) { return false; }
		if ( !LoadLmp88959FilterSettings( _sFile, _froOptions.lsLmpPalCpu ) ) { return false; }
		if ( !LoadLmp88959FilterSettings( _sFile, _froOptions.lsLmpPalGpu ) ) { return false; }
		if ( !LoadGpuFilterSettings( _sFile, _froOptions.gfoLmpPal ) ) { return false; }

		if ( !LoadLSpiroFilterSettings( _sFile, _froOptions.lsoLSpiroCpu ) ) { return false; }
		if ( !LoadLSpiroFilterSettings( _sFile, _froOptions.lsoLSpiroGpu ) ) { return false; }
		if ( !LoadGpuFilterSettings( _sFile, _froOptions.gfoLSpiro ) ) { return false; }
		return true;
	}

	/**
	 * Saves the settings of every filter for a single region.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _froOptions The filter settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveFilterRegionSettings( CStream &_sFile, const LSN_FILTER_REGION_OPTIONS &_froOptions ) {
		if ( !_sFile.Write( uint8_t( _froOptions.bUseGlobal ? 1 : 0 ) ) ) { return false; }

		if ( !SaveGpuFilterSettings( _sFile, _froOptions.gfoPalette ) ) { return false; }

		if ( !SaveBlarggFilterSettings( _sFile, _froOptions.boBlarggNtsc ) ) { return false; }
		if ( !SaveGpuFilterSettings( _sFile, _froOptions.gfoBlargg ) ) { return false; }

		if ( !SaveLmp88959FilterSettings( _sFile, _froOptions.lsLmpNtscCpu ) ) { return false; }
		if ( !SaveLmp88959FilterSettings( _sFile, _froOptions.lsLmpNtscGpu ) ) { return false; }
		if ( !SaveGpuFilterSettings( _sFile, _froOptions.gfoLmpNtsc ) ) { return false; }
		if ( !SaveLmp88959FilterSettings( _sFile, _froOptions.lsLmpPalCpu ) ) { return false; }
		if ( !SaveLmp88959FilterSettings( _sFile, _froOptions.lsLmpPalGpu ) ) { return false; }
		if ( !SaveGpuFilterSettings( _sFile, _froOptions.gfoLmpPal ) ) { return false; }

		if ( !SaveLSpiroFilterSettings( _sFile, _froOptions.lsoLSpiroCpu ) ) { return false; }
		if ( !SaveLSpiroFilterSettings( _sFile, _froOptions.lsoLSpiroGpu ) ) { return false; }
		if ( !SaveGpuFilterSettings( _sFile, _froOptions.gfoLSpiro ) ) { return false; }
		return true;
	}

	/**
	 * Loads the settings shared by all GPU filters.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _gfoOptions The settings into which to load the settings data.  Values that are invalid are left unchanged.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadGpuFilterSettings( CStream &_sFile, LSN_GPU_FILTER_OPTIONS &_gfoOptions ) {
		if ( !ReadRangedSetting<uint32_t>( _sFile, _gfoOptions.cspColorSpace, 0, int64_t( CColorSpace::LSN_CSP_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _gfoOptions.fHueOffset ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _gfoOptions.gGamma, 0, int64_t( CNesPalette::LSN_G_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _gfoOptions.fCrt1White ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _gfoOptions.fCrt1Brightness ) ) { return false; }
		if ( !ReadBoolSetting( _sFile, _gfoOptions.bPhosphorDecay ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _gfoOptions.fPhosphorDecayLevel ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _gfoOptions.fPhosphorDecayPeriod ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _gfoOptions.fPhosphorRedMultiplier ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _gfoOptions.fPhosphorBlueMultiplier ) ) { return false; }
		LSN_VIDEO_LIMITS::ClampPhosphor( _gfoOptions.fPhosphorDecayLevel, _gfoOptions.fPhosphorDecayPeriod );
		_gfoOptions.fPhosphorRedMultiplier = std::clamp( _gfoOptions.fPhosphorRedMultiplier, LSN_VIDEO_LIMITS::fMinPhosphorMultiplier, LSN_VIDEO_LIMITS::fMaxPhosphorMultiplier );
		_gfoOptions.fPhosphorBlueMultiplier = std::clamp( _gfoOptions.fPhosphorBlueMultiplier, LSN_VIDEO_LIMITS::fMinPhosphorMultiplier, LSN_VIDEO_LIMITS::fMaxPhosphorMultiplier );
		_gfoOptions.fCrt1White = std::clamp( _gfoOptions.fCrt1White, LSN_VIDEO_LIMITS::fMinCrt1White, LSN_VIDEO_LIMITS::fMaxCrt1White );
		_gfoOptions.fCrt1Brightness = std::clamp( _gfoOptions.fCrt1Brightness, LSN_VIDEO_LIMITS::fMinCrt1Brightness, LSN_VIDEO_LIMITS::fMaxCrt1Brightness );
		if ( !ReadRangedSetting<uint32_t>( _sFile, _gfoOptions.ui32VertSharpness, 1, 32 ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _gfoOptions.ui32HorSharpness, 1, 32 ) ) { return false; }
		if ( !ReadBoolSetting( _sFile, _gfoOptions.bDynamicScaling ) ) { return false; }
		if ( !ReadBoolSetting( _sFile, _gfoOptions.bHighQualityResampler ) ) { return false; }
		for ( auto & ffThis : _gfoOptions.ffConvolution ) {
			// LSN_FF_TOTAL (none) is valid.
			if ( !ReadRangedSetting<uint32_t>( _sFile, ffThis, 0, int64_t( CResamplerBase::LSN_FF_TOTAL ) ) ) { return false; }
		}
		return true;
	}


	/**
	 * Saves the settings shared by all GPU filters.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _gfoOptions The settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveGpuFilterSettings( CStream &_sFile, const LSN_GPU_FILTER_OPTIONS &_gfoOptions ) {
		if ( !_sFile.Write( uint32_t( _gfoOptions.cspColorSpace ) ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.fHueOffset ) ) { return false; }
		if ( !_sFile.Write( uint32_t( _gfoOptions.gGamma ) ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.fCrt1White ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.fCrt1Brightness ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _gfoOptions.bPhosphorDecay ? 1 : 0 ) ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.fPhosphorDecayLevel ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.fPhosphorDecayPeriod ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.fPhosphorRedMultiplier ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.fPhosphorBlueMultiplier ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.ui32VertSharpness ) ) { return false; }
		if ( !_sFile.Write( _gfoOptions.ui32HorSharpness ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _gfoOptions.bDynamicScaling ? 1 : 0 ) ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _gfoOptions.bHighQualityResampler ? 1 : 0 ) ) ) { return false; }
		for ( auto ffThis : _gfoOptions.ffConvolution ) {
			if ( !_sFile.Write( uint32_t( ffThis ) ) ) { return false; }
		}
		return true;
	}


	/**
	 * Loads the settings of a Blargg filter.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _boOptions The settings into which to load the settings data.  Values that are invalid are left unchanged.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadBlarggFilterSettings( CStream &_sFile, LSN_BLARGG_OPTIONS &_boOptions ) {
		for ( double * pdThis : { &_boOptions.dHue, &_boOptions.dSaturation, &_boOptions.dContrast, &_boOptions.dBrightness, &_boOptions.dSharpness,
			&_boOptions.dGamma, &_boOptions.dResolution, &_boOptions.dArtifacts, &_boOptions.dFringing, &_boOptions.dBleed } ) {
			if ( !ReadFiniteSetting( _sFile, (*pdThis) ) ) { return false; }
		}
		if ( !ReadBoolSetting( _sFile, _boOptions.bMergeFields ) ) { return false; }
		return true;
	}

	/**
	 * Saves the settings of a Blargg filter.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _boOptions The settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveBlarggFilterSettings( CStream &_sFile, const LSN_BLARGG_OPTIONS &_boOptions ) {
		for ( double dThis : { _boOptions.dHue, _boOptions.dSaturation, _boOptions.dContrast, _boOptions.dBrightness, _boOptions.dSharpness,
			_boOptions.dGamma, _boOptions.dResolution, _boOptions.dArtifacts, _boOptions.dFringing, _boOptions.dBleed } ) {
			if ( !_sFile.Write( dThis ) ) { return false; }
		}
		if ( !_sFile.Write( uint8_t( _boOptions.bMergeFields ? 1 : 0 ) ) ) { return false; }
		return true;
	}

	/**
	 * Loads the settings of an LMP88959 filter.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _lsOptions The settings into which to load the settings data.  Values that are invalid are left unchanged.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadLmp88959FilterSettings( CStream &_sFile, LSN_LMP88959_SETTINGS &_lsOptions ) {
		for ( int32_t * pi32This : { &_lsOptions.i32Hue, &_lsOptions.i32Brightness, &_lsOptions.i32Contrast, &_lsOptions.i32Saturation,
			&_lsOptions.i32BlackPoint, &_lsOptions.i32WhitePoint, &_lsOptions.i32Noise } ) {
			if ( !ReadRangedSetting<int32_t>( _sFile, (*pi32This), -100000, 100000 ) ) { return false; }
		}
		if ( !ReadBoolSetting( _sFile, _lsOptions.bBlend ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _lsOptions.ui32BorderColor, 0, 0x3F ) ) { return false; }
		if ( !ReadBoolSetting( _sFile, _lsOptions.bVSync ) ) { return false; }
		if ( !ReadBoolSetting( _sFile, _lsOptions.bHSync ) ) { return false; }
		if ( !ReadBoolSetting( _sFile, _lsOptions.bChromaCorrection ) ) { return false; }
		if ( !ReadRangedSetting<int32_t>( _sFile, _lsOptions.i32YOffset, -64, 64 ) ) { return false; }
		return true;
	}

	/**
	 * Saves the settings of an LMP88959 filter.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _lsOptions The settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveLmp88959FilterSettings( CStream &_sFile, const LSN_LMP88959_SETTINGS &_lsOptions ) {
		for ( int32_t i32This : { _lsOptions.i32Hue, _lsOptions.i32Brightness, _lsOptions.i32Contrast, _lsOptions.i32Saturation,
			_lsOptions.i32BlackPoint, _lsOptions.i32WhitePoint, _lsOptions.i32Noise } ) {
			if ( !_sFile.Write( i32This ) ) { return false; }
		}
		if ( !_sFile.Write( uint8_t( _lsOptions.bBlend ? 1 : 0 ) ) ) { return false; }
		if ( !_sFile.Write( _lsOptions.ui32BorderColor ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _lsOptions.bVSync ? 1 : 0 ) ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _lsOptions.bHSync ? 1 : 0 ) ) ) { return false; }
		if ( !_sFile.Write( uint8_t( _lsOptions.bChromaCorrection ? 1 : 0 ) ) ) { return false; }
		if ( !_sFile.Write( _lsOptions.i32YOffset ) ) { return false; }
		return true;
	}

	/**
	 * Loads the settings of an L. Spiro filter.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _lsoOptions The settings into which to load the settings data.  Values that are invalid are left unchanged.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadLSpiroFilterSettings( CStream &_sFile, LSN_LSPIRO_OPTIONS &_lsoOptions ) {
		if ( !ReadRangedSetting<uint16_t>( _sFile, _lsoOptions.ui16WidthScale, 1, 64 ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _lsoOptions.ui32KernelSizeY, 1, 128 ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _lsoOptions.ui32KernelSizeChroma, 1, 128 ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _lsoOptions.ffFilterY, 0, int64_t( LSN_FF_FUNC_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _lsoOptions.fFilterYParms[0] ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _lsoOptions.fFilterYParms[1] ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _lsoOptions.ffFilterChroma, 0, int64_t( LSN_FF_FUNC_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _lsoOptions.fFilterChromaParms[0] ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _lsoOptions.fFilterChromaParms[1] ) ) { return false; }
		for ( float * pfThis : { &_lsoOptions.fHue, &_lsoOptions.fGamma, &_lsoOptions.fBrightness, &_lsoOptions.fSaturation, &_lsoOptions.fContrast, &_lsoOptions.fBlackLevel,
			&_lsoOptions.fWhiteLevel, &_lsoOptions.fBleed, &_lsoOptions.fPhosphorDecayPeriod, &_lsoOptions.fPhosphorDecayLevel } ) {
			if ( !ReadFiniteSetting( _sFile, (*pfThis) ) ) { return false; }
		}
		LSN_VIDEO_LIMITS::ClampPhosphor( _lsoOptions.fPhosphorDecayLevel, _lsoOptions.fPhosphorDecayPeriod );
		_lsoOptions.fBleed = std::clamp( _lsoOptions.fBleed, 0.0f, LSN_VIDEO_LIMITS::fMaxBleed );
		if ( !ReadRangedSetting<uint8_t>( _sFile, _lsoOptions.cfCombFilter, 0, int64_t( CLSpiroPalFilterBase::LSN_CF_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _lsoOptions.fCombWeight ) ) { return false; }
		_lsoOptions.fCombWeight = std::clamp( _lsoOptions.fCombWeight, 0.0f, 1.0f );
		if ( !ReadRangedSetting<uint16_t>( _sFile, _lsoOptions.ui16PixelToSignal, 1, 64 ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, _lsoOptions.ntNoiseType, 0, int64_t( LSN_NT_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadFiniteSetting( _sFile, _lsoOptions.fNoiseAmount ) ) { return false; }
		_lsoOptions.fNoiseAmount = std::max( _lsoOptions.fNoiseAmount, 0.0f );
		return true;
	}

	/**
	 * Saves the settings of an L. Spiro filter.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _lsoOptions The settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveLSpiroFilterSettings( CStream &_sFile, const LSN_LSPIRO_OPTIONS &_lsoOptions ) {
		if ( !_sFile.Write( _lsoOptions.ui16WidthScale ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.ui32KernelSizeY ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.ui32KernelSizeChroma ) ) { return false; }
		if ( !_sFile.Write( uint32_t( _lsoOptions.ffFilterY ) ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.fFilterYParms[0] ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.fFilterYParms[1] ) ) { return false; }
		if ( !_sFile.Write( uint32_t( _lsoOptions.ffFilterChroma ) ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.fFilterChromaParms[0] ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.fFilterChromaParms[1] ) ) { return false; }
		for ( float fThis : { _lsoOptions.fHue, _lsoOptions.fGamma, _lsoOptions.fBrightness, _lsoOptions.fSaturation, _lsoOptions.fContrast, _lsoOptions.fBlackLevel,
			_lsoOptions.fWhiteLevel, _lsoOptions.fBleed, _lsoOptions.fPhosphorDecayPeriod, _lsoOptions.fPhosphorDecayLevel } ) {
			if ( !_sFile.Write( fThis ) ) { return false; }
		}
		if ( !_sFile.Write( uint8_t( _lsoOptions.cfCombFilter ) ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.fCombWeight ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.ui16PixelToSignal ) ) { return false; }
		if ( !_sFile.Write( uint32_t( _lsoOptions.ntNoiseType ) ) ) { return false; }
		if ( !_sFile.Write( _lsoOptions.fNoiseAmount ) ) { return false; }
		return true;
	}
	/**
	 * Loads the settings of the host display.
	 *
	 * \param _ui32Version The file version.
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _hdoOptions The settings into which to load the settings data.  Values that are invalid are left unchanged.
	 * \return Returns true if the settings data was loaded.
	 */
	bool CBeesNes::LoadHostDisplaySettings( uint32_t /*_ui32Version*/, CStream &_sFile, LSN_HOST_DISPLAY_OPTIONS &_hdoOptions ) {
		uint32_t ui32Tag, ui32SectionVersion;
		if ( !_sFile.ReadUi32( ui32Tag ) ) { return false; }
		if ( ui32Tag != LSN_HOST_DISPLAY_SECTION_TAG ) { return true; }
		if ( !_sFile.ReadUi32( ui32SectionVersion ) ) { return false; }
		if ( ui32SectionVersion != LSN_HOST_DISPLAY_SECTION_VERSION ) { return true; }

		CColorSpace::LSN_DISPLAY_SETTINGS & dsDisplay = _hdoOptions.dsDisplay;
		if ( !ReadRangedSetting<uint32_t>( _sFile, dsDisplay.cspColorSpace, 0, int64_t( CColorSpace::LSN_CSP_TOTAL ) - 1 ) ) { return false; }
		CNesPalette::LSN_GAMMA gMonitor = dsDisplay.gGamma;
		if ( !ReadRangedSetting<uint32_t>( _sFile, gMonitor, 0, int64_t( CNesPalette::LSN_G_TOTAL ) - 1 ) ) { return false; }
		if ( CColorSpace::IsMonitorGamma( gMonitor ) ) { dsDisplay.gGamma = gMonitor; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, dsDisplay.waWhiteAdaptation, 0, int64_t( CColorSpace::LSN_WA_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, dsDisplay.gmGamutMapping, 0, int64_t( CColorSpace::LSN_GM_TOTAL ) - 1 ) ) { return false; }
		if ( !ReadRangedSetting<uint32_t>( _sFile, dsDisplay.orOutputRange, 0, int64_t( CColorSpace::LSN_OR_TOTAL ) - 1 ) ) { return false; }
		return true;
	}

	/**
	 * Saves the settings of the host display.
	 *
	 * \param _sFile The in-memory stream of the settings file.
	 * \param _hdoOptions The settings to write to the settings data.
	 * \return Returns true if the settings data was saved.
	 */
	bool CBeesNes::SaveHostDisplaySettings( CStream &_sFile, const LSN_HOST_DISPLAY_OPTIONS &_hdoOptions ) {
		if ( !_sFile.WriteUi32( LSN_HOST_DISPLAY_SECTION_TAG ) ) { return false; }
		if ( !_sFile.WriteUi32( LSN_HOST_DISPLAY_SECTION_VERSION ) ) { return false; }
		const CColorSpace::LSN_DISPLAY_SETTINGS & dsDisplay = _hdoOptions.dsDisplay;
		if ( !_sFile.Write( uint32_t( dsDisplay.cspColorSpace ) ) ) { return false; }
		if ( !_sFile.Write( uint32_t( dsDisplay.gGamma ) ) ) { return false; }
		if ( !_sFile.Write( uint32_t( dsDisplay.waWhiteAdaptation ) ) ) { return false; }
		if ( !_sFile.Write( uint32_t( dsDisplay.gmGamutMapping ) ) ) { return false; }
		if ( !_sFile.Write( uint32_t( dsDisplay.orOutputRange ) ) ) { return false; }
		return true;
	}


	/**
	 * Adds or move to the top a given file path.
	 * 
	 * \param _s16Path The file path to add or move to the top.
	 **/
	void CBeesNes::AddPath( const std::u16string &_s16Path ) {
		std::u16string s16Normalized = CUtilities::Replace( _s16Path, u'/', u'\\' );
		// 2 types of culling owing to the use of ZIP files.
#ifdef LSN_WINDOWS
		for ( auto I = m_vRecentFiles.size(); I--; ) {
			if ( CSTR_EQUAL == ::CompareStringEx( LOCALE_NAME_INVARIANT, NORM_IGNORECASE, reinterpret_cast<LPCWCH>(s16Normalized.c_str()), -1, reinterpret_cast<LPCWCH>(m_vRecentFiles[I].c_str()), -1, NULL, NULL, NULL ) ) {
				m_vRecentFiles.erase( m_vRecentFiles.begin() + I );
			}
		}
#else
		while ( true ) {
			auto I = std::find( m_vRecentFiles.begin(), m_vRecentFiles.end(), s16Normalized );
			if ( m_vRecentFiles.end() != I ) {
				m_vRecentFiles.erase( I );
			}
			else { break; }
		}
#endif	// #ifdef LSN_WINDOWS

		
		if ( CUtilities::LastChar( s16Normalized ) == u'}' ) {
			CUtilities::LSN_FILE_PATHS fpDecPathI;
			CUtilities::DeconstructFilePath( s16Normalized, fpDecPathI );
			fpDecPathI.u16sPath.pop_back();
			std::filesystem::path pPathI( fpDecPathI.u16sPath );
			for ( size_t I = 0; I < m_vRecentFiles.size(); ++I ) {
				if ( CUtilities::LastChar( m_vRecentFiles[I] ) == u'}' ) {
					CUtilities::LSN_FILE_PATHS fpDecPathJ;
					CUtilities::DeconstructFilePath( m_vRecentFiles[I], fpDecPathJ );
					fpDecPathJ.u16sPath.pop_back();
					std::filesystem::path pPathJ( fpDecPathJ.u16sPath );

					try {
						if ( std::filesystem::equivalent( pPathI, pPathJ ) ) {
							m_vRecentFiles.erase( m_vRecentFiles.begin() + I-- );
						}
					}
					catch ( ... ) {}

#ifdef LSN_WINDOWS
					if ( CSTR_EQUAL == ::CompareStringEx( LOCALE_NAME_INVARIANT, NORM_IGNORECASE, reinterpret_cast<LPCWCH>(fpDecPathI.u16sFile.c_str()), -1, reinterpret_cast<LPCWCH>(fpDecPathJ.u16sFile.c_str()), -1, NULL, NULL, NULL ) ) {
						m_vRecentFiles.erase( m_vRecentFiles.begin() + I );
					}
#else
					if ( fpDecPathI.u16sFile == fpDecPathJ.u16sFile ) {
						m_vRecentFiles.erase( m_vRecentFiles.begin() + I );
					}
					else {
						try {
							if ( std::filesystem::equivalent( std::filesystem::path( fpDecPathI.u16sFile ), std::filesystem::path( fpDecPathJ.u16sFile ) ) ) {
								m_vRecentFiles.erase( m_vRecentFiles.begin() + I-- );
							}
						}
						catch ( ... ) {}
					}
#endif	// #ifdef LSN_WINDOWS


				}
			}
		}
		else {
			std::filesystem::path pPathI( s16Normalized );
			for ( size_t I = 0; I < m_vRecentFiles.size(); ++I ) {
				if ( CUtilities::LastChar( m_vRecentFiles[I] ) != u'}' ) {
					std::filesystem::path pPathJ( m_vRecentFiles[I] );
					try {
						if ( std::filesystem::equivalent( pPathI, pPathJ ) ) {
							m_vRecentFiles.erase( m_vRecentFiles.begin() + I-- );
						}
					}
					catch ( ... ) {}
				}
			}
		}
		m_vRecentFiles.insert( m_vRecentFiles.begin(), s16Normalized );
		while ( m_vRecentFiles.size() > m_ui32RecentLimit ) { m_vRecentFiles.pop_back(); }
	}

}	// namespace lsn
