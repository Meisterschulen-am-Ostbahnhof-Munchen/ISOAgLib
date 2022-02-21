/*
  timedate_c.h: Handling of Time/Date information from the ISOBUS.

  (C) Copyright 2015 - 2019 by OSB AG

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
#ifndef TIMEDATE_C_H
#define TIMEDATE_C_H

#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/basecommon_c.h>


namespace __IsoAgLib {


class TimeDate_c : public BaseCommon_c
{
  public:
    multiton getMultitonInst() const { return mi_multitonInst; }
    void setMultitonInst(multiton ai_instance) { mi_multitonInst = ai_instance; }
private:
    multiton mi_multitonInst;
public:
  const IsoAgLib::iTimezone_s &getTimezone() const;
  const IsoAgLib::iDateTime_s &get( IsoAgLib::TimeStandard_t ) const;

  ecutime_t lastUpdateTime() const { return BaseCommon_c::lastUpdateTime(); }

  // @return true => Request was sent on the bus. false => IdentItem not properly claimed!
  bool requestUpdate();

protected:
  TimeDate_c();
  virtual ~TimeDate_c() {}

  /** @param apc_ident pointer to the variable of the responsible member instance. If NULL the module cannot send requests!
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  virtual bool config_base ( IdentItem_c* apc_ident, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask );

private:
  // SchedulerTask_c
  virtual void timeEvent();

  // from BaseCommon_c
  virtual void init_specialized();
  virtual void close_specialized();
  virtual void checkCreateReceiveFilter() {};


  virtual bool processMsgRequestPGN(
    uint32_t /*aui32_pgn*/,
    IsoItem_c* /*apc_isoItemSender*/,
    IsoItem_c* /*apc_isoItemReceiver*/,
    ecutime_t /*ai_time*/ ) { return false; } // not used in here!

  void processMsg( const CanPkg_c& arc_data );

private:
  IsoAgLib::iDateTime_s m_dateTime[ 3 ];
  IsoAgLib::iTimezone_s m_timezone;

  friend TimeDate_c &getTimeDateInstance( unsigned instance );
};


TimeDate_c &getTimeDateInstance( unsigned instance );


inline
const IsoAgLib::iTimezone_s &
TimeDate_c::getTimezone() const
{
  return m_timezone;
}


inline
const IsoAgLib::iDateTime_s &
TimeDate_c::get( IsoAgLib::TimeStandard_t ts ) const
{
  return m_dateTime[ ts ];
}


} // __IsoAgLib

#endif
