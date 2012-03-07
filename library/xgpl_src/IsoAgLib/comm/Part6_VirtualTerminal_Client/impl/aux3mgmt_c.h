/*

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef AUX3_STATE_ENGINE_H
#define AUX3_STATE_ENGINE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include "../ivttypes.h"

#if 0

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** class for central management of AUX3 */
class Aux3Mgmt_c : public SaClaimHandler_c
{
private:
  /** operation states of AUX3 */
  typedef enum {
    Idle, ///< initial state - never reached as soon as left once
    /** in this intermediate state, suitable Preferred Assignments are searched
        (just for description - intermediate state of exacution of reactOnIsoItemModification
    */
    LookupPrefAssign,
    UploadPrefAssign, ///< in case a suitable Preferred Assignment is found, this setup is uploaded to VT
    AwaitAuxAssign, ///< as soon as Preferred Assignment is uploaded, wait for receive of AUX assignments for all contained Aux Inputs
    UpdatePrefAssignDb, ///< during this intermediate state, the operator defined AUX assignment is stored
    Operation ///< normal operation mode
  } Aux3States_t;
  /**
    Any rejected assignment from a preferred assignment request is stored in a node of
    Aux3ConcurringAssignment_s. Thus the BUS and assignment monitoring can easily detect, when
    a new and better preferred assignment could be requested from VT.
  */
  typedef struct {
    uint8_t mui8_aux3InputObjId; ///< object id of the monitored AUX3 input
    IsoName_c mc_aux3InputDevice; ///< ISONAME of the AUX3 input device which should provide the wanted AUX3 function
    /** ISONAME of the ECU which got assignment, while assignment request of this
        has been rejected. In case the concurring devices leaves BUS or looses assignment,
        a new preferred assignment request with this AUX3 input can be requested.
    */
    IsoName_c mc_concurringSingleExclusivelyAssignedDevice;
  } Aux3ConcurringAssignment_s;
public:
  /** constructor
     @param reference to VT object pool data for direct initialization of mvec_aux3FunctionObject
   */
  Aux3Mgmt_c(IsoAgLib::iIsoTerminalObjectPool_c& arc_pool)
  : mc_prefAssignMgmt(arc_pool), mc_aux3MessageHandling(*this), men_aux3State(Idle){init();};

  /** initialize AUX3 management. called from constructor
      * register suitable event handlers to detect triggers
        for definition of new preferred assignments (esp. register in IsoMonitor_c
        as SA-Claim handler)
      * call defineSaMonitors to define suitabls ISONAME monitors
        (AUX Inputs and concurring AUX Function devices)
      * set state to Idle
  */
  init();

  /** check whether the system state has been changed. Check whether a new or better preferred assignment can
      be requested from VT. Check for availability of ISONAMEs from mvec_monitoredAux3InputDevices or
      loss of any concurring ISONAME from mvec_concurringAssignments4Monitoring.
      Use Aux3PrefAssignMgmt_c::deriveSelectOptimalPreferredAssignment() for checking
      set state to UploadPrefAssign if deriveSelectOptimalPreferredAssignment() indicates new preferred assignment
      else leave state unchanged
     @see IsoItemModification_en / IsoItemModification_t
     @param at_action enumeration indicating what happened to this IsoItem.
     @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
  */
  virtual void reactOnIsoItemModification (IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem);

  /** handle EndOfTP from TP for uploading a preferred assignment data set
      this function is called when MultiSend_c detects finish of TP.
      Change to state AwaitAuxAssign for awaiting of incoming AUX assignments
  */
  void handleFinishedPreferredAssignmentUpload();

  /** periodic activities:
    + perform storing of current preferred assignment in state UpdatePrefAssignDb
      with call to Aux3PrefAssignMgmt_c::storePreferredAssignmentIfNeeded
    @return true -> all activities performed
  */
  bool timeEvent();

  /** process a received AUX3 message.
      Use Aux3MessageHandling_c.
      Called from VtClientServerCommunication_c::processMsg()
   */
  bool processMsg(){ return mc_aux3MessageHandling.processMsg()};

private:
  /** allow Aux3MessageHandling_c to call internal functions */
  friend class Aux3MessageHandling_c;

  /** process assignment call - called by Aux3MessageHandling_c.
      Call Aux3PrefAssignMgmt_c::addAssignment() and return false, in case new assignment has been rejected or was wrong (not new)
      Afterwards check with Aux3PrefAssignMgmt_c::check4CompleteAssignment() whether all AUX3 Functions are now assigned.
      Change to state UpdatePrefAssignDb in case assignment process is complete.
      Change to state AwaitAuxAssign in case assignment process is not complete and current state is different from AwaitAuxAssign
      @param arc_inputIsoname ISONAME of the AUX3 input device
      @param ab_storeAsPreferred normally true - but VT might request no storage in error condition
      @param aui8_auxFunctionType type of the assigned input function
      @param aui16_auxInputObjId object ID of the assigned AUX3 input
      @param aui16_auxFunctionObjId object ID of the local AUX3 function that got the assignment
      @return true --> accept assignment, false --> do NOT accept
  */
  bool processAuxAssignment( const IsoName_c& arc_inputIsoname, bool ab_storeAsPreferred, uint8_t aui8_auxFunctionType. uint16_t aui16_auxInputObjId, uint16_t aui16_auxFunctionObjId );

  /** derive from known AUX3 input devices
      a list of to be monitored ISONAMEs.
      This is used for detection of relevant network state changes, which might trigger
      upload of better Preferred Assignment
      Call Aux3PrefAssignMgmt_c::retrieveMonitoredAux3InputDevices() to update mvec_monitoredAux3InputDevices
   */
   void defineSaMonitors();
  /** sub class for management of preferred assignments */
  Aux3PrefAssignMgmt_c mc_prefAssignMgmt;
  /** Aux3MessageHandling_c for message handling */
  Aux3MessageHandling_c mc_aux3MessageHandling;
  /** vector of ISONAMEs from known AUX3 input devices which can trigger selection
      of new Preferred Assignment set */
  std::vector<IsoName_c> mvec_monitoredAux3InputDevices;
  /** vector of concurring assignments, so that trigger for definition of new preferred
      assignment can be derived */
  std::vector<Aux3ConcurringAssignment_s> mvec_concurringAssignments4Monitoring;
  /** enum which stores the state of AUX3 control */
  Aux3States_t men_aux3State;

};

}
#endif
#endif
