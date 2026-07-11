#include "LSNStdController.h"


namespace lsn {

	// == Functions.
	/**
	 * Handles a read of $4016/$4017.
	 * 
	 * \return Returns the desired read value.
	 **/
	uint8_t CStdController::Read() {
		uint8_t ui8Ret = ((m_ui8Status & 0x80) != 0);	// Upper 3 bits will be forced to open bus.
		m_ui8Status <<= 1;
		return ui8Ret;
	}

	/**
	 * Handles the write to $4016.
	 * 
	 * \param _ui8Val The value being written.
	 **/
	void CStdController::Write( uint8_t /*_ui8Val*/ ) {
		// Poll for all 8 buttons.
	}

	/**
	 * Called when the console is reset and at start-up.
	 **/
	void CStdController::Reset() {
		m_ui8Status = 0;
	}

	/**
	 * Called when controllers are detached from the system.
	 **/
	void CStdController::ControllerDetached() {
	}

	/**
	 * Applies controller configurations to the perpheral.
	 * 
	 * \param _ieConfig The configuration to apply.  [4] configurations, [8] buttons.
	 * \param _ieTurboConfig The turbo configuration to apply.  [4] configurations, [8] buttons.
	 * \param _vUsbControllers The attached USB controllers/devices.
	 * \param _icarRecord An in/out record of what controllers have been accessed and how.
	 * \return Returns true if all buttons were able to be applied.
	 **/
	bool CStdController::ApplyConfiguration( lsn::LSN_INPUT_EVENT _ieConfig[4][8], lsn::LSN_INPUT_EVENT _ieTurboConfig[4][8],
		const std::vector<CUsbControllerBase *> &/*_vUsbControllers*/,
		LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &/*_icarRecord*/ ) {
		return false;
	}

}	// namespace lsn
