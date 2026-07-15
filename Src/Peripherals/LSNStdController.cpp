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
	void CStdController::Write( uint8_t _ui8Val ) {
		// Poll for all 8 buttons.
		if ( _ui8Val & 0b00000001 ) {
			m_ui8Status = 0;

			{
				// Get Left and Right controls together.
				uint8_t ui8Left = Poll( LSN_B_LEFT );
				uint8_t ui8Right = Poll( LSN_B_RIGHT );
				if ( m_bDisallowInvalidDpad ) {
					// Only one or the other may be pressed.
					if ( !m_ui8PrevStatus[LSN_B_LEFT] && ui8Left ) {
						// Left is just now being pressed, so it overrides Right.
						ui8Right = 0;
					}
					else if ( !m_ui8PrevStatus[LSN_B_RIGHT] && ui8Right ) {
						// Right is just now being pressed, so it overrides Left.
						ui8Left = 0;
					}
					else if ( ui8Left && ui8Right ) {
						// Both must have been held at the same time.  Just cancel out the input.
						ui8Left = ui8Right = 0;
					}
				}
				m_ui8Status |= ui8Left * LSN_IB_LEFT;
				m_ui8Status |= ui8Right * LSN_IB_RIGHT;
				m_ui8PrevStatus[LSN_B_LEFT] = ui8Left;
				m_ui8PrevStatus[LSN_B_RIGHT] = ui8Right;
			}
			{
				// Get Up and Down controls together.
				uint8_t ui8Up = Poll( LSN_B_UP );
				uint8_t ui8Down = Poll( LSN_B_DOWN );
				if ( m_bDisallowInvalidDpad ) {
					// Only one or the other may be pressed.
					if ( !m_ui8PrevStatus[LSN_B_UP] && ui8Up ) {
						// Up is just now being pressed, so it overrides Down.
						ui8Down = 0;
					}
					else if ( !m_ui8PrevStatus[LSN_B_DOWN] && ui8Down ) {
						// Down is just now being pressed, so it overrides Up.
						ui8Up = 0;
					}
					else if ( ui8Up && ui8Down ) {
						// Both must have been held at the same time.  Just cancel out the input.
						ui8Up = ui8Down = 0;
					}
				}
				m_ui8Status |= ui8Up * LSN_IB_UP;
				m_ui8Status |= ui8Down * LSN_IB_DOWN;
				m_ui8PrevStatus[LSN_B_UP] = ui8Up;
				m_ui8PrevStatus[LSN_B_DOWN] = ui8Down;
			}

			uint8_t ui8Tmp;
#define LSN_POLLME( BUTTON )		m_ui8Status |= (ui8Tmp = Poll( LSN_B_ ## BUTTON )) * LSN_IB_ ## BUTTON; m_ui8PrevStatus[LSN_B_ ## BUTTON] = ui8Tmp
			LSN_POLLME( A );
			LSN_POLLME( B );
			LSN_POLLME( START );
			LSN_POLLME( SELECT );
#undef LSN_POLLME
		}
	}

	/**
	 * Called when the console is reset and at start-up.
	 **/
	void CStdController::Reset() {
		m_ui8Status = 0;
		std::memset( m_ui8PrevStatus, 0, sizeof( m_ui8PrevStatus ) );

		for ( size_t B = 0; B < LSN_B_TOTAL; ++B ) {
			m_vNormalPollFuncs[B].clear();
			m_vTurboPollFuncs[B].clear();
			m_vMappings[B].clear();
			m_vTurboMappings[B].clear();
		}
	}

	/**
	 * Called when controllers are detached from the system.
	 **/
	void CStdController::ControllerDetached() {
		// For each button.
		for ( size_t B = 0; B < LSN_B_TOTAL; ++B ) {
			// For each assignment on that button.
			// Normal:
			for ( auto A = m_vMappings[B].size(); A--; ) {
				if ( m_vMappings[B][A].pucbController ) {
					m_vMappings[B].erase( m_vMappings[B].begin() + A );
					m_vNormalPollFuncs[B].erase( m_vNormalPollFuncs[B].begin() + A );
				}
			}
			// Turbo:
			for ( auto A = m_vTurboMappings[B].size(); A--; ) {
				if ( m_vTurboMappings[B][A].pucbController ) {
					m_vTurboMappings[B].erase( m_vTurboMappings[B].begin() + A );
					m_vTurboPollFuncs[B].erase( m_vTurboPollFuncs[B].begin() + A );
				}
			}
		}
	}

	/**
	 * Applies controller configurations to the perpheral.
	 * 
	 * \param _ieConfig The configuration to apply.  [LSN_IS_INPUT_SETS] configurations, [LSN_B_TOTAL] buttons.
	 * \param _ieTurboConfig The turbo configuration to apply.  [LSN_IS_INPUT_SETS] configurations, [LSN_B_TOTAL] buttons.
	 * \param _vUsbControllers The attached USB controllers/devices.
	 * \param _icarRecord An in/out record of what controllers have been accessed and how.
	 * \param _bStrictApply If true, USB controllers should only connect to the specific devices that were used to configur inputs.
	 * \return Returns true if all buttons were able to be applied.
	 **/
	bool CStdController::ApplyConfiguration( lsn::LSN_INPUT_EVENT _ieConfig[LSN_IS_INPUT_SETS][LSN_B_TOTAL], lsn::LSN_INPUT_EVENT _ieTurboConfig[LSN_IS_INPUT_SETS][LSN_B_TOTAL],
		const std::vector<CUsbControllerBase *> &_vUsbControllers,
		LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord,
		bool _bStrictApply ) {
		try {
			// For each button.
			for ( size_t B = 0; B < LSN_B_TOTAL; ++B ) {
				if ( _bStrictApply ) {	// Pass 1.
					m_vNormalPollFuncs[B].clear();
					m_vTurboPollFuncs[B].clear();
					m_vMappings[B].clear();
					m_vTurboMappings[B].clear();
				}
				// For each alternative key.
				for ( size_t A = 0; A < LSN_IS_INPUT_SETS; ++A ) {
					// Normal buttons.
					switch ( _ieConfig[A][B].dtType ) {
						case LSN_INPUT_EVENT::LSN_DT_KEYBOARD : {
							if ( _bStrictApply ) {
								if ( _ieConfig[A][B].u.kb.kKey.bKeyCode ) {
									m_vNormalPollFuncs[B].push_back( KeyBoardPoll );
									LSN_RUNTIME_POLL rpPoll = { .ieEvent = _ieConfig[A][B] };
									m_vMappings[B].push_back( rpPoll );
								}
							}
							break;
						}
						case LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER : {
							auto * pucbController = GetPreferredController( _ieConfig[A][B], _vUsbControllers, _icarRecord, _bStrictApply );
							if ( pucbController ) {
								LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_USED_MAPPING umAssignment = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
									.pucbController = pucbController, .ieEvent = _ieConfig[A][B], .sButtonIdx = B };

								if ( !CheckForExistingMapping( umAssignment, _icarRecord ) ) {
									switch ( _ieConfig[A][B].u.cont.ieEvent.icType ) {
										case CUsbControllerBase::LSN_IC_AXIS : {
											if ( _ieConfig[A][B].u.cont.ieEvent.u.lAxis < 0 ) {
												m_vNormalPollFuncs[B].push_back( UsbController_AxisNegativePoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieConfig[A][B] };
												m_vMappings[B].push_back( rpPoll );
											}
											else {
												m_vNormalPollFuncs[B].push_back( UsbController_AxisPositivePoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieConfig[A][B] };
												m_vMappings[B].push_back( rpPoll );
											}

											LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_PERIPHERAL_USB_PAIR pupPairing = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
												.pucbController = pucbController };
											_icarRecord.sPairings.insert( pupPairing );

											_icarRecord.vAssignments.push_back( umAssignment );
											break;
										}
										case CUsbControllerBase::LSN_IC_POV : {
											if ( PovIsUp( _ieConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vNormalPollFuncs[B].push_back( UsbController_PovUpPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieConfig[A][B] };
												m_vMappings[B].push_back( rpPoll );
											}
											else if ( PovIsLeft( _ieConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vNormalPollFuncs[B].push_back( UsbController_PovLeftPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieConfig[A][B] };
												m_vMappings[B].push_back( rpPoll );
											}
											else if ( PovIsRight( _ieConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vNormalPollFuncs[B].push_back( UsbController_PovRightPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieConfig[A][B] };
												m_vMappings[B].push_back( rpPoll );
											}
											else if ( PovIsDown( _ieConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vNormalPollFuncs[B].push_back( UsbController_PovDownPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieConfig[A][B] };
												m_vMappings[B].push_back( rpPoll );
											}

											LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_PERIPHERAL_USB_PAIR pupPairing = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
												.pucbController = pucbController };
											_icarRecord.sPairings.insert( pupPairing );

											_icarRecord.vAssignments.push_back( umAssignment );
											break;
										}
										case CUsbControllerBase::LSN_IC_BUTTON : {
											m_vNormalPollFuncs[B].push_back( UsbController_ButtonPoll );
											LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieConfig[A][B] };
											m_vMappings[B].push_back( rpPoll );

											LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_PERIPHERAL_USB_PAIR pupPairing = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
												.pucbController = pucbController };
											_icarRecord.sPairings.insert( pupPairing );

											_icarRecord.vAssignments.push_back( umAssignment );
											break;
										}
									}
								}
							}
							break;
						}
					}

					// Turbo buttons.
					switch ( _ieTurboConfig[A][B].dtType ) {
						case LSN_INPUT_EVENT::LSN_DT_KEYBOARD : {
							if ( _bStrictApply ) {
								if ( _ieTurboConfig[A][B].u.kb.kKey.bKeyCode ) {
									m_vTurboPollFuncs[B].push_back( KeyBoardPoll );
									LSN_RUNTIME_POLL rpPoll = { .ieEvent = _ieTurboConfig[A][B] };
									m_vTurboMappings[B].push_back( rpPoll );
								}
							}
							break;
						}
						case LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER : {
							auto * pucbController = GetPreferredController( _ieTurboConfig[A][B], _vUsbControllers, _icarRecord, _bStrictApply );
							if ( pucbController ) {
								LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_USED_MAPPING umAssignment = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
									.pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B], .sButtonIdx = B };

								if ( !CheckForExistingMapping( umAssignment, _icarRecord ) ) {
									switch ( _ieTurboConfig[A][B].u.cont.ieEvent.icType ) {
										case CUsbControllerBase::LSN_IC_AXIS : {
											if ( _ieTurboConfig[A][B].u.cont.ieEvent.u.lAxis < 0 ) {
												m_vTurboPollFuncs[B].push_back( UsbController_AxisNegativePoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B] };
												m_vTurboMappings[B].push_back( rpPoll );
											}
											else {
												m_vTurboPollFuncs[B].push_back( UsbController_AxisPositivePoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B] };
												m_vTurboMappings[B].push_back( rpPoll );
											}

											LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_PERIPHERAL_USB_PAIR pupPairing = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
												.pucbController = pucbController };
											_icarRecord.sPairings.insert( pupPairing );

											_icarRecord.vAssignments.push_back( umAssignment );
											break;
										}
										case CUsbControllerBase::LSN_IC_POV : {
											if ( PovIsUp( _ieTurboConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vTurboPollFuncs[B].push_back( UsbController_PovUpPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B] };
												m_vTurboMappings[B].push_back( rpPoll );
											}
											else if ( PovIsLeft( _ieTurboConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vTurboPollFuncs[B].push_back( UsbController_PovLeftPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B] };
												m_vTurboMappings[B].push_back( rpPoll );
											}
											else if ( PovIsRight( _ieTurboConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vTurboPollFuncs[B].push_back( UsbController_PovRightPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B] };
												m_vTurboMappings[B].push_back( rpPoll );
											}
											else if ( PovIsDown( _ieTurboConfig[A][B].u.cont.ieEvent.u.dwPov ) ) {
												m_vTurboPollFuncs[B].push_back( UsbController_PovDownPoll );
												LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B] };
												m_vTurboMappings[B].push_back( rpPoll );
											}

											LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_PERIPHERAL_USB_PAIR pupPairing = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
												.pucbController = pucbController };
											_icarRecord.sPairings.insert( pupPairing );

											_icarRecord.vAssignments.push_back( umAssignment );
											break;
										}
										case CUsbControllerBase::LSN_IC_BUTTON : {
											m_vTurboPollFuncs[B].push_back( UsbController_ButtonPoll );
											LSN_RUNTIME_POLL rpPoll = { .pucbController = pucbController, .ieEvent = _ieTurboConfig[A][B] };
											m_vTurboMappings[B].push_back( rpPoll );

											LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_PERIPHERAL_USB_PAIR pupPairing = { .ppbPeripheral = static_cast<IPeripheralBase *>(this),
												.pucbController = pucbController };
											_icarRecord.sPairings.insert( pupPairing );

											_icarRecord.vAssignments.push_back( umAssignment );
											break;
										}
									}
								}
							}
							break;
						}
					}
				}
			}
			return true;
		}
		catch ( ... ) {}
		for ( size_t I = LSN_B_TOTAL; I--; ) {
			m_vNormalPollFuncs[I].clear();
			m_vTurboPollFuncs[I].clear();
			m_vMappings[I].clear();
			m_vTurboMappings[I].clear();
		}
		return false;
	}

	/**
	 * Finds a preferred USB controller to use for polling for a given button.
	 * 
	 * \param _ieConfig The input event describing the controller to seek.
	 * \param _vUsbControllers The array of USB controllers attached to the PC.
	 * \param _icarRecord A record of past attachments, used to avoid double-pairing on the same device when _bStrict is false. Ignored when _bStrict is true.
	 * \param _bStrict AIf true, only the specific device specified by _ieConfig.u.cont.guId is selected. Otherwise any device of the same product can be select if it has not been
	 *	already.
	 * \return Returns the preferred controller for the given button or nullptr.
	 **/
	CUsbControllerBase * CStdController::GetPreferredController( lsn::LSN_INPUT_EVENT _ieConfig,
		const std::vector<CUsbControllerBase *> &_vUsbControllers,
		LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord, bool _bStrict ) {
		for ( size_t I = 0; I < _vUsbControllers.size(); ++I ) {
#ifdef LSN_WINDOWS
			if ( std::memcmp( &_ieConfig.u.cont.guId, _vUsbControllers[I]->UniqueId(), sizeof( _ieConfig.u.cont.guId ) ) == 0 ) {
				return _vUsbControllers[I];
			}
			if ( _bStrict ) { continue; }

			if ( std::memcmp( &_ieConfig.u.cont.guProductId, _vUsbControllers[I]->ProductId(), sizeof( _ieConfig.u.cont.guId ) ) == 0 ) {
				// Same product.  Is it in use by some other peripheral?
				bool bUsedByOther = false;
				for ( const auto & C : _icarRecord.sPairings ) {
					if ( C.pucbController == _vUsbControllers[I] && C.ppbPeripheral != static_cast<IPeripheralBase *>(this) ) {
						bUsedByOther = true;
						break;
					}
				}
				if ( !bUsedByOther ) {
					// Not used by any other peripheral.  Ours for the taking.
					return _vUsbControllers[I];
				}
			}
#endif	// #ifdef LSN_WINDOWS
		}
		return nullptr;
	}

	/**
	 * Determines if the supplied button mapping has already been made.
	 * 
	 * \param _umAssignment The button mapping to check for existing in _icarRecord.
	 * \param _icarRecord The record of mappings to check for the existence of _umAssignment.
	 * \return Returns true if the given button map exists in the record.
	 **/
	bool CStdController::CheckForExistingMapping( const LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_USED_MAPPING &_umAssignment,
		const LSN_INPUT_CONFIGURATION_APPLICATION_RECORD &_icarRecord ) {
		for ( auto I = _icarRecord.vAssignments.size(); I--; ) {
			if ( std::memcmp( &_umAssignment, &_icarRecord.vAssignments[I], sizeof( LSN_INPUT_CONFIGURATION_APPLICATION_RECORD::LSN_USED_MAPPING ) ) == 0 ) { return true; }
		}
		return false;
	}

	/**
	 * Polls a button, returning 1 if it is pressed, 0 otherwise.
	 * 
	 * \param _bButton The button to poll.
	 * \return Returns 1 if the given button is pressed, 0 otherwise.
	 **/
	uint8_t CStdController::Poll( LSN_BUTTONS _bButton ) {
#ifdef LSN_WINDOWS
		// Test turbo first.
		for ( size_t I = 0; I < m_vTurboPollFuncs[_bButton].size(); ++I ) {
			if ( (*m_vTurboPollFuncs[_bButton][I])( m_vTurboMappings[_bButton][I] ) ) {
				if ( !m_ui8PrevPhysicalStatus[_bButton] ) {
					m_vTurboMappings[_bButton][I].ui64TurboBits = m_vTurboMappings[_bButton][I].ui64OriginalTurboBits;
				}
				m_ui8PrevPhysicalStatus[_bButton] = true;
				uint8_t ui8Ret = m_vTurboMappings[_bButton][I].ui64TurboBits & 1;
				m_vTurboMappings[_bButton][I].ui64TurboBits = RotateRight64( m_vTurboMappings[_bButton][I].ui64TurboBits, 1 );
				return ui8Ret;
			}
		}
		// Normal buttons.
		for ( size_t I = 0; I < m_vNormalPollFuncs[_bButton].size(); ++I ) {
			if ( (*m_vNormalPollFuncs[_bButton][I])( m_vMappings[_bButton][I] ) ) {
				m_ui8PrevPhysicalStatus[_bButton] = true;
				return 1;
			}
		}
		m_ui8PrevPhysicalStatus[_bButton] = false;
		return 0;
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Polls the keyboard for a given button.
	 * 
	 * \param _rpPollData Contains which key to poll.
	 * \return Returns true if the given key is pressed.
	 **/
	bool CStdController::KeyBoardPoll( const LSN_RUNTIME_POLL &_rpPollData ) {
#ifdef LSN_WINDOWS
		SHORT sPoll = ::GetAsyncKeyState( _rpPollData.ieEvent.u.kb.kKey.bKeyCode );
		return (sPoll & 0x8000) != 0;
#else
		return false;
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Polls a USB controller for a given button being pressed.
	 * 
	 * \param _rpPollData Contains the data for which button to poll.
	 * \return Returns true if the assigned button is pressed on the given controller.
	 **/
	bool CStdController::UsbController_ButtonPoll( const LSN_RUNTIME_POLL &_rpPollData ) {
		return _rpPollData.pucbController->PollButton( uint8_t( _rpPollData.ieEvent.u.cont.ieEvent.stIdx ) ) != 0;
	}

	/**
	 * Polls a USB controller for the POV being Up.
	 * 
	 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
	 * \return Returns true if the controller's POV is in the Up direction.
	 **/
	bool CStdController::UsbController_PovUpPoll( const LSN_RUNTIME_POLL &_rpPollData ) {
		return PovIsUp( _rpPollData.pucbController->PollPov( uint8_t( _rpPollData.ieEvent.u.cont.ieEvent.stIdx ) ) );
	}

	/**
	 * Polls a USB controller for the POV being Left.
	 * 
	 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
	 * \return Returns true if the controller's POV is in the Left direction.
	 **/
	bool CStdController::UsbController_PovLeftPoll( const LSN_RUNTIME_POLL &_rpPollData ) {
		return PovIsLeft( _rpPollData.pucbController->PollPov( uint8_t( _rpPollData.ieEvent.u.cont.ieEvent.stIdx ) ) );
	}

	/**
	 * Polls a USB controller for the POV being Right.
	 * 
	 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
	 * \return Returns true if the controller's POV is in the Right direction.
	 **/
	bool CStdController::UsbController_PovRightPoll( const LSN_RUNTIME_POLL &_rpPollData ) {
		return PovIsRight( _rpPollData.pucbController->PollPov( uint8_t( _rpPollData.ieEvent.u.cont.ieEvent.stIdx ) ) );
	}

	/**
	 * Polls a USB controller for the POV being Down.
	 * 
	 * \param _rpPollData Contains the data for the controller whose POV is to be tested.
	 * \return Returns true if the controller's POV is in the Down direction.
	 **/
	bool CStdController::UsbController_PovDownPoll( const LSN_RUNTIME_POLL &_rpPollData ) {
		return PovIsDown( _rpPollData.pucbController->PollPov( uint8_t( _rpPollData.ieEvent.u.cont.ieEvent.stIdx ) ) );
	}

	/**
	 * Polls a USB controller for having a positive axis.
	 * 
	 * \param _rpPollData Contains the data for the controller whose axes are to be tested and the deadzone for said axis.
	 * \return Returns true if the desired axis is above the threshold in the positive direction.
	 **/
	bool CStdController::UsbController_AxisPositivePoll( const LSN_RUNTIME_POLL &_rpPollData ) {
		long lVal = _rpPollData.pucbController->PollAxis( uint8_t( _rpPollData.ieEvent.u.cont.ieEvent.stIdx ) );
		return lVal / 1000.0f > _rpPollData.ieEvent.u.cont.fDeadzone;
	}

	/**
	 * Polls a USB controller for having a negative axis.
	 * 
	 * \param _rpPollData Contains the data for the controller whose axes are to be tested and the deadzone for said axis.
	 * \return Returns true if the desired axis is above the threshold in the negative direction.
	 **/
	bool CStdController::UsbController_AxisNegativePoll( const LSN_RUNTIME_POLL &_rpPollData ) {
		return _rpPollData.pucbController->PollAxis( uint8_t( _rpPollData.ieEvent.u.cont.ieEvent.stIdx ) ) / 1000.0f < -_rpPollData.ieEvent.u.cont.fDeadzone;
	}

}	// namespace lsn
