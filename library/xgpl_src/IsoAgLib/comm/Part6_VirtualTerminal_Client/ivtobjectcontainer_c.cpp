/*
 * ivtobjectcontainer_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectcontainer_c.h"


#ifdef CONFIG_USE_VTOBJECT_container


namespace IsoAgLib {


  // implement here a normal constructor/destructor, as the compiler dislikes inlining of that simple
  // constructor/destructor direct in scope of iVtObjectContainer_c
  iVtObjectContainer_c::~iVtObjectContainer_c() {}


} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_container
