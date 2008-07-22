/***************************************************************************
                          ivtclientservercommunication.h - Interface class
                                        for vt-client-server communication
                             -------------------
    begin                : Thu Oct 05 2006
    copyright            : (C) 2006 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
    type                 : Header
    $LastChangedDate: 2006-10-05 09:49:23 +0200 (Do, 05 Okt 2006) $
    $LastChangedRevision:  $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2006 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef IVT_CLIENT_SERVER_COMMUNICATION_H
#define IVT_CLIENT_SERVER_COMMUNICATION_H

//include the declaration of the internal object
#include "impl/vtclientservercommunication_c.h"
#include "iisoterminal_c.h"
#include "impl/vtserverinstance_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Instance Object for one VT-Client-Server communication channel
  @author Dipl.-Inf. Martin Wodok
  */
class iVtClientServerCommunication_c : private __IsoAgLib::VtClientServerCommunication_c
{
public:

  bool isVtActive() { return VtClientServerCommunication_c::isVtActive(); }

  bool sendCommandDeleteObjectPool()
  {return VtClientServerCommunication_c::sendCommandDeleteObjectPool();}

  bool sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects)
    { return VtClientServerCommunication_c::sendCommandUpdateObjectPool (rppc_vtObjects, aui16_numObjects); }

  bool sendCommandChangeNumericValue (uint16_t aui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeNumericValue(aui16_objectUid, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);}
  bool sendCommandChangeAttribute    (uint16_t aui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeAttribute(aui16_objectUid, attrId, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);}
  bool sendCommandChangeSoftKeyMask  (uint16_t aui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeSoftKeyMask(aui16_objectUid, maskType, newSoftKeyMaskID, b_enableReplaceOfCmd);}
  bool sendCommandChangeStringValue  (uint16_t aui16_objectUid, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeStringValue(aui16_objectUid, apc_newValue, overrideSendLength, b_enableReplaceOfCmd);}
  bool sendCommandChangeChildPosition (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeChildPosition(aui16_objectUid, aui16_childObjectUid, x, y, b_enableReplaceOfCmd);}
  bool sendCommandChangeChildLocation (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeChildLocation(aui16_objectUid, aui16_childObjectUid, dx, dy, b_enableReplaceOfCmd);}
  bool sendCommandChangeBackgroundColour (uint16_t aui16_objectUid, uint8_t newColour,  bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeBackgroundColour(aui16_objectUid, newColour,  b_enableReplaceOfCmd);}
  bool sendCommandChangeSize             (uint16_t aui16_objectUid, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeSize(aui16_objectUid, newWidth, newHeight, b_enableReplaceOfCmd);}
  bool sendCommandChangeFillAttributes (uint16_t aui16_objectUid, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeFillAttributes(aui16_objectUid, newFillType, newFillColour, newFillPatternObject, b_enableReplaceOfCmd);}
  bool sendCommandChangeFontAttributes (uint16_t aui16_objectUid, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeFontAttributes(aui16_objectUid, newFontColour, newFontSize, newFontType, newFontStyle, b_enableReplaceOfCmd);}
  bool sendCommandChangeLineAttributes (uint16_t aui16_objectUid, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeLineAttributes(aui16_objectUid, newLineColour, newLineWidth, newLineArt, b_enableReplaceOfCmd);}
  bool sendCommandHideShow (uint16_t aui16_objectUid, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandHideShow(aui16_objectUid, b_hideOrShow, b_enableReplaceOfCmd);}
  bool sendCommandEsc (bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandEsc(b_enableReplaceOfCmd);}

  uint8_t getClientId() const
    { return VtClientServerCommunication_c::getClientId(); }

  iIdentItem_c& getIdentItem() { return static_cast<IsoAgLib::iIdentItem_c&>(VtClientServerCommunication_c::getIdentItem()); }

  /** @todo SOON Make an iVtServerInstance_c !*/
  const __IsoAgLib::VtServerInstance_c& getVtServerInst() { return VtClientServerCommunication_c::getVtServerInst(); }

private:
  /** PRIVATE constructor to forbid instantiation of this interface class.
    * it can only be static_cast'ed to this class, not constructed!
    */
  iVtClientServerCommunication_c (iIdentItem_c& arc_wsMasterIdentItem,iIsoTerminal_c& ar_isoTerminal,iIsoTerminalObjectPool_c& arc_pool,char* apc_versionLabel, uint8_t ui8_clientID SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  : __IsoAgLib::VtClientServerCommunication_c(
      static_cast<__IsoAgLib::IdentItem_c&>(arc_wsMasterIdentItem),
      static_cast<__IsoAgLib::IsoTerminal_c&>( ar_isoTerminal),
      arc_pool, apc_versionLabel, ui8_clientID SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA)
  {}

  friend class iIsoTerminal_c;
  friend class __IsoAgLib::VtClientServerCommunication_c;
};

}
#endif
