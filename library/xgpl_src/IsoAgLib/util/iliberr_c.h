/*
  iliberr_c.h: central object for error management

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

#ifndef _ILIB_ERR_H
#define _ILIB_ERR_H

#include <IsoAgLib/isoaglib_config.h>

#include <IsoAgLib/util/impl/multiton.h>
#include <IsoAgLib/util/impl/container.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>


// Begin Namespace IsoAgLib
namespace IsoAgLib {
  class iLibErr_c;
  class iErrorObserver_c;

  /** C-style function, to get access to the unique iLibErr_c singleton instance */
  iLibErr_c &getILibErrInstance();

/**
  Basic object for Error Management:
  some functions for state check and setting of error state;
  @author Dipl.-Inform. Achim Spangler
  @author Dipl.-Inform. Martin Wodok
*/
class iLibErr_c {
public:
  void init();
  void close();

  enum TypeNonFatal_en {
    VtOutOfStorageSpace,
    VtOutOfMemory,
    MonitorInvalidDa,
    MonitorInvalidSa,
    MonitorSaRxConflict, /* rxed a message with a locally claimed SA */
    MonitorNameRxConflict, /* rxed an addres claim with a locally registered NAME */
    TracMultipleSender,
    TracError,
    HalCanBusWarn,
    HalCanBusOff,
    HalCanBusOverflow,
    HalEpromError,
    HalEpromWriteError,
    ProcData,
    ProcDataSetpoint,
    TcOutOfMemory,
    MultiReceiveTpAbort,
    TypeNonFatalSize
  };

  enum State_en {
    Inactive,
    Active
  };

  void registerNonFatal( TypeNonFatal_en at_errType, int instance );

  State_en state( TypeNonFatal_en type, int instance ) const {
    return ( m_nonFatal[ type ] & ( 1 << instance ) ? Active : Inactive );
  }

  bool good( TypeNonFatal_en type, int instance ) const {
    return ( Inactive == state( type, instance ) );
  }

  void clear( TypeNonFatal_en type, int instance ) { 
    m_nonFatal[ type ] &= uint16_t(~( 1 << instance ));
  }

  void registerObserver( iErrorObserver_c &arc_observer );
  void deregisterObserver( iErrorObserver_c &arc_observer );

private:
  iLibErr_c();

  uint16_t m_nonFatal[ TypeNonFatalSize ];

  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(iErrorObserver_c)
  friend iLibErr_c &getILibErrInstance();
};

class iErrorObserver_c {
public:
  typedef iLibErr_c::TypeNonFatal_en TypeNonFatal_en;

  virtual ~iErrorObserver_c() {}

  void nonFatalError( TypeNonFatal_en type, int instance ) {
    onNonFatalError( type, instance );
  }

private:
  virtual void onNonFatalError( TypeNonFatal_en type, int instance ) = 0;
};


inline void iLibErr_c::registerObserver( iErrorObserver_c &arc_observer )
{
  registerC1( &arc_observer );
}


inline void iLibErr_c::deregisterObserver( iErrorObserver_c &arc_observer )
{
  unregisterC1( &arc_observer );
}


} // IsoAgLib

#endif
