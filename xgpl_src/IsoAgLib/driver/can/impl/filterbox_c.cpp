/***************************************************************************
                          filterbox_c.cpp  - FilterBox_c permits free definition
                                          of mask/filter for receiving CAN
                                          telegrams by a CANCustomer
                                          (or derived) object
                             -------------------
    begin                : Fri Jul 30 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#include "filterbox_c.h"
#include "canio_c.h"
#include <IsoAgLib/util/impl/cancustomer_c.h>

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
using namespace __IsoAgLib;

/**
  default constructor without parameter values for creating an instance
  with default start state (init variables)

   @exception badAlloc
*/
FilterBox_c::FilterBox_c()
  :
    c_filter(0, Ident_c::StandardIdent),
    c_mask(0, Ident_c::StandardIdent),
    c_additionalMask(~0, Ident_c::StandardIdent), // additional Mask set to "all-1s", so it defaults to "no additional mask".
    ui8_filterBoxNr(IdleState),
    ui8_busNumber(IdleState)
{ };

/**
  constructor with parameter values setting specific start state with
  setting pointer to the root CANIO_c and to the according CANCustomer
  instance; even define specific mask and filter setting

  @param rpc_customer  pointer to the CANCustomer_c instance, which creates this FilterBox_c instance
  @param rt_mask       mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param rt_filter     filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param ren_E         select if FilterBox_c is used for standard 11bit or extended 29bit ident
  @param rpc_filterBox optional parameter for getting to filterboxes connected together into the same MsgObj!
  @exception badAlloc
*/
FilterBox_c::FilterBox_c(CANCustomer_c* rrpc_customer,
                         MASK_TYPE rt_mask, MASK_TYPE rt_filter,
                         Ident_c::identType_t ren_identType, FilterBox_c* rpc_filterBox)
  : c_filter(rt_filter, ren_identType),
    c_mask(rt_mask, ren_identType),
    c_additionalMask( (rpc_filterBox == NULL) ? (~0) : (~(rpc_filterBox->c_filter.ident() ^ rt_filter)), ren_identType),
    vec_customer(1,rrpc_customer),
    ui8_filterBoxNr(rpc_filterBox->ui8_filterBoxNr),
    ui8_busNumber(rpc_filterBox->ui8_busNumber)
{
//  vec_customer.push_back(rrpc_customer);
}

/**
  copy constructor which uses data of another FilterBox_c instance

  @param rrefc_src reference to the source FilterBox_c instance for copying
   @exception badAlloc
*/
FilterBox_c::FilterBox_c(const FilterBox_c& rrefc_src)
  : c_filter(rrefc_src.c_filter),
    c_mask(rrefc_src.c_mask),
    c_additionalMask(rrefc_src.c_additionalMask),
    vec_customer(rrefc_src.vec_customer),
    ui8_filterBoxNr(rrefc_src.ui8_filterBoxNr),
    ui8_busNumber(rrefc_src.ui8_busNumber)
{};

/**
  destructor of this FilterBox_c instance
*/
FilterBox_c::~FilterBox_c()
{
  clearData();
}

/**
  copy values of rrefc_src FilterBox_c object to this instance

  possible errors:
      * badAlloc on not enough memory for copying puffed CAN msg from source

  @param rrefc_src FilterBox_c instance with data to assign to this instance
  @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
*/
FilterBox_c& FilterBox_c::operator=(const FilterBox_c& rrefc_src){
  if ( this != &rrefc_src)
  {
    // rrefc_src and self are different object instances
    vec_customer = rrefc_src.vec_customer;

    c_filter = rrefc_src.c_filter;
    c_mask = rrefc_src.c_mask;
    c_additionalMask = rrefc_src.c_additionalMask;
    ui8_busNumber = rrefc_src.ui8_busNumber;
    ui8_filterBoxNr = rrefc_src.ui8_filterBoxNr;
  }
  return *this;
}

/** clear the data of this instance */
void FilterBox_c::clearData()
{
  vec_customer.clear();
  c_mask.set(0, DEFAULT_IDENT_TYPE);
  c_filter.set(0, DEFAULT_IDENT_TYPE);
  c_filter.setEmpty(true);
  c_mask.setEmpty(true);
  c_additionalMask.set(~0, DEFAULT_IDENT_TYPE);
  ui8_busNumber = IdleState;
  ui8_filterBoxNr = IdleState;
};

/**
  configures the CAN hardware of given FilterBox (uses BIOS function)

  possible errors:
      * hwConfig used CAN BUS wans't configured properly
      * range given BUS or FilterBox number not in allowed area
      * hwBusy wanted FilterBox already in use
      * unspecified some other error
  @param rui8_busNumber BUS number, where this instance should act
  @param rui8_FilterBoxNr CAN hardware msg number for BIOS interaction
  @return true -> BIOS CAN object without errors configured
 */
bool FilterBox_c::configCan(uint8_t rui8_busNumber, uint8_t rui8_FilterBoxNr)
{
  // store ui8_busNumber for later close of can
  ui8_busNumber = rui8_busNumber;
  ui8_filterBoxNr = rui8_FilterBoxNr;

  #if ( ( defined( USE_DIN_9684 ) && defined( USE_ISO_11783 ) )  || ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) )
  // we have either compiled for DIN and ISO, OR there is at least one internal / proprietary CAN channel
  #ifdef USE_ISO_11783
  // when we are communicating on the standardized CAN channel, and ISO is used, the default shall be true
  b_performIsobusResolve = true;
  #else
  // when only DIN is used, or only proprietary protocol is used, the default shall be false
  b_performIsobusResolve = false;
  #endif
  #endif

  #if CAN_INSTANCE_CNT > PRT_INSTANCE_CNT
  // we have at least one internal/proprietary CAN channel --> check whether this FilterBox_c
  // was created for this internal (i.e. non ISOBUS) communication
  for ( unsigned int ind = PRT_INSTANCE_CNT; ind < CAN_INSTANCE_CNT; ind++ )
  {
    if ( getCanInstance( ind ).getBusNumber() == ui8_busNumber )
    { // this FilterBox_c has been created for processing of internal/proprietary messages
      b_performIsobusResolve = false;
      break;
    }
  }
  #endif // when amount of standardized prt instances is same as amount of CAN instances, no special check is needed

  #if defined( USE_DIN_9684 ) && defined( USE_ISO_11783 )
  if ( b_performIsobusResolve )
  { // check whether the message is DIN and not ISO
    if ( ( c_filter.identType() == Ident_c::StandardIdent                                                     )
      || ( ( c_filter.identType() == Ident_c::BothIdent ) && ( DEFAULT_IDENT_TYPE == Ident_c::StandardIdent ) ) )
    {
      b_performIsobusResolve = false;
    }
  }
  #endif

  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  if (c_filter.identType() == Ident_c::BothIdent) c_filter.setIdentType(DEFAULT_IDENT_TYPE);
  if (c_filter.empty() || c_mask.empty() ) return false;
  switch (HAL::can_configMsgobjInit(rui8_busNumber, rui8_FilterBoxNr, c_filter, c_mask, 0))
  {
    case HAL_NO_ERR:
      return true;
      break;
    case HAL_BUSY_ERR:
      /* this BIOS-Obj is already in use */
      getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::Can );
      break;
    case HAL_CONFIG_ERR:
      /* BUS not initialized, undefined msg type, CAN-BIOS memory error */
      #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "\r\nALARM Not enough memory for CAN buffer\r\n";
      #endif
      getLbsErrInstance().registerError( LibErr_c::HwConfig, LibErr_c::Can );
      break;
    case HAL_RANGE_ERR:
      /* undefined BUS number, undefined BIOS-Obj number, wrong puffer size */
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Can );
      break;
    default:
      /* unspecified error */
      getLbsErrInstance().registerError( LibErr_c::Unspecified, LibErr_c::Can );
      break;
  }
  #endif
  return false;
}

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
/**   close the BIOS filterbox object of this instance and close hardware CAN filterbox object */
void FilterBox_c::closeHAL()
{
  if ( ui8_busNumber != IdleState && ui8_filterBoxNr != IdleState )
  {
    if (HAL::can_configMsgobjClose(ui8_busNumber, ui8_filterBoxNr ) == HAL_RANGE_ERR)
    { // given BUS or filterBox number is wrong
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Can );
    }
    ui8_busNumber = ui8_filterBoxNr = IdleState;
  }
}
#endif

/* *************************************** */
/* ******* filter/mask managing ********** */
/* *************************************** */

/**
  set the mask (t_mask) and filter (t_filter) of this FilterBox
  @param rpc_customer pointer to the CANCustomer_c instance, which creates this FilterBox_c instance
  @param rt_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param rt_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param ren_E select if FilterBox_c is used for standard 11bit or extended 29bit ident
*/
void FilterBox_c::set(const Ident_c& rrefc_mask,
    const Ident_c& rrefc_filter, CANCustomer_c *rpc_customer, FilterBox_c* rpc_filterBox)
{
  c_filter = rrefc_filter;
  c_mask = rrefc_mask;
  if( !equalCustomer( *rpc_customer) )
    vec_customer.push_back(rpc_customer);

  if (rpc_filterBox == NULL)
    c_additionalMask.set (~0, c_mask.identType());
  else
    c_additionalMask.set (~(rpc_filterBox->c_filter.ident() ^ c_filter.ident()), c_mask.identType());
};

bool FilterBox_c::equalCustomer( const __IsoAgLib::CANCustomer_c& rref_customer ) const
{
  std::vector<CANCustomer_c*>::const_iterator pc_iter;
  for(pc_iter = vec_customer.begin(); pc_iter != vec_customer.end(); pc_iter++)
    if( &rref_customer == *pc_iter)
      return true;

  return false;
}

/** delete CANCustomer_c instance from array or set FilterBox_c to idle
    if CANCustomer_c is the only customer for this FilterBox_c instance
    @param  rref_customer  CANCustomer to delete
    @return                true -> no more cancustomers exist, whole filterbox can be deleted
  */
bool FilterBox_c::deleteFilter( const __IsoAgLib::CANCustomer_c& rref_customer)
{
  for (std::vector<CANCustomer_c*>::iterator pc_iter = vec_customer.begin();
        pc_iter != vec_customer.end(); pc_iter++)
  {
    if (&rref_customer == *pc_iter)
    { // the to-be-deleted customer is found and now pointed by pc_iter
      vec_customer.erase(pc_iter);
      break;
    }
  }
  if ( vec_customer.empty() )
  { // the last customer has been removed
    #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    closeHAL();
    //if the connection to the HAL is disconnected clear all data of the now idle filterbox
    clearData();
    #endif
    return true;
  }
  else
  { // there is at least one other can customer -> this FilterBox_c is still in use
    return false;
  }
}

/* ************************************************** */
/* ***** insert/get/process puffered CANPkg_c ******** */
/* ************************************************** */

/**
  control the processing of a received message
  (MsgObj_c::processMsg inserted data directly in CANCustomer
   -> FilterBox_c::processMsg() initiates conversion of CAN string
      to data flags and starts processing in CANCustomer_c)

  possible errors:
      * precondition no valid CANCustomer_c (or derived) is registered
  @return true -> FilterBox_c was able to inform registered CANCustomer
*/
bool FilterBox_c::processMsg()
{
  for (uint8_t i=0; i < vec_customer.size(); i++)
  {
    if (vec_customer[i] == NULL)
    { // pointer to CANCustomer_c wasn't set
      // -> don't know who wants to process the msg
      getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::Can );
      return false;
    }
    // ####################################################
    // from here on the vec_customer[i] has a valid pointer
    // ####################################################

    CANPkgExt_c* pc_target = &(vec_customer[i]->dataBase());
    #if defined SYSTEM_WITH_ENHANCED_CAN_HAL
      HAL::can_useMsgobjGet(ui8_busNumber, 0xFF, pc_target);
    #else
      HAL::can_useMsgobjGet(ui8_busNumber, ui8_filterBoxNr, pc_target);
    #endif

    // call customer's processMsg function, to let it
    // process the received CAN msg
    pc_target->string2Flags();

    #if ( ( defined( USE_ISO_11783 ) ) && ( defined( USE_DIN_9684 ) || ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) ) )
    if ( b_performIsobusResolve )
    #endif
    #ifdef USE_ISO_11783
    { // this block is only used for ISOBUS messages
      const MessageState_t cb_wasValidMsg = pc_target->resolveReceivingInformation();
      if ( cb_wasValidMsg == Invalid )
      {
        #ifdef PROCESS_INVALID_PACKETS
        if (vec_customer[i]->processInvalidMsg() )
        { // customer indicated, that it processed the content of the received message
          // --> do not show this message to any other FilterBox_c that might be connected to the same MsgObj_c
          return true;
        }
        #endif
      }
      else if ( ( cb_wasValidMsg == Valid ) || ( vec_customer[i]->isNetworkMgmt() ) )
      { // is either valid or OnlyNetworkMgmt with a CANCustomer which is of type NetworkMgmt
        if ( vec_customer[i]->processMsg() )
        { // customer indicated, that it processed the content of the received message
          //--> do not show this message to any other FilterBox_c that might be connected to the same MsgObj_c
          return true;
        }
      }
    }
    #endif // USE_ISO_11783
    #if ( ( defined( USE_ISO_11783 ) ) && ( defined( USE_DIN_9684 ) || ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) ) )
    else
    #endif
    #if ( defined( USE_DIN_9684 )  || ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) )
    {
      if ( vec_customer[i]->processMsg() )
      { // customer indicated, that it processed the content of the received message
        //--> do not show this message to any other FilterBox_c that might be connected to the same MsgObj_c
        return true;
      }
    }
    #endif
  }
  return false;
}

#ifdef DEBUG_CAN_BUFFER_FILLING
/** some debug messages */
void FilterBox_c::doDebug(uint8_t rui8_busNumber)
{
  bool b_detectedOverflow = false;
  #if CAN_INSTANCE_CNT == 1
  static uint16_t sui16_maxBufferUseage = 0;
  uint16_t &ref_maxCnt = sui16_maxBufferUseage;

  static uint16_t sui16_minBufferFree = 0xFFFF;
  uint16_t &ref_minFree = sui16_minBufferFree;
  #elif CAN_INSTANCE_CNT == 2
  static uint16_t sui16_maxBufferUseage[2] = {0, 0};
  uint16_t &ref_maxCnt = sui16_maxBufferUseage[rui8_busNumber];

  static uint16_t sui16_minBufferFree[2] = {0xFFFF,0xFFFF};
  uint16_t &ref_minFree = sui16_minBufferFree[rui8_busNumber];
  #else
  static uint16_t sui16_maxBufferUseage[CAN_INSTANCE_CNT];
  uint16_t &ref_maxCnt = sui16_maxBufferUseage[rui8_busNumber];

  static uint16_t sui16_minBufferFree[CAN_INSTANCE_CNT];
  uint16_t &ref_minFree = sui16_minBufferFree[rui8_busNumber];
  if ( ( sui16_maxBufferUseage[0] == 0 ) && ( sui16_maxBufferUseage[1] == 0 ) )
  {
    for ( uint16_t ind = 0; ind < CAN_INSTANCE_CNT; ind++) sui16_minBufferFree[ind] = 0xFFFF;
  }
  #endif
  if ( HAL::can_stateMsgobjBuffercnt(rui8_busNumber, ui8_filterBoxNr) > ref_maxCnt )
  { // new MAX detected -> update and print
    ref_maxCnt = HAL::can_stateMsgobjBuffercnt(rui8_busNumber, ui8_filterBoxNr);
    INTERNAL_DEBUG_DEVICE << "\r\nNew Max buffer filling: " << ref_maxCnt
      << " at Filterbox Nr: " << uint16_t(ui8_filterBoxNr)
      << " with Filter: " << c_filter.ident()
      << " at BUS: " << uint16_t(rui8_busNumber)
      << "\r\n";
  }
  if ( HAL::can_stateMsgobjFreecnt(rui8_busNumber, ui8_filterBoxNr) < ref_minFree )
  { // new MIN detected -> update and print
    ref_minFree = HAL::can_stateMsgobjFreecnt(rui8_busNumber, ui8_filterBoxNr);
    INTERNAL_DEBUG_DEVICE << "\r\nNew Min buffer free: " << ref_minFree
      << " at Filterbox Nr: " << uint16_t(ui8_filterBoxNr)
      << " with Filter: " << c_filter.ident()
      << " at BUS: " << uint16_t(rui8_busNumber)
      << "\r\n";
  }
}
#endif
