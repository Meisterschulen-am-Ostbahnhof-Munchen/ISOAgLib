/*
  aux2inputs_c.h - send Auxiliary Input Type 2 Maintenance message

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "aux2inputs_c.h"
#include "vtclient_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>

#include <IsoAgLib/driver/system/isystem_c.h>


namespace __IsoAgLib {

Aux2Inputs_c::Aux2Inputs_c( const IdentItem_c& arc_wsMasterIdentItem )
  : SchedulerTask_c( 10, true )
  , mrc_wsMasterIdentItem( arc_wsMasterIdentItem )
  , m_state(Aux2InputsState_NoAuxInputAvailable)
  , m_modelIdentificationCode( 0 )
  , mlist_auxInput2()
  , mi32_timeStampLastMaintenance( 0 )
  , mb_learnMode(false)
  , mp_vtClientServerCommunication( NULL )
{
}

void Aux2Inputs_c::init(VtClientConnection_c* ap_vtClientServerCommunication)
{
  getSchedulerInstance().registerTask( *this, 0 );

  setPeriod( 10, false );

  setState(Aux2InputsState_Initializing);

  mp_vtClientServerCommunication = ap_vtClientServerCommunication;
}

Aux2Inputs_c::~Aux2Inputs_c(void)
{
  if (Aux2InputsState_NoAuxInputAvailable != m_state)
  { // m_state is different from initial state => was registered in scheduler 
    getSchedulerInstance().deregisterTask( *this );
  }
}


void Aux2Inputs_c::timeEvent(void)
{
  switch (m_state)
  {
    case Aux2InputsState_Initializing:
    case Aux2InputsState_Ready:
    {
      if (mrc_wsMasterIdentItem.isClaimedAddress())
      {
        const ecutime_t i32_currentTime = HAL::getTime();

        // send each 100msec, period of timeEvent() is < 100msec!
        if (i32_currentTime - mi32_timeStampLastMaintenance > 100)
        {
          CanPkgExt_c sendData;
          sendData.setExtCanPkg8(
            7, 0, ECU_TO_VT_PGN>>8,
            0xFF, mrc_wsMasterIdentItem.getIsoItem()->nr(),
            0x23, // auxiliary input maintenance command
            (m_modelIdentificationCode & 0xFF),
            (m_modelIdentificationCode >> 8),
            m_state,
            0xFF, 0xFF, 0xFF, 0xFF);
          
          // additionally set IsoName for SA
          // => IsoName can be extracted from sendData in notifyAllVtClientsOnAux2InputMaintenance()
          sendData.setISONameForSA(mrc_wsMasterIdentItem.isoName());

          getIsoBusInstance( mrc_wsMasterIdentItem.getMultitonInst() ) << sendData;

          mi32_timeStampLastMaintenance = i32_currentTime;

          // if this application has also AUX2 functions which handle the AUX2 inputs
          // => notify them directly (sender won't get his own CAN message!)
          for( unsigned i=0; i<PRT_INSTANCE_CNT; ++i )
            getVtClientInstance( i ).notifyAllConnectionsOnAux2InputMaintenance( sendData );
        }
      }

      break;
    }
    case Aux2InputsState_NoAuxInputAvailable:
      // no inputs in pool => don't send maintenance message
      break;
  }

  timeEventInputStateMsg(NULL);
}


void Aux2Inputs_c::timeEventInputStateMsg(vtObjectAuxiliaryInput2_c* a_aux2InputObj)
{
  if (mrc_wsMasterIdentItem.isClaimedAddress())
  {
    CanPkgExt_c sendData;
    uint8_t ui8_destinationAddress = 0xFF; // default: broadcast this message
    uint8_t ui8_pduFormat = VT_TO_ECU_PGN>>8;

    if (mb_learnMode)
    { // send message only to VT with function instance 0
      // => send to VT instance in m_vtClientServerCommunication because the application should care that a pool containing AUX2 objects
      //    is only uploaded to a VT with function instance 0 (handled in initAndRegisterObjectPool())
      if (mp_vtClientServerCommunication->getVtServerInstPtr())
      {
        ui8_destinationAddress = mp_vtClientServerCommunication->getVtServerInstPtr()->getVtSourceAddress();
        ui8_pduFormat = ECU_TO_VT_PGN>>8;
      }
    }

    for (std::list<IsoAgLib::iVtObjectAuxiliaryInput2_c*>::iterator iter = mlist_auxInput2.begin(); iter != mlist_auxInput2.end(); ++iter)
    {
      // send message only when in learn mode or input enabled
      if (!mb_learnMode && !(*iter)->getInputStateEnabled())
        continue;

      if ((NULL != a_aux2InputObj) && (*iter != a_aux2InputObj))
        continue;

      // send status for all AUX2 input objects or only for the requested AUX2 input object
      const ecutime_t currentTime = HAL::getTime();

      // timing 1: do not send with less then 50 msec distance
      if (currentTime - (*iter)->getTimeStampLastStateMsg() >= 50)
      {
        // timing 2: send after 1sec for all
        // timing 3: send after 200msec for non latching booleans
        // timing 4: send immediately if change of state occured.
        if ( (currentTime - (*iter)->getTimeStampLastStateMsg() >= 1000) || 
              ((*iter)->highUpdateRateActive() && 
                 (currentTime - (*iter)->getTimeStampLastStateMsg() >= 200) ) || 
             (*iter)->sendNextStatusAsSoonAsPossible() )
        {

          uint8_t ui8_byte8 = 0;
          if (mb_learnMode)
          {
            ui8_byte8 = (1<<0);
            ui8_byte8 |= ((*iter)->getInputActivatedInLearnMode() ? (1<<1) : 0);
          }

          sendData.setExtCanPkg8 ( 7, 0, ui8_pduFormat,
                                   ui8_destinationAddress, mrc_wsMasterIdentItem.getIsoItem()->nr(),
                                   0x26, // auxiliary input type 2 status message
                                   ((*iter)->getID() & 0xFF),
                                   ((*iter)->getID() >> 8),
                                   ((*iter)->getValue1() & 0xFF),
                                   ((*iter)->getValue1() >> 8),
                                   ((*iter)->getValue2() & 0xFF),
                                   ((*iter)->getValue2() >> 8),
                                   ui8_byte8);

          // additionally set IsoName for SA
          // => IsoName can be extracted from sendData in notifyAllVtClientsOnAux2InputStatus()
          sendData.setISONameForSA(mrc_wsMasterIdentItem.isoName());

          getIsoBusInstance( mrc_wsMasterIdentItem.getMultitonInst() ) << sendData;

          (*iter)->setTimeStampLastStateMsg();

          (*iter)->updateValueForHeldPositionAfterFirstMsg();

          if (!mb_learnMode)
          { // if this application has also AUX2 functions which handle the AUX2 inputs
            // => notify them directly (sender won't get his own CAN message!)
            for( unsigned i=0; i<PRT_INSTANCE_CNT; ++i )
              getVtClientInstance( i ).notifyAllConnectionsOnAux2InputStatus( sendData );
          }
          else
          {
            // in learn mode, the message is only sent to VT => do not notify all VT clients
          }
        }
      }
    }
  }    
}



bool Aux2Inputs_c::setInputStateEnabledInObjects(uint16_t aui16_inputObjId, bool a_enabled)
{
  bool b_objFound = false;
  for (std::list<IsoAgLib::iVtObjectAuxiliaryInput2_c*>::iterator iter = mlist_auxInput2.begin(); iter != mlist_auxInput2.end(); ++iter)
  {
    if ( ((*iter)->getID() == aui16_inputObjId) || (0xFFFF == aui16_inputObjId))
    {
      (*iter)->setInputStateEnabled(a_enabled);
      b_objFound = true;
    }
  }
  return b_objFound;
}

    void Aux2Inputs_c::setState(Aux2Inputs_c::Aux2InputsState_en a_state) { m_state = a_state; }

    void Aux2Inputs_c::setModelIdentificationCode(uint16_t a_model) { m_modelIdentificationCode = a_model; }

    std::list<IsoAgLib::iVtObjectAuxiliaryInput2_c *> &Aux2Inputs_c::getObjectList() { return mlist_auxInput2; }

    void Aux2Inputs_c::setLearnMode(bool a_learnMode) { mb_learnMode = a_learnMode; }


}
