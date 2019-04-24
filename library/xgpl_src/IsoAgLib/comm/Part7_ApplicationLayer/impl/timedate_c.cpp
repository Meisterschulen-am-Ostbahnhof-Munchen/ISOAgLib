/*
  timedate_c.cpp: Handling of Time/Date information from the ISOBUS.

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
#include "timedate_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/util/iutil_funcs.h>
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/iassert.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#pragma warning( disable : 4355 )
#endif



namespace __IsoAgLib {


  TimeDate_c::TimeDate_c()
  {
    // wait 1s for a TIME_DATE response
    setTimeOut( 1000 );
  }


  // don't use BaseCommon_c::timeEvent
  void
  TimeDate_c::timeEvent()
  {
    getSelectedDataSourceISOName().setUnspecified();
    ( void )BaseCommon_c::sendPgnRequest( TIME_DATE_PGN );
  }


  bool
  TimeDate_c::requestUpdate()
  {
    if( BaseCommon_c::sendPgnRequest( TIME_DATE_PGN ) )
    {
      if( getSelectedDataSourceISOName().isSpecified() )
      {
        if( mt_task.isRegistered() )
          mt_task.setNextTriggerTime( HAL::getTime() + getTimeOut() );
        else
          getSchedulerInstance().registerTask(mt_task, getTimeOut() );
      }
      else
        if( mt_task.isRegistered() )
          getSchedulerInstance().deregisterTask( mt_task );

      return true;
    }

    return false;
  }

  void
  TimeDate_c::init_specialized()
  {
    getIsoBusInstance4Comm().insertFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (TIME_DATE_PGN<<8) ), 8 );
    // BaseCommon did register the task already :(
    getSchedulerInstance().deregisterTask(mt_task);
    mt_task.setPeriod( -1, false ); // one-shot = -1

    m_dateTime[ IsoAgLib::TimeStandardUtc ].timestamp = -1;
    m_dateTime[ IsoAgLib::TimeStandardLocal ].timestamp = -1;
    m_dateTime[ IsoAgLib::TimeStandardUnknown ].timestamp = -1;
  }

  void
  TimeDate_c::close_specialized()
  {
    getIsoBusInstance4Comm().deleteFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (TIME_DATE_PGN<<8) ) );
    // BaseCommon is about to deregister the task :(
    if( !mt_task.isRegistered() )
      getSchedulerInstance().registerTask(mt_task, 0);
  }

  bool TimeDate_c::config_base ( IdentItem_c* apc_ident, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask )
  {
    isoaglib_assert( at_identMode == IsoAgLib::IdentModeImplement );
    isoaglib_assert( aui16_suppressMask == 0 );

    return BaseCommon_c::config_base ( apc_ident, at_identMode, aui16_suppressMask );
  };

  bool isLeapYear( unsigned year )
  {
    return( ( ( year % 4 ) == 0 ) && ( ( year % 100 ) != 0 ) )
         || ( ( year % 400 ) == 0 );
  }

  unsigned daysInMonth( unsigned year, unsigned month )
  {
    switch( month )
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      return 31;

    case 4:
    case 6:
    case 9:
    case 11:
      return 30;

    case 2:
      return isLeapYear( year ) ? 29 : 28;

    default:
      isoaglib_assert( !"wrong month passed!" );
    }
    return 0; // shouldn't occur!
  }


  void TimeDate_c::processMsg( const CanPkg_c& frame )
  {
    CanPkgExt_c pkg( frame, getMultitonInst() );
    if( !pkg.isValid() || (pkg.getMonitorItemForSA() == NULL) )
      return;

    isoaglib_assert( pkg.isoPgn() == TIME_DATE_PGN );

    IsoName_c const& senderName = pkg.getISONameForSA();
    if ( checkParseReceived( senderName ) )
    {
      // received something, so don't kick out this sender anymore!
      if( mt_task.isRegistered() )
        getSchedulerInstance().deregisterTask(mt_task);

      static IsoAgLib::iDateTime_s tempDateTime;

      tempDateTime.timestamp = HAL::getTime();
      tempDateTime.date.year   = pkg.getUint8Data(5) + 1985;
      tempDateTime.date.month  = pkg.getUint8Data(3);
      tempDateTime.date.day    = (pkg.getUint8Data(4)+3) / 4;
      tempDateTime.time.hour   = pkg.getUint8Data(2);
      tempDateTime.time.minute = pkg.getUint8Data(1);
      tempDateTime.time.second = pkg.getUint8Data(0) / 4;
      tempDateTime.time.msec   = (pkg.getUint8Data(0) & 0x3) * 250;

      // completely ignore a message with a false/OoR date/time
      if( ( tempDateTime.date.month == 0 ) || ( tempDateTime.date.month > 12 )
          || ( tempDateTime.date.day == 0 ) || ( tempDateTime.date.day > daysInMonth( tempDateTime.date.year, tempDateTime.date.month ) )
          || ( tempDateTime.time.hour > 23 )
          || ( tempDateTime.time.minute > 59 )
          || ( tempDateTime.time.second > 59 ) )
      {
        if( getSelectedDataSourceISOName() == senderName )
          timeEvent();

        return;
      }

      m_timezone.minuteOffset = pkg.getUint8Data(6) - 125;
      m_timezone.hourOffset = pkg.getUint8Data(7) - 125;

      IsoAgLib::TimeStandard_t ts;
      if( ( m_timezone.hourOffset <= -24 )
        || ( ( m_timezone.hourOffset >= 24 ) && ( m_timezone.hourOffset <= 123 ) )
        || ( m_timezone.hourOffset >= 126 ) )
      {
        ts = IsoAgLib::TimeStandardUnknown;
        m_timezone.available = false;
      }
      else if( ( m_timezone.hourOffset >= -23 ) && ( m_timezone.hourOffset <= 23 ) )
      {
        ts = IsoAgLib::TimeStandardUtc;
        m_timezone.available = true;
      }
      else if( m_timezone.hourOffset == 124 )
      {
        ts = IsoAgLib::TimeStandardUtc;
        m_timezone.available = false;
      }
      else
      {
        isoaglib_assert( m_timezone.hourOffset == 125 );
        ts = IsoAgLib::TimeStandardLocal;
        m_timezone.available = false;
      }

      m_dateTime[ IsoAgLib::TimeStandardUtc ].timestamp = -1;
      m_dateTime[ IsoAgLib::TimeStandardLocal ].timestamp = -1;
      m_dateTime[ IsoAgLib::TimeStandardUnknown ].timestamp = -1;

      m_dateTime[ ts ] = tempDateTime;

      if( m_timezone.available )
      {
        // we received UTC with local time offsets, thus we can also provide local time
        IsoAgLib::iDateTime_s &tsLocal = m_dateTime[ IsoAgLib::TimeStandardLocal ];
        tsLocal = m_dateTime[ IsoAgLib::TimeStandardUtc ];

        int16_t minutesOnDay = tsLocal.time.minute + ( tsLocal.time.hour * 60 );
        int16_t minutesOffset = m_timezone.minuteOffset + ( m_timezone.hourOffset * 60 );

        minutesOnDay += minutesOffset;
        const int16_t minutesPerDay = 24 * 60;

        if( minutesOnDay > minutesPerDay )
        {
          minutesOnDay -= minutesPerDay;
          tsLocal.date.day += 1;
          if( tsLocal.date.day > daysInMonth( tsLocal.date.year, tsLocal.date.month ) )
          {
            tsLocal.date.day = 1;
            tsLocal.date.month += 1;
            if( tsLocal.date.month > 12 )
            {
              tsLocal.date.month = 1;
              tsLocal.date.year += 1;
            }
          }
        }
        else if( minutesOnDay < 0 )
        {
          minutesOnDay += minutesPerDay;
          tsLocal.date.day -= 1;
          if( tsLocal.date.day < 1 )
          {
            tsLocal.date.month -= 1;
            if( tsLocal.date.month < 1 )
            {
              tsLocal.date.month = 12;
              tsLocal.date.year -= 1;
            }
            tsLocal.date.day = daysInMonth( tsLocal.date.year, tsLocal.date.month );
          }
        }

        tsLocal.time.hour = minutesOnDay / 60;
        tsLocal.time.minute = minutesOnDay % 60;
      }

      setUpdateTime( pkg.time() );
      setSelectedDataSourceISOName( senderName );

      notifyOnEvent( pkg.isoPgn() );
    }
    else
    { // there is a sender conflict
      IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::TracMultipleSender, getMultitonInst() );
    }
  }

  TimeDate_c &getTimeDateInstance( unsigned instance )
  {
    MACRO_MULTITON_GET_INSTANCE_BODY(TimeDate_c, PRT_INSTANCE_CNT, instance);
  }


} // namespace __IsoAgLib

