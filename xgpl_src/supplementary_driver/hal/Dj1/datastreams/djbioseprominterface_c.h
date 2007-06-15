//
// C++ Interface: djbioseprominterface_c
//
// Description: 
//
//
// Author: Sergej Stefan Goldynski , (C) 2007
// mail S.goldynski@osb-ag.de
// mail <stefan.goldynski@web.de>
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef DJBIOSEPROM_INTERFACE_DEFINED
#define DJBIOSEPROM_INTERFACE_DEFINED



#include "djbiostypedefs.h"

class DjBiosEpromInterface_c
{
  public:

   static DjBool_t EofIop ( void const *Handle ) { return __HAL::DjBios_IOP_EoF ( Handle ); }

   static void GetIopName ( char *Filename ) { __HAL::DjBios_IOP_GetName ( Filename ); }

   static uint16_t WriteIop ( void const *Handle, uword Size, uword NumObj, void const *Data )
      { return __HAL::DjBios_IOP_Write ( Handle, Size, NumObj, Data ); }

   static uint16_t ReadIop ( void const *Handle, uword Size, uword NumObj, void *Data )
      { return __HAL::DjBios_IOP_Read ( Handle, Size, NumObj, Data ); }

   static DjBool_t CloseIop ( void const *Handle ) { return __HAL::DjBios_IOP_Close ( Handle ); }

   static void* OpenIop ( char const *Filename, char const *Mode )
      {  return __HAL::DjBios_IOP_Open ( Filename, Mode ); }

};

#endif
