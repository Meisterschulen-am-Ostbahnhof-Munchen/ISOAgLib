/*
  aux2functions_c.h:

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "aux2functions_c.h"
#include "isoterminal_c.h"
#include "vtclientservercommunication_c.h"

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>


namespace __IsoAgLib {

  // this structure is used as map key for collecting all preferred AUX2 assignments
  // (sorted by isoname and model identification of the preferred AUX2 input device)
  struct Aux2PreferredAssignmentInputDevice_s
  { // operator< is necessary for using it as map key
    bool operator<(const Aux2PreferredAssignmentInputDevice_s& ref) const
    { 
      if (ui16_model == ref.ui16_model)
        return (isoName < ref.isoName);
      else
        return (ui16_model < ref.ui16_model);
    }

    IsoName_c isoName;
    uint16_t ui16_model;
  };

  struct Aux2PreferredAssignmentObjects_s
  {
    uint16_t ui16_input;
    uint16_t ui16_function;
  };


Aux2Functions_c::Aux2Functions_c(VtClientServerCommunication_c* a_vtClientServerCommunication)
  : m_vtClientServerCommunication(a_vtClientServerCommunication),
    m_state(State_WaitForPoolUploadSuccessfully),
    m_timeStampWaitForSendingPreferredAssignment(0),
    m_deltaWaitForSendingPreferredAssignment(0),
    mb_learnMode(false)
{
}


Aux2Functions_c::~Aux2Functions_c(void)
{
}


void
Aux2Functions_c::notifyOnAux2InputStatus( const CanPkgExt_c& arc_data, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool)
{
#ifdef USE_VTOBJECT_auxiliaryfunction2
  if (mb_learnMode || ((arc_data.getUint8Data(8-1) & 0x3) != 0))
    return; // do not respond to input status messages in learn mode or learn mode bits are set in received message

  const IsoName_c& c_inputIsoName = arc_data.getISONameForSA();
  uint16_t const cui16_inputObjectId = arc_data.getUint16Data(2-1);

  IsoName_c c_assignedIsoName;
  uint16_t ui16_assignedInputUid = 0xFFFF;

  for (STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*>::iterator iter = mlist_aux2Function.begin(); iter != mlist_aux2Function.end(); ++iter)
  {
    (*iter)->getAssignedInput(c_assignedIsoName, ui16_assignedInputUid);

    if ( (0xFFFF != ui16_assignedInputUid) &&
         (IsoName_c::IsoNameUnspecified() != c_assignedIsoName) &&
         (cui16_inputObjectId == ui16_assignedInputUid) &&
         (c_inputIsoName == c_assignedIsoName) )
    { // notify application on this new Input Status!
      const uint16_t cui16_value1 = arc_data.getUint16Data (4-1);
      const uint16_t cui16_value2 = arc_data.getUint16Data (6-1);
      const uint8_t cui8_operatingState = arc_data.getUint8Data (8-1);
      arc_pool.eventAuxFunction2Value ((*iter)->getID(), cui16_value1, cui16_value2, cui8_operatingState);
    }
  }
#endif
}

void
Aux2Functions_c::notifyOnAux2InputMaintenance( const CanPkgExt_c& arc_data)
{
#ifdef USE_VTOBJECT_auxiliaryfunction2
  bool b_sendPreferredAssignments = false;

  const int32_t i32_now = HAL::getTime();
  const IsoName_c c_inputIsoName = arc_data.getISONameForSA();
  const uint16_t ui16_modelIdentificationCode = arc_data.getUint16Data(2-1);

  if (c_inputIsoName.isSpecified())
    mmap_receivedInputMaintenanceData[c_inputIsoName] = InputMaintenanceDataForIsoName_s(ui16_modelIdentificationCode, i32_now);

  const uint8_t ui8_status = arc_data.getUint8Data(4-1);
  if (1 != ui8_status)
    return; // inputs with status not "ready" should not trigger sendPreferredAux2Assignments()


  switch (m_state)
  {
    case State_WaitForPoolUploadSuccessfully:
      break; // nothing to do

    case State_WaitForFirstInputMaintenanceMessage:
      // we have some preferred assignments (otherwise this state would not be set in objectPoolUploadedSuccessfully
      // => let's wait for some time to receive input maintenance messages from different devices to send a more complete preferred assignment message
      m_state = State_CollectInputMaintenanceMessage;
      m_timeStampWaitForSendingPreferredAssignment = HAL::getTime();
      break;

    case State_CollectInputMaintenanceMessage:
    case State_Ready:
      break; // handle these cases in next switch, because m_state might have been modified in the above cases.
  }

  IsoName_c c_prefAssignedIsoName;
  uint16_t ui16_prefAssignedModelIdentificationCode;
  uint16_t ui16_prefAssignedInputUid;

  switch (m_state)
  {
    case State_WaitForPoolUploadSuccessfully:
    case State_WaitForFirstInputMaintenanceMessage:
      break; // handled above

    case State_CollectInputMaintenanceMessage:
      for (STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*>::iterator iter = mlist_aux2Function.begin(); iter != mlist_aux2Function.end(); ++iter)
      {
        if ((*iter)->getMatchingPreferredAssignedInputReady())
          continue; // this input device was already found in our preferred assignments => skip it
        
        (*iter)->getPreferredAssignedInput(c_prefAssignedIsoName, ui16_prefAssignedModelIdentificationCode, ui16_prefAssignedInputUid);
        if ( (ui16_prefAssignedModelIdentificationCode == ui16_modelIdentificationCode) &&
             (c_prefAssignedIsoName == c_inputIsoName) )
        { 
          (*iter)->setMatchingPreferredAssignedInputReady(true);
        }
      }

      if (m_timeStampWaitForSendingPreferredAssignment + (int32_t)m_deltaWaitForSendingPreferredAssignment < HAL::getTime())
      { // we waited long enough
        b_sendPreferredAssignments = true;
      }
      break;

    case State_Ready:
      break; // nothing to do
  }

  if (b_sendPreferredAssignments)
    sendPreferredAux2Assignments();

#endif
}

void
Aux2Functions_c::objectPoolUploadedSuccessfully()
{
#ifdef USE_VTOBJECT_auxiliaryfunction2
  // if we don't have any preferred assignments => send empty preferred assignment msg
  bool b_preferredAssignmentFound = false;
  for (STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*>::iterator iter = mlist_aux2Function.begin(); iter != mlist_aux2Function.end(); ++iter)
  {
    b_preferredAssignmentFound |= (*iter)->hasPreferredAssigment();
  }

  if (!b_preferredAssignmentFound)
  { // set State_Ready
    sendPreferredAux2Assignments();
  }
  else
  { // we have preferred assignments => wait for first input maintenance message
    m_state = State_WaitForFirstInputMaintenanceMessage;
  }
#endif
}

bool
Aux2Functions_c::storeAux2Assignment(Stream_c& arc_stream, uint16_t& rui16_functionObjId, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool)
{
#ifdef USE_VTOBJECT_auxiliaryfunction2
  const uint16_t ui16_totalstreamsize = arc_stream.getByteTotalSize();
  
  if (ui16_totalstreamsize != 14)
    return false; // message should always have 14 bytes

  STL_NAMESPACE::vector<uint8_t> c_buffer;
  for (uint16_t i = 0; i < arc_stream.getByteTotalSize(); i++)
  {
    c_buffer.push_back( arc_stream.get() );
  }

  // first byte already consumed from stream!
  const IsoName_c c_inputIsoName(&c_buffer[2-2]);
  const bool b_preferredAssignment = !(c_buffer[10-2] & (1 << 7));
  uint16_t const cui16_inputObjId = (c_buffer[11-2] | (c_buffer[12-2] << 8));

  bool b_assignedInputsChanged = false;
  bool b_preferredAssignedInputsChanged = false;

  // set reference of function object ID (is needed for response in caller)
  rui16_functionObjId = (c_buffer[13-2] | (c_buffer[14-2] << 8));

  if (mmap_receivedInputMaintenanceData.find(c_inputIsoName) == mmap_receivedInputMaintenanceData.end())
    return false; // we did not yet receive a input maintenance message for this isoname => unknown

  for (STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*>::iterator iter = mlist_aux2Function.begin(); iter != mlist_aux2Function.end(); ++iter)
  {
    if ((*iter)->getID() == rui16_functionObjId)
    { // matching iVtObjectAuxiliaryFunction2_c found 
      // store / remove assignment in setAssignedInput (according to value of cui16_inputObjId)
      b_assignedInputsChanged |= (*iter)->setAssignedInput(c_inputIsoName, cui16_inputObjId);

      if (b_preferredAssignment)
      { // additionally store this assignement as preferred

        // try to get model identification code from mmap_receivedInputMaintenanceData (sort of a "backdoor" method because we do not get the code in the message!)
        STL_NAMESPACE::map<IsoName_c,InputMaintenanceDataForIsoName_s>::const_iterator iter_map = mmap_receivedInputMaintenanceData.find(c_inputIsoName);
        uint16_t ui16_modelIdentificationCode = (mmap_receivedInputMaintenanceData.end() != iter_map)
          ? iter_map->second.mui16_inputModelIdentificationCode : 0xFFFF;

        b_preferredAssignedInputsChanged |= (*iter)->setPreferredAssignedInput(c_inputIsoName, ui16_modelIdentificationCode, cui16_inputObjId);
      }
    }

    if (0xFFFF == rui16_functionObjId)
    { // remove all assignments
      b_assignedInputsChanged |= (*iter)->setAssignedInput(IsoName_c::IsoNameUnspecified(), 0xFFFF);

      if (b_preferredAssignment)
        b_preferredAssignedInputsChanged |= (*iter)->setPreferredAssignedInput(IsoName_c::IsoNameUnspecified(), 0xFFFF /* default value */, 0xFFFF);
    }
  }

  if (b_assignedInputsChanged)
    arc_pool.aux2AssignmentChanged();

  if (b_preferredAssignedInputsChanged)
    arc_pool.aux2PreferredAssignmentChanged();

#endif
  return true;
}

void
Aux2Functions_c::checkAndHandleAux2MaintenanceTimeout(IsoAgLib::iIsoTerminalObjectPool_c& arc_pool)
{
#ifdef USE_VTOBJECT_auxiliaryfunction2
  bool b_assignedInputsChanged = false;
  int32_t i32_now = HAL::getTime();

  for (STL_NAMESPACE::map<IsoName_c,InputMaintenanceDataForIsoName_s>::iterator iter_map = mmap_receivedInputMaintenanceData.begin(); iter_map != mmap_receivedInputMaintenanceData.end(); ++iter_map)
  {
    if ( (iter_map->second.mi32_timeLastAux2Maintenance != 0) &&
         (i32_now > iter_map->second.mi32_timeLastAux2Maintenance + 300) )
    {
      // timeout => unassign all functions with matching isoname
      for (STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*>::const_iterator iter_function = mlist_aux2Function.begin(); iter_function != mlist_aux2Function.end(); ++iter_function)
      {
        b_assignedInputsChanged |= (*iter_function)->unassignInputIfIsoNameMatches(iter_map->first);
      }

      mmap_receivedInputMaintenanceData.erase(iter_map);
      break; // do not continue in map iteration after erase (other elements are checked soon in next call)
    }
  }

  if (b_assignedInputsChanged)
    arc_pool.aux2AssignmentChanged();
#endif
}


bool
Aux2Functions_c::sendPreferredAux2Assignments()
{
#ifdef USE_VTOBJECT_auxiliaryfunction2
  m_state = State_Ready;

  if (mlist_aux2Function.size() == 0)
    return true;

  static SendUpload_c msc_tempSendUpload;
  Aux2PreferredAssignmentInputDevice_s key;
  Aux2PreferredAssignmentObjects_s objIds;
  STL_NAMESPACE::map<Aux2PreferredAssignmentInputDevice_s, STL_NAMESPACE::list<Aux2PreferredAssignmentObjects_s> > map_Inputs;

  for (STL_NAMESPACE::list<IsoAgLib::iVtObjectAuxiliaryFunction2_c*>::const_iterator iter = mlist_aux2Function.begin(); iter != mlist_aux2Function.end(); ++iter)
  {
    if (!(*iter)->getMatchingPreferredAssignedInputReady())
      continue; // skip preferred assignments for which we didn't detect a input maintenance message with ready state

    objIds.ui16_function = (*iter)->getID();

    (*iter)->getPreferredAssignedInput(key.isoName, key.ui16_model, objIds.ui16_input);
      
    if (key.isoName.isSpecified())
    {
      map_Inputs[key].push_back(objIds);
    }
  }

  msc_tempSendUpload.vec_uploadBuffer.clear();

  // calculate needed size for vec_uploadBuffer
  uint16_t msgSize = 2; // minimum size for no assignments
  uint8_t noInputUnits = 0;

  for (STL_NAMESPACE::map<Aux2PreferredAssignmentInputDevice_s, STL_NAMESPACE::list<Aux2PreferredAssignmentObjects_s> >::const_iterator iter = map_Inputs.begin(); iter != map_Inputs.end(); ++iter)
  {
    noInputUnits++;
    msgSize += 11;
    msgSize += iter->second.size() * 2 * sizeof(uint16_t);
  }

  msc_tempSendUpload.vec_uploadBuffer.reserve (msgSize);

  msc_tempSendUpload.vec_uploadBuffer.push_back (0x22);
  msc_tempSendUpload.vec_uploadBuffer.push_back (noInputUnits);

  for (STL_NAMESPACE::map<Aux2PreferredAssignmentInputDevice_s, STL_NAMESPACE::list<Aux2PreferredAssignmentObjects_s> >::const_iterator iter = map_Inputs.begin(); iter != map_Inputs.end(); ++iter)
  {
    // !!!!!!!!!!!!!!!!!!!!!!!!
    // @todo: check endianess ?
    // !!!!!!!!!!!!!!!!!!!!!!!!
    
    const uint8_t* pui8_isoName = iter->first.isoName.outputString(); 
    for (uint8_t ui8_i = 0; ui8_i < 8; ui8_i++)
    {
      msc_tempSendUpload.vec_uploadBuffer.push_back(pui8_isoName[ui8_i]);
    }

    msc_tempSendUpload.vec_uploadBuffer.push_back(iter->first.ui16_model & 0xFF);
    msc_tempSendUpload.vec_uploadBuffer.push_back(iter->first.ui16_model >> 8);
    msc_tempSendUpload.vec_uploadBuffer.push_back(iter->second.size());
    
    for (STL_NAMESPACE::list<Aux2PreferredAssignmentObjects_s>::const_iterator iter_list = iter->second.begin(); iter_list != iter->second.end(); ++iter_list)
    {
      msc_tempSendUpload.vec_uploadBuffer.push_back(iter_list->ui16_function & 0xFF);
      msc_tempSendUpload.vec_uploadBuffer.push_back(iter_list->ui16_function >> 8);
      msc_tempSendUpload.vec_uploadBuffer.push_back(iter_list->ui16_input & 0xFF);
      msc_tempSendUpload.vec_uploadBuffer.push_back(iter_list->ui16_input >> 8);
    }
  }

  // pad with 0xFF in case there is only a 8 byte message
  for (uint8_t ui8_i = msgSize; ui8_i < 8; ui8_i++)
  {
    msc_tempSendUpload.vec_uploadBuffer.push_back(0xFF);
  }
  
  return m_vtClientServerCommunication->queueOrReplace (msc_tempSendUpload, true /* b_enableReplaceOfCmd */);
#else
  return true;
#endif
}


}
