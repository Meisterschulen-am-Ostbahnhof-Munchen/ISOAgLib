/*
 * ivtclient_c.cpp
 *
 *  Created on: 01.02.2022
 *      Author: franz
 */

#include "ivtclient_c.h"

namespace IsoAgLib {


iVtClient_c &getIvtClientInstance() { return static_cast<iVtClient_c&>(__IsoAgLib::getVtClientInstance( 0 )); }

iVtClient_c &getIvtClientInstance(unsigned int instance) { return static_cast<iVtClient_c&>(__IsoAgLib::getVtClientInstance( instance )); }


}



