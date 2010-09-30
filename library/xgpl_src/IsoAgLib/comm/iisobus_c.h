/*
  iisobus_c.h: Interface access class for the managed ISO11783-CAN-instances

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IISOBUS_C_H
#define IISOBUS_C_H

#include "impl/isobus_c.h"


namespace IsoAgLib {

/**
  Interface-class managing one ISOBUS CAN instance.
  @author Martin Wodok
  @see CanIo_c
*/
class iIsoBus_c : private __IsoAgLib::IsoBus_c {

 public:
  /** Initialize the CAN hardware and all related ISO-protocol modules.
      For possible errors refer to @see CanIo_c
      @param aui8_busNumber number of the physical CAN bus (starting at 0)
    */
  bool init (uint8_t aui8_busNumber) { return IsoBus_c::init (aui8_busNumber); }

  /** Close the CAN hardware and all related ISO-protocol modules.
      For possible errors refer to @see CanIo_c
    */
  void close() { IsoBus_c::close(); }

  /** @return CAN-Bus Number - 0xFF in case the bus is not initialized yet */
  uint8_t getBusNumber() const { return IsoBus_c::getBusNumber(); }

 private:
  /** allow getIisoBusInstance() access to shielded base class.
      otherwise __IsoAgLib::getIsoBusInstance() wouldn't be accepted by compiler
    */
  #if (PRT_INSTANCE_CNT > 1)
  friend iIsoBus_c& getIIsoBusInstance( uint8_t aui8_instance );
  #else
  friend iIsoBus_c& getIIsoBusInstance( void );
  #endif
};


/** C-style function, to get access to the unique singleton instance(s) */
#if (PRT_INSTANCE_CNT > 1)
  inline iIsoBus_c& getIIsoBusInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iIsoBus_c&>(__IsoAgLib::getIsoBusInstance(aui8_instance)); }
#else
  inline iIsoBus_c& getIIsoBusInstance( void )
  { return static_cast<iIsoBus_c&>(__IsoAgLib::getIsoBusInstance()); }
#endif


} // IsoAgLib

#endif // IISOBUS_C_H
