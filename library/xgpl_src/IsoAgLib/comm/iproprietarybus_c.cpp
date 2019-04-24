/*
  iproprietarybus.cpp - Interface access class for the proprietary CAN-busses

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

#include <IsoAgLib/comm/iproprietarybus_c.h>

#if 0 < PROP_INSTANCE_CNT
namespace IsoAgLib
{

iProprietaryBus_c &getIProprietaryBusInstance( unsigned instance )
{
  MACRO_MULTITON_GET_INSTANCE_BODY(iProprietaryBus_c, PROP_INSTANCE_CNT, instance);
}

}
#endif
