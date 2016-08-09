/*
  ivtclient_c.h: central Virtual Terminal-Client management

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTCLIENT_H
#define IVTCLIENT_H

#include "impl/vtclient_c.h"
#include "ivttypes.h"

#include <list>

namespace IsoAgLib {

class iScheduler_c;


/**
  class to define an interface class for the storage of Preferred ISOVT. Users can derive from
  and implement the load and store functions to their needs.
 */
class iVtClientDataStorage_c {
  public:
    /** Application needs to load the stored preferred ISOVT iIsoName_c and boottime.
        @param arc_isoname saved isoname. Set to Undefined if not known
        @param arui8_boottime_s saved boottime in second. Set to 0 or 0xFF if not known
      */
    virtual void loadPreferredVt( IsoAgLib::iIsoName_c &arc_isoname, uint8_t &arui8_boottime_s ) = 0;

    /** Application needs to store the preferred ISOVT iIsoName_c and boottime.
        @param arc_isoname isoname to be saved
        @param arui8_boottime_s boottime to be saved, in second
      */
    virtual void storePreferredVt( const IsoAgLib::iIsoName_c &arc_isoname, uint8_t aui8_bootTime) = 0;

    /** Application can load for one function object more then one input objects.
        => The preferred assignment will be created, depending on the currently active joy sticks on the bus.
        There can be CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION different input objects managed for one function object.
      */
    virtual void loadPreferredAux2Assignment( iAux2Assignment_c& assignment ) = 0;
    
    /** Called, when an assignment for a function is changed and the "preferred assignment" flag is set.
        The application gets also the not matched preferred assignments (for not active joy sticks), which have been initially loaded via loadPreferredAux2Assignment().
        => the passed list can be used to overwrite all previous PA infos for this function object.
        
        The iAux2InputData.preferredAssignmentMatched marks the input which is currently assigned.
        It's not necessary to save preferredAssignmentMatched, because it is not used in loadPreferredAux2Assignment().
      */
    virtual void storePreferredAux2Assignment( uint16_t a_functionUid, const STL_NAMESPACE::list<iAux2InputData>& a_ref_preferred_assignment ) = 0;
                                               
    virtual int32_t getAux2DeltaWaitBeforeSendingPreferredAssigment() { return 2000; };
    virtual uint16_t getAux2ModelIdentificationCode() { return 0; }
};


/**
  central ISO11783 terminal management object

  For how to specify an ISO VT Object Pool please refer to \ref XMLspec .
  */
class iVtClient_c : private __IsoAgLib::VtClient_c {
public:
  /**
    register given object pool for uploading when possible.
  */
  iVtClientConnection_c* initAndRegisterObjectPool (iIdentItem_c& arc_identItem, iVtClientObjectPool_c& arc_pool, const char* apc_versionLabel, IsoAgLib::iVtClientDataStorage_c& apc_claimDataStorage, iVtClientObjectPool_c::RegisterPoolMode_en aen_mode)
  { return VtClient_c::initAndRegisterObjectPool (static_cast<__IsoAgLib::IdentItem_c&>(arc_identItem), arc_pool, apc_versionLabel, apc_claimDataStorage, aen_mode)->toInterfacePointer(); }

  bool deregisterObjectPool (iIdentItem_c& arc_wsMasterIdentItem)
  { return VtClient_c::deregisterObjectPool (arc_wsMasterIdentItem); }

  iVtClientConnection_c& getClientByID (uint8_t ui8_clientIndex)
  { return VtClient_c::getClientByID (ui8_clientIndex).toInterfaceReference(); }

  iVtClientConnection_c* getClientPtrByID (uint8_t ui8_clientIndex)
  { return VtClient_c::getClientPtrByID (ui8_clientIndex)->toInterfacePointer(); }

  bool isAnyVtAvailable() const { return VtClient_c::isAnyVtAvailable(); }

// the following define should be globally defined in the project settings...
#ifdef USE_IOP_GENERATOR_FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes)
  { VtClient_c::fakeVtProperties (aui16_dimension, aui16_skWidth, aui16_skHeight, aui16_colorDepth, aui16_fontSizes); }
#endif

private:
#if ( PRT_INSTANCE_CNT == 1 )
  friend iVtClient_c& getIvtClientInstance();
#endif
  friend iVtClient_c& getIvtClientInstance( unsigned instance );
  friend class iVtClientConnection_c;
};

#if ( PRT_INSTANCE_CNT == 1 )
  inline iVtClient_c& getIvtClientInstance()
  { return static_cast<iVtClient_c&>(__IsoAgLib::getVtClientInstance( 0 )); }
#endif
  inline iVtClient_c& getIvtClientInstance( unsigned instance )
  { return static_cast<iVtClient_c&>(__IsoAgLib::getVtClientInstance( instance )); }

}

#endif
