/*
  aux2functions_c.h:

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef AUX2FUNCTIONS_H
#define AUX2FUNCTIONS_H

#include <list>
#include <map>

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/iisoterminalobjectpool_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/util/impl/singleton.h>

#include "../ivtobjectauxiliaryfunction2_c.h"

namespace __IsoAgLib {

// forward declaration
class VtClientServerCommunication_c;

class Aux2Functions_c
{
  // @todo: check correct singleton instantiation!
#if 1 < PRT_INSTANCE_CNT
  MACRO_MULTITON_CONTRIBUTION();
#else
  MACRO_SINGLETON_CONTRIBUTION();
#endif

public:

  enum Aux2FunctionsState_en
  {
    State_WaitForPoolUploadSuccessfully,
    State_WaitForFirstInputMaintenanceMessage,
    State_CollectInputMaintenanceMessage,
    State_Ready
  };

  Aux2Functions_c(VtClientServerCommunication_c* a_vtClientServerCommunication);

  virtual ~Aux2Functions_c(void);

#ifdef USE_VTOBJECT_auxiliaryfunction2
  STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*>& getObjectList() { return mlist_aux2Function; }
#endif

  void notifyOnAux2InputStatus( const CanPkgExt_c& arc_data, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool);

  /** update maintenance time stamp in mmap_receivedInputMaintenanceData and decide when to send the preferred assignment message
   *
   * sending preferred assingment:
   * 1. wait for own object pool upload successfully
   * 2. wait for first input maintenance message with ready state
   *    - first receive: send empty preferred assignments if we do not have any preferred assignments
   *    - following receives until timeout: check if preferred assignment match with the input devices
   *    - after timeout: send preferred assignments for detected inputs (maybe empty too)
   */
  void notifyOnAux2InputMaintenance( const CanPkgExt_c& arc_data);

  bool storeAux2Assignment(Stream_c& arc_stream, uint16_t& rui16_functionObjId, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool);

  void checkAndHandleAux2MaintenanceTimeout(IsoAgLib::iIsoTerminalObjectPool_c& arc_pool);

  bool sendPreferredAux2Assignments();

  void setState(Aux2FunctionsState_en a_state) { m_state = a_state; }

  /**
   * set internal state and send empty preferred AUX2 assignment message (if we don't have any preferred assignments)
   */
  void objectPoolUploadedSuccessfully();

  void setWaitTimeForSendingPreferredAssignment(uint32_t a_waitTime) { m_deltaWaitForSendingPreferredAssignment = a_waitTime; }

  void setLearnMode(bool a_learnMode) { mb_learnMode = a_learnMode; }

private:

  // use this structure to store received data from input maintenance message in map with isoname as key
  struct InputMaintenanceDataForIsoName_s
  {
    InputMaintenanceDataForIsoName_s()
      : mui16_inputModelIdentificationCode(0xFFFF),
        mi32_timeLastAux2Maintenance(0)
    {}

    InputMaintenanceDataForIsoName_s(uint16_t a_inputModelIdentificationCode, int32_t a_timeLastAux2Maintenance)
      : mui16_inputModelIdentificationCode(a_inputModelIdentificationCode),
        mi32_timeLastAux2Maintenance(a_timeLastAux2Maintenance)
    {}

    uint16_t mui16_inputModelIdentificationCode;
    int32_t  mi32_timeLastAux2Maintenance;
  };

  // use this map:
  // 1. to detect input maintenance message timeout => unassign functions
  // 2. when storing preferred assignment (during processing of assign command from VT), get the model identificaton code from here (via matching isoname)
  STL_NAMESPACE::map<IsoName_c,InputMaintenanceDataForIsoName_s> mmap_receivedInputMaintenanceData;

#ifdef USE_VTOBJECT_auxiliaryfunction2
  STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*> mlist_aux2Function;
#endif

  // back reference for accessing functions in parent
  VtClientServerCommunication_c* m_vtClientServerCommunication;

  Aux2FunctionsState_en m_state;

  int32_t m_timeStampWaitForSendingPreferredAssignment;

  // set by application to delay sending of preferred assignment message to collect more input maintenance messages
  uint32_t m_deltaWaitForSendingPreferredAssignment;

  bool mb_learnMode;

};


}
#endif
