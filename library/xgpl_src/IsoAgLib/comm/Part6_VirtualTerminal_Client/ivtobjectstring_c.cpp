/*
 * ivtobjectstring_c.cpp
 *
 *  Created on: 04.02.2022
 *      Author: franz
 */

#include "ivtobjectstring_c.h"

namespace IsoAgLib {


  // implement here a normal constructor/destructor, as the compiler dislikes inlining of that simple
  // constructor/destructor direct in scope of iVtObjectString_c
  iVtObjectString_c::iVtObjectString_c(int ai_multitonInst)
  :vtObject_c(ai_multitonInst)
  {}




} // IsoAgLib


