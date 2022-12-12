#include "LSNDisplayClient.h"
#include "LSNDisplayHost.h"


namespace lsn {

	CDisplayClient::~CDisplayClient() {
		if ( m_pdhHost ) {
			m_pdhHost->DetatchFromDisplayClient();
			DetatchFromDisplayHost();
		}
	}

}	// namespace lsn
