/***************************************************************************
                             dinmaskupload_c.h - upload of terminal mask
                                                according to specification
                                                 of Fieldstar and LBS+
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef DIN_MASKUPLOAD_H
#define DIN_MASKUPLOAD_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../dinmaskuploadstructures_t.h"
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#include <IsoAgLib/comm/Process/elementddi_s.h>
#include <IsoAgLib/comm/Process/Local/SimpleMeasureSetpoint/impl/procdatalocalsimplesetpointsimplemeasure_c.h>


#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC)
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif

#include <functional>


namespace IsoAgLib { class iDINMaskUpload_c;};

// Begin Namespace IsoAgLibMaskUpload
namespace __IsoAgLib {

/** enum for the state of mask upload */
enum maskUploadState_t { none = 0, await_start = 1, running = 2, success = 3, abort = 4};


/**
  This class manages the upload of terminal masks according to
  the specification of Fieldstar an LBS+. Mask definitions can be
  simply registered. As soon as one of the registered terminals
  is detected on the BUS, DINMaskUpload_c creates the needed
  process data values for synchronisation. After receiving a
  upload command, DINMaskUpload_c commands MultiSend_c to
  send the data stream of the mask to the terminal.

  @author Dipl.-Inform. Achim Spangler
*/
class DINMaskUpload_c : public SINGLETON_DERIVED(DINMaskUpload_c, ElementBase_c)
{
private:
  typedef ProcDataLocalSimpleSetpointSimpleMeasure_c syncproc_t;
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::slist<syncproc_t,STL_NAMESPACE::__malloc_alloc_template<0> > Vec_SyncProc;
  #else
  typedef STL_NAMESPACE::slist<syncproc_t> Vec_SyncProc;
  #endif
protected:
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::slist<syncproc_t,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator Vec_SyncProcIterator;
  #else
  typedef STL_NAMESPACE::slist<syncproc_t>::iterator Vec_SyncProcIterator;
  #endif
public:
  /** initialisation for DINMaskUpload_c
  */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );
  /** default destructor which has nothing to do */
  virtual ~DINMaskUpload_c();

  /**
    deliver reference to data pkg
    @return reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  MultiSendPkg_c& data(){return *pc_data;};
  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  CANPkgExt_c& dataBase();

  /**
    start processing of a process msg
    ignore all invalid messages where SEND is not of a member with claimed address,
    or where EMPF isn't valid

    possible errors:
      * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  */
  bool processMsg();

  /**
    perform periodical actions
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );
  /**
    register a terminal LBS+ for potential mask upload
    @param rpsMask pointer to mask identifying structure
    @return true -> mask was registrated for upload
  */
  bool registerLbsPlusMask(IsoAgLib::t_maskDefinition* rpsMask);
  /**
    check if mask upload is running
    @return true -> mask is being uploaded at the moment
  */
  bool isRunning()const{return (en_maskUploadState == running)?true:false;};
  /**
    check if mask upload is finished with success
    @return true -> mask upload is finished with success
  */
  bool isSuccess()const{return (en_maskUploadState == success)?true:false;};
  /**
    check if mask upload is aborted
    @return true -> mask upload is aborted
  */
  bool isAborted()const{return (en_maskUploadState == abort)?true:false;};
private: // Private methods
  friend class SINGLETON_DERIVED(DINMaskUpload_c, ElementBase_c);
  friend class IsoAgLib::iDINMaskUpload_c;
  /**
    HIDDEN constructor for a DINMaskUpload_c object instance
    NEVER instantiate a variable of type DINMaskUpload_c within application
    only access DINMaskUpload_c via getDinMaskuploadInstance() or getDinMaskuploadInstance( int riLbsBusNr )
    in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  DINMaskUpload_c() {};
  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit() { init(); };
  /**
    create the syncronisation process data for IsoAgLib+ mask upload
  */
  void createLbsplusProcdata();
  /**
    create the syncronisation process data for oldstyle fieldstar mask upload
  */
  void createFieldstarProcdata();
  /**
    deliver a pointer to the active mask definition
    @return pointer to active mask
  */
  const IsoAgLib::t_maskDefinition& activeMask()const{return *(psMaskDef[ui8_maskDefInd]);};
  /**
    in some cases the Varioterminal doesn't detect the implement.
    Retrigger send of sync data every 500 msec.
  */
  void tryResendMaskSyncData();
private: // Private attributes
  /** msg object for CAN I/O */
  MultiSendPkg_c* pc_data;

  /** pointer to the MemberITem of the temrinal */
  const DINItem_c* pc_terminal;

  /** array of pointer to Mask Definitions */
  IsoAgLib::t_maskDefinition* psMaskDef[REGISTRABLE_MASK_CNT];
  /** list of local process data for syncronisation */
  Vec_SyncProc arrSyncproc;
  /** actual state of mask upload */
  maskUploadState_t en_maskUploadState;

  MultiSend_c::sendSuccess_t en_sendSuccess;

  /** DEV_KEY of the connected terminal */
  DevKey_c c_devKey;
  /** active entry no in psMaskDef */
  uint8_t ui8_maskDefInd;
  /** amount of registered masks */
  uint8_t ui8_maskDefCnt;
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINMaskUpload_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  DINMaskUpload_c& getDinMaskuploadInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  DINMaskUpload_c& getDinMaskuploadInstance( void );
#endif

}
#endif
