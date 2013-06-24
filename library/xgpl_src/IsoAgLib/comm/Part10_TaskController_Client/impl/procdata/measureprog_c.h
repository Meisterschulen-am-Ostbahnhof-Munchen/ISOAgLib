/*
  measureprog_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASUREPROG_C_H
#define MEASUREPROG_C_H

#include "measuresubprog_c.h"
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

#include <list>


namespace __IsoAgLib {

  class ProcData_c;
  class TcClientConnection_c;

  class MeasureProg_c : public SchedulerTask_c {
    private:
      struct ThresholdInfo_s {
        IsoAgLib::ProcData::MeasurementCommand_t en_type;
        int32_t i32_threshold;
      };

      typedef STL_NAMESPACE::list<ThresholdInfo_s> ThresholdInfoContainer_t;
      typedef STL_NAMESPACE::list<ThresholdInfo_s>::iterator ThresholdInfoContainerIterator_t;
      typedef STL_NAMESPACE::list<ThresholdInfo_s>::const_iterator ThresholdInfoContainerConstIterator_t;

      typedef STL_NAMESPACE::list<MeasureSubprog_c> MeasureSubprogContainer_t;
      typedef STL_NAMESPACE::list<MeasureSubprog_c>::iterator MeasureSubprogContainerIterator_t;

    public:
      MeasureProg_c( TcClientConnection_c* c, ProcData_c& procdata);
      ~MeasureProg_c();

      bool startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment );
      void stopAllMeasurements();

      void processMeasurementMsg( ProcessPkg_c::CommandType_t command, int32_t pdValue );
      void processRequestMsg();
      void processSetMsg( int32_t pdValue );

      void timeEvent();

      void setValue( int32_t ai32_val );

      int32_t getValue() const {
        return m_value;
      }

      TcClientConnection_c* const GetConnection() const { return m_connection; }

    private:
      MeasureSubprog_c& addSubprog( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment );
      bool minMaxLimitsPassed( int32_t value ) const;
      void stopMeasurement( IsoAgLib::ProcData::MeasurementCommand_t ren_type );

      ThresholdInfoContainer_t m_thresholdInfo;
      MeasureSubprogContainer_t m_measureSubprog;

      /** not copyable : copy constructor/operators only declared, not defined */
      MeasureProg_c( const MeasureProg_c& );
      MeasureProg_c& operator=( const MeasureProg_c& );

      int32_t m_value;

      TcClientConnection_c* m_connection;
      ProcData_c& m_procdata;

  };

}

#endif
