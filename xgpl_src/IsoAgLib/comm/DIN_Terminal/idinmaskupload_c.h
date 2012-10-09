/***************************************************************************
                             idinmaskupload_c.h - upload of terminal mask
                                                 according to specification
                                                 of Fieldstar and LBS+
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IDIN_MASKUPLOAD_H
#define IDIN_MASKUPLOAD_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
/** include structure definition for IsoAgLib+ terminal property definition */
#include "dinmaskuploadstructures_t.h"
#include "impl/dinmaskupload_c.h"

// Begin Namespace IsoAgLibMaskUpload
namespace IsoAgLib {

/**
  This class manages the upload of terminal masks according to
  the specification of Fieldstar an LBS+. Mask definitions can be
  simply registered. As soon as one of the registered terminals
  is detected on the BUS, iDINMaskUpload_c creates the needed
  process data values for synchronisation. After receiving a
  upload command, iDINMaskUpload_c commands iMultiSend_c to
  send the data stream of the mask to the terminal.

  @author Dipl.-Inform. Achim Spangler
*/
class iDINMaskUpload_c : private __IsoAgLib::DINMaskUpload_c {
 public:
  /** initialisation for DINMaskUpload_c
  */
  void init( void ) { DINMaskUpload_c::init(); } ;
  /**
    register a terminal LBS+ for potential mask upload
    @param rpsMask pointer to mask identifying structure
    @return true -> mask was registrated for upload
  */
  bool registerLbsPlusMask(t_maskDefinition* rpsMask)
    {return DINMaskUpload_c::registerLbsPlusMask(rpsMask);};
  /**
    check if mask upload is running
    @return true -> mask is being uploaded at the moment
  */
  bool isRunning() const
    {return DINMaskUpload_c::isRunning();};
  /**
    check if mask upload is finished with success
    @return true -> mask upload is finished with success
  */
  bool isSuccess()const
    {return DINMaskUpload_c::isSuccess();}
  /**
    check if mask upload is aborted
    @return true -> mask upload is aborted
  */
  bool isAborted()const
    {return DINMaskUpload_c::isAborted();}
 private:
  /** allow getIdinMaskuploadInstance() access to shielded base class.
      otherwise __IsoAgLib::getDinMaskuploadInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iDINMaskUpload_c& getIdinMaskuploadInstance( uint8_t rui8_instance );
  #else
  friend iDINMaskUpload_c& getIdinMaskuploadInstance( void );
  #endif
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINMaskUploadC_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iDINMaskUpload_c& getIdinMaskuploadInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iDINMaskUpload_c&>(__IsoAgLib::getDinMaskuploadInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique DINMaskUploadC_c singleton instance */
  inline iDINMaskUpload_c& getIdinMaskuploadInstance( void )
  { return static_cast<iDINMaskUpload_c&>(__IsoAgLib::getDinMaskuploadInstance());};
#endif

}
#endif
