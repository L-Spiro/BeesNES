#include "LSNDisplayHost.h"
#include "LSNDisplayClient.h"


namespace lsn {

	CDisplayHost::~CDisplayHost() {
		if ( m_pdcClient ) {
			m_pdcClient->DetatchFromDisplayHost();
			DetatchFromDisplayClient();
		}
	}

}	// namespace lsn
