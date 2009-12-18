/*
  isorequestpgnhandler_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_REQUEST_PGN_HANDLER_C_H
#define ISO_REQUEST_PGN_HANDLER_C_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */

namespace __IsoAgLib {


class IsoItem_c;
/**
  This class implements the handling of Request_For_PGN

  @author Dipl.-Inf.(FH) Martina Winkler
*/
class IsoRequestPgnHandler_c : public CanCustomer_c
{

public:
  virtual ~IsoRequestPgnHandler_c() {};
  // IsoItem_c is intentionally not const, because the internal IsoAgLib classes are allowed to e.g. use sendSaClaim().
  virtual bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/) { return false; }
};

}
#endif
