/*
 * ivtobjectworkingset_c.cpp
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */


#include "ivtobjectworkingset_c.h"

namespace IsoAgLib {

iVtObjectWorkingSet_c::iVtObjectWorkingSet_c(const iVtObjectWorkingSet_s *vtObjectWorkingSetSROM,
                                                       int ai_multitonInst)
        :__IsoAgLib::vtObjectWorkingSet_c(vtObjectWorkingSetSROM , ai_multitonInst)
{}


}
