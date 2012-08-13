/*
  iprocdatahandler_c.h: base class for application handler

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCDATAHANDLER_C_H
#define IPROCDATAHANDLER_C_H

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>


namespace IsoAgLib {

class iProcData_c;

class iProcDataHandler_c
{
public:
  virtual ~iProcDataHandler_c() {}

  virtual void processSetpointSet( iProcData_c& procdata, int32_t value, IsoAgLib::ProcData::remoteType_t a_ecuType, bool change ) = 0;
  virtual void processDefaultLoggingStart( IsoAgLib::ProcData::remoteType_t ecuType ) = 0;
  virtual void processTcConnected( IsoAgLib::ProcData::remoteType_t ecuType, const IsoAgLib::iIsoName_c& tcname) = 0;

  virtual void processTaskStarted( IsoAgLib::ProcData::remoteType_t ecuType ) = 0;
  virtual void processTaskStopped( IsoAgLib::ProcData::remoteType_t ecuType ) = 0;
};

}

#endif
