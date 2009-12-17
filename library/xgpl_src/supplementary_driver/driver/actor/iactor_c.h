/*
  iactor.h:
    header file for iActorO_c, a central object for enum types used in
    all actor objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IACTOR_H
#define IACTOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/actor.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**class for all enum types in relation to Actor_O
  @see iActor_O
  @see iDigital_O
  @author Dipl.-Inform. Achim Spangler
  */
class iActor_c : public __IsoAgLib::Actor_c {
public:
  /** constructor with no function, because this object is only needed for its enum types */
  iActor_c() : Actor_c(){};
  /** destructor with no function */
  ~iActor_c() {};
};

}
#endif
