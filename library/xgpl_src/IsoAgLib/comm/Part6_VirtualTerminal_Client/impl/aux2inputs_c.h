/*
  aux2inputs_c.h:
  - send Auxiliary Input Type 2 Maintenance message

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
#ifndef AUX2INPUTS_C_H
#define AUX2INPUTS_C_H

#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/util/impl/multiton.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>

#include "../ivtobjectauxiliaryinput2_c.h"


namespace __IsoAgLib {

class VtClientConnection_c;

class Aux2Inputs_c : public SchedulerTask_c
{
public:
  enum Aux2InputsState_en 
  {
    Aux2InputsState_Initializing = 0,
    Aux2InputsState_Ready = 1,
    Aux2InputsState_NoAuxInputAvailable
  };

  Aux2Inputs_c(const IdentItem_c& arc_wsMasterIdentItem);
  virtual ~Aux2Inputs_c(void);

  void init(VtClientConnection_c* a_vtClientServerCommunication);

  /** send AUX2 input maintenance message (100msec) and call timeEventInputStateMsg(NULL) (1sec) */
  void timeEvent(void);

  void setState(Aux2InputsState_en a_state);

  void setModelIdentificationCode(uint16_t a_model);

#ifdef CONFIG_USE_VTOBJECT_auxiliaryinput2
  std::list<IsoAgLib::iVtObjectAuxiliaryInput2_c*>& getObjectList();
#endif

  /**
    set enabled state for matching input object ID (0xFFFF => all input objects)
    @return true if at least one object ID matches
  */
  bool setInputStateEnabledInObjects(uint16_t aui16_inputObjId, bool a_enabled);


#ifdef CONFIG_USE_VTOBJECT_auxiliaryinput2
  /**
    send AUX2 input status message for all inputs (a_aux2InputObj == NULL) or only for passed AUX2 input object
  */
  void timeEventInputStateMsg(vtObjectAuxiliaryInput2_c* a_aux2InputObj);
#endif

  void setLearnMode(bool a_learnMode);

private:
  const IdentItem_c& mrc_wsMasterIdentItem;

  Aux2InputsState_en m_state;

  uint16_t m_modelIdentificationCode;

#ifdef CONFIG_USE_VTOBJECT_auxiliaryinput2
  std::list<IsoAgLib::iVtObjectAuxiliaryInput2_c*> mlist_auxInput2;
#endif

  // time stamp of last auxiliary input maintenance message sent
  ecutime_t mi32_timeStampLastMaintenance;

  bool mb_learnMode;

  VtClientConnection_c* mp_vtClientServerCommunication;

private:
  /** not copyable : copy constructor/operator only declared, not defined */
  Aux2Inputs_c( const Aux2Inputs_c& );
  Aux2Inputs_c& operator=( const Aux2Inputs_c& );
};

}

#endif
