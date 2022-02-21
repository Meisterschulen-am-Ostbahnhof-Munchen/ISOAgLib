/*
  vtobject_c.h

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
#ifndef VTOBJECT_C_H
#define VTOBJECT_C_H

#include "../ivtobject_c.h"
#include "vttypes.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/util/iassert.h>


namespace __IsoAgLib {

class vtObject_c : public virtual IsoAgLib::iVtObject_c
{
public:
  explicit vtObject_c(multiton ai_multitonInst);
  virtual ~vtObject_c() override = 0; // pure Virtual !

  virtual uint16_t stream(uint8_t* destMemory,
                         uint16_t maxBytes,
                         objRange_t sourceOffset) = 0;

  virtual uint32_t fitTerminal() const = 0;
 
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  virtual void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue) = 0;
#endif


  //! Internal checker function
  bool isOmittedFromUpload() const;

protected:

  enum OffsetMode_en
  {
    DataAlarmMaskOffset,
    SoftKeyOffset,
    NoOffset
  };

  void setAttribute      (uint8_t attrID, uint32_t newValue, bool b_enableReplaceOfCmd=true);
  void setAttributeFloat (uint8_t attrID, float newValue, bool b_enableReplaceOfCmd=true);

  #ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  void getAttribute      (uint8_t attrID, bool b_enableReplaceOfCmd=true);
  #endif


protected:


  void scaleSize( uint16_t &width, uint16_t &height ) const;

  bool genericChangeChildLocation (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_s* objectsToFollow, bool b_enableReplaceOfCmd=true);
  bool genericChangeChildPosition (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_s* objectsToFollow, bool b_enableReplaceOfCmd, OffsetMode_en);

  // ATTENTION: Should only be used by Input-Objects of course!
  bool able (IsoAgLib::Enabled enOrDis, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

        virtual bool select(uint8_t selectOrActivate);

private:
  friend class vtLayoutManager_c;
  bool genericChangeChildLocationPosition (bool ab_isLocation, IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_s* objectsToFollow);

  virtual void updateEnable(IsoAgLib::Enabled enOrDis);
};




} // __IsoAgLib

#endif
