#ifdef LSN_USE_WINDOWS

#include "LSNInputPage.h"

#include <dbt.h>


namespace lsn {

	/**
	 * The WM_DEVICECHANGE handler.
	 * 
	 * \param _wDbtEvent The event that has occurred.  One of the DBT_* values from the Dbt.h header file.
	 * \param _lParam A pointer to a structure that contains event-specific data. Its format depends on the value of the wParam parameter. For more information, refer to the documentation for each event.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CInputPage::DeviceChange( WORD _wDbtEvent, LPARAM _lParam ) {
		switch ( _wDbtEvent ) {
			case DBT_DEVNODES_CHANGED : {
				for ( size_t I = 0; I < m_vChildren.size(); ++I ) {
					m_vChildren[I]->DeviceChange( _wDbtEvent, _lParam );
				}
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
