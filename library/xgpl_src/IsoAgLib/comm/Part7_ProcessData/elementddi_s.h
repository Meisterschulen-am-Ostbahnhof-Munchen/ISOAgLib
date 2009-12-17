/*
  elementddi_s.h: struct for defining ISO element/DDI pairs

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ELEMENTDDI_H
#define ELEMENTDDI_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "iprocesscmd_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  struct for defining ISO element/DDI pairs (with additional info about sepoint/measurement and value group (min, max, exact, default)
  @author Dipl.-Inform. Achim Spangler
*/
struct ElementDdi_s {
  uint16_t ui16_DDI;
  bool b_isSetpoint;
  ProcessCmd_c::ValueGroup_t en_valueGroup;
};


/** this typedef is only for some time to provide backward compatibility at API level */
typedef ElementDdi_s ElementDDI_s;


}
#endif
