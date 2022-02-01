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


    int32_t iVtClientDataStorage_c::getAux2DeltaWaitBeforeSendingPreferredAssigment() { return 2000; }


    bool iVtClient_c::deregisterObjectPool(iIdentItem_c &arc_wsMasterIdentItem) { return VtClient_c::deregisterObjectPool (arc_wsMasterIdentItem); }

    iVtClientConnection_c *
    iVtClient_c::initAndRegisterObjectPool(iIdentItem_c &arc_identItem, iVtClientObjectPool_c &arc_pool,
                                           const char *apc_versionLabel, iVtClientDataStorage_c &apc_claimDataStorage,
                                           iVtClientObjectPool_c::RegisterPoolMode_en aen_mode) { return VtClient_c::initAndRegisterObjectPool (static_cast<__IsoAgLib::IdentItem_c&>(arc_identItem), arc_pool, apc_versionLabel, apc_claimDataStorage, aen_mode)->toInterfacePointer(); }



    iVtClientConnection_c &iVtClient_c::getClientByID(uint8_t ui8_clientIndex) { return VtClient_c::getClientByID (ui8_clientIndex).toInterfaceReference(); }


    iVtClientConnection_c *iVtClient_c::getClientPtrByID(uint8_t ui8_clientIndex) {
        __IsoAgLib::VtClientConnection_c* result = VtClient_c::getClientPtrByID( ui8_clientIndex );
        return result ? result->toInterfacePointer() : NULL;
    }


}



