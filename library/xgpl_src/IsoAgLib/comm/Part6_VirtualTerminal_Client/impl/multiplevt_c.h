/*
  multiplevt_c.h: handle "next VT"

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
#ifndef MULTIPLEVT_C_H
#define MULTIPLEVT_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivttypes.h>

#include <queue>

namespace __IsoAgLib {

  class UploadPoolState_c;
  class VtClient_c;
  class VtServerInstance_c;

  class MultipleVt_c
  {
    public:

      enum TimeEventResult
      {
        TIME_EVENT_RESULT_NO_ACTION,
        TIME_EVENT_RESULT_TRY_NEXT_VT
      };

      MultipleVt_c(const UploadPoolState_c& aref_uploadPoolState,
                   const VtClient_c& aref_vtClient);

      /*
       * - trigger multipleActiveVts() callback if "move to next VT" will be possible or not
       * - detect pool upload failure
       */
      TimeEventResult timeEvent();

      void resetInfoShown();

      void trySwitchingState();
      bool isSwitching() const;
      void resetSwitchingData();
      void setLastVtServerInstance(const VtServerInstance_c* ap_lastVtServerInstance);
      const VtServerInstance_c* getLastVtServerInstance() const;

    private:
      MultipleVt_c( const MultipleVt_c& ); // non-copyable
      MultipleVt_c& operator=( const MultipleVt_c& );

      enum State
      {
        STATE_INACTIVE,
        STATE_SWITCH_REQUESTED
      };

      enum vtInfoShownState
      {
        ONE_VT,
        MORE_THAN_ONE_VT,
        NOT_SET
      };

      const UploadPoolState_c& mref_uploadPoolState;
      const VtClient_c&        mref_vtClient;
      vtInfoShownState m_moveToNextVtInfoShown;
      State m_state;
      const VtServerInstance_c* mpc_lastVtServerInstance;
  };
}

#endif
