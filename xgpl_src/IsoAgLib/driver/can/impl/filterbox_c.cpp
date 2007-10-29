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

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE) || defined(DEBUG_CAN_BUFFER_FILLING)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

//#include <IsoAgLib/hal/generic_utils/can/canfifo_c.h>
#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>

namespace __IsoAgLib {

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
    ui8_busNumber(IdleState),
    i32_fbVecIdx(-1)
#if ((defined( USE_ISO_11783)) \
     && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
    , b_performIsobusResolve(true)
  #endif
{}

/**
  constructor with parameter values setting specific start state with
  setting pointer to the root CanIo_c and to the according CANCustomer
  instance; even define specific mask and filter setting

  @param apc_customer  pointer to the CanCustomer_c instance, which creates this FilterBox_c instance
  @param at_mask       mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param at_filter     filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)


  @param ren_E         select if FilterBox_c is used for standard 11bit or extended 29bit ident
  @param apc_filterBox optional parameter for getting to filterboxes connected together into the same MsgObj!
  @exception badAlloc
FilterBox_c::FilterBox_c(CanCustomer_c * rrpc_customer,
                         MASK_TYPE at_mask, MASK_TYPE at_filter,
                         Ident_c::identType_t ren_identType, FilterBox_c* apc_filterBox)
  : c_filter(at_filter, ren_identType),
    c_mask(at_mask, ren_identType),
    c_additionalMask( (apc_filterBox == NULL) ? (~0) : (~(apc_filterBox->c_filter.ident() ^ at_filter)), ren_identType),
    vec_customer(1,rrpc_customer),
    ui8_filterBoxNr( (apc_filterBox == NULL) ? (IdleState) : (apc_filterBox->ui8_filterBoxNr) ),
    ui8_busNumber( (apc_filterBox == NULL) ? (IdleState) : (apc_filterBox->ui8_busNumber) )
#if ((defined( USE_ISO_11783)) \
     && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
    , b_performIsobusResolve(apc_filterBox->b_performIsobusResolve)
  #endif
{}
*/

/**
  copy constructor which uses data of another FilterBox_c instance

  @param arc_src reference to the source FilterBox_c instance for copying
   @exception badAlloc
*/
FilterBox_c::FilterBox_c(const FilterBox_c& arc_src)
  : c_filter(arc_src.c_filter),
    c_mask(arc_src.c_mask),
    c_additionalMask(arc_src.c_additionalMask),
    vec_customer(arc_src.vec_customer),
    ui8_filterBoxNr(arc_src.ui8_filterBoxNr),
    ui8_busNumber(arc_src.ui8_busNumber),
    i32_fbVecIdx(arc_src.i32_fbVecIdx)
#if ((defined( USE_ISO_11783)) \
		    && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
    , b_performIsobusResolve(arc_src.b_performIsobusResolve)
  #endif
{}

/**
  destructor of this FilterBox_c instance
*/
FilterBox_c::~FilterBox_c()
{
  clearData();
}

/**
  copy values of arc_src FilterBox_c object to this instance

  possible errors:
      * badAlloc on not enough memory for copying puffed CAN msg from source

  @param arc_src FilterBox_c instance with data to assign to this instance
  @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
*/

FilterBox_c& FilterBox_c::operator=(const FilterBox_c& arc_src){
  if ( this != &arc_src)
  {
    // arc_src and self are different object instances
    vec_customer = arc_src.vec_customer;

    c_filter = arc_src.c_filter;
    c_mask = arc_src.c_mask;
    c_additionalMask = arc_src.c_additionalMask;
    ui8_busNumber = arc_src.ui8_busNumber;
    ui8_filterBoxNr = arc_src.ui8_filterBoxNr;
    i32_fbVecIdx = arc_src.i32_fbVecIdx;
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
  i32_fbVecIdx = InvalidIdx;
}

/**
  configures the CAN hardware of given FilterBox (uses BIOS function)

  possible errors:
      * hwConfig used CAN BUS wans't configured properly
      * range given BUS or FilterBox number not in allowed area
      * hwBusy wanted FilterBox already in use
      * unspecified some other error
  @param aui8_busNumber BUS number, where this instance should act
  @param aui8_FilterBoxNr CAN hardware msg number for BIOS interaction
  @return true -> BIOS CAN object without errors configured
 */
bool FilterBox_c::configCan(uint8_t aui8_busNumber, uint8_t aui8_FilterBoxNr)
{
  // store ui8_busNumber for later close of can
  ui8_busNumber = aui8_busNumber;
  ui8_filterBoxNr = aui8_FilterBoxNr;

#if ((defined(USE_ISO_11783)) \
     && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
  // we have either compiled forISO, OR there is at least one internal / proprietary CAN channel

  // when we are communicating on the standardized CAN channel, and ISO is used, the default shall be true
  b_performIsobusResolve = true;

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

  #ifdef ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
  for ( STL_NAMESPACE::vector<CustomerLen_s>::const_iterator iter = vec_customer.begin(); iter != vec_customer.end(); iter++ )
  {
    if ( (*iter).pc_customer->isProprietaryMessageOnStandardizedCan() )
    { // at least one CanCustomer_c  uses a proprietary protocol at a normal ISOBUS CanIo_c instance
      b_performIsobusResolve = false;
    }
    else
    { // this FilterBox_c has at least one CanCustomer_c  which performs real ISOBUS
      // --> set b_performIsobusResolve to default TRUE
      b_performIsobusResolve = true;
      break;
    }
  }
  #endif

  #endif // end of #ifdef for usage of b_performIsobusResolve

  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  if (c_filter.empty() || c_mask.empty() ) return false;
  switch (HAL::can_configMsgobjInit(aui8_busNumber, aui8_FilterBoxNr, c_filter, c_mask, 0))
  {
    case HAL_NO_ERR:
      return true;
      break;
    case HAL_BUSY_ERR:
      /* this BIOS-Obj is already in use */
      getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::Can );
      break;
    case HAL_CONFIG_ERR:
      /* BUS not initialized, undefined msg type, CAN-BIOS memory error */
      #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "\r\nALARM Not enough memory for CAN buffer" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
      break;
    case HAL_RANGE_ERR:
      /* undefined BUS number, undefined BIOS-Obj number, wrong puffer size */
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
    default:
      /* unspecified error */
      getILibErrInstance().registerError( iLibErr_c::Unspecified, iLibErr_c::Can );
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
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
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
 @param at_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param at_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param apc_customer pointer to the CanCustomer_c instance, which creates this FilterBox_c instance
  @param ai8_dlcForce force the DLC to be exactly this long (0 to 8 bytes). use -1 for NO FORCING and accepting any length can-pkg


  @param ren_E select if FilterBox_c is used for standard 11bit or extended 29bit ident
*/
void FilterBox_c::set (const Ident_c& arc_mask,
                       const Ident_c& arc_filter,
                       CanCustomer_c* apc_customer,
                       int8_t ai8_dlcForce,
                       FilterBox_c* apc_filterBox)
{
  c_filter = arc_filter;
  c_mask = arc_mask;

  STL_NAMESPACE::vector<CustomerLen_s>::iterator pc_iter = vec_customer.begin();
  for (; pc_iter != vec_customer.end(); pc_iter++)
  {
    if (apc_customer == pc_iter->pc_customer)
    { // overwrite the DLC of the one found!
      pc_iter->i8_dlcForce = ai8_dlcForce;
      break;
    }
  }
  if (pc_iter == vec_customer.end())
  { // push back new
    vec_customer.push_back (CustomerLen_s (apc_customer, ai8_dlcForce));
  }

  if (apc_filterBox == NULL)
       c_additionalMask.set (~0, c_mask.identType());
  else c_additionalMask.set (~(apc_filterBox->c_filter.ident() ^ c_filter.ident()), c_mask.identType());
};

bool FilterBox_c::equalCustomer( const __IsoAgLib::CanCustomer_c& ar_customer ) const

{
  STL_NAMESPACE::vector<CustomerLen_s>::const_iterator pc_iter;
  for(pc_iter = vec_customer.begin(); pc_iter != vec_customer.end(); pc_iter++)
    if( &ar_customer == pc_iter->pc_customer)
      return true;

  return false;
}

/** delete CanCustomer_c  instance from array or set FilterBox_c to idle
    if CanCustomer_c  is the only customer for this FilterBox_c instance
    @param  ar_customer  CANCustomer to delete
    @return                true -> no more cancustomers exist, whole filterbox can be deleted
  */
bool FilterBox_c::deleteFilter( const __IsoAgLib::CanCustomer_c& ar_customer)

{
  for (STL_NAMESPACE::vector<CustomerLen_s>::iterator pc_iter = vec_customer.begin();
        pc_iter != vec_customer.end(); pc_iter++)
  {
    if (&ar_customer == pc_iter->pc_customer)
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
    #endif

    clearData();

    return true;
  }
  else
  { // there is at least one other can customer -> this FilterBox_c is still in use
    return false;
  }
}

/* ************************************************** */
/* ***** insert/get/process puffered CanPkg_c ******** */
/* ************************************************** */

/**
  control the processing of a received message
  (MsgObj_c::processMsg inserted data directly in CANCustomer
   -> FilterBox_c::processMsg() initiates conversion of CAN string
      to data flags and starts processing in CanCustomer_c )

  possible errors:
      * precondition no valid CanCustomer_c  (or derived) is registered
  @return true -> FilterBox_c was able to inform registered CANCustomer
*/
bool FilterBox_c::processMsg()
{
  for ( STL_NAMESPACE::vector<CustomerLen_s>::iterator c_customerIterator = vec_customer.begin(); c_customerIterator != vec_customer.end(); c_customerIterator++ )
  {
    if (c_customerIterator->pc_customer == NULL)
    { // pointer to CanCustomer_c  wasn't set
      // -> don't know who wants to process the msg
      getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::Can );
      return false;
    }
    // ####################################################
    // from here on the c_customerIterator has a valid pointer
    // ####################################################

    CanPkgExt_c* pc_target = &(c_customerIterator->pc_customer->dataBase());
    #if defined SYSTEM_WITH_ENHANCED_CAN_HAL
      HAL::can_useMsgobjGet(ui8_busNumber, 0xFF, pc_target);
    #else

      bool b_fifoRet = HAL::fifo_useMsgObjGet(ui8_busNumber, pc_target);
      if(b_fifoRet != HAL_NO_ERR)
      {
      #ifdef DEBUG
         INTERNAL_DEBUG_DEVICE
        << "Central Fifo - Reading problem on bus : " << int(ui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
        IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::CanWarn, IsoAgLib::iLibErr_c::Can );
        return false;
      }
  #ifdef DEBUG
         INTERNAL_DEBUG_DEVICE
        << "FilterBox is consuming the message " << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif

    #endif

    const int8_t ci8_vecCurstomerDlcForce = c_customerIterator->i8_dlcForce;
    const int8_t ci8_targetLen = pc_target->getLen();

    /// Check DataLengthCode (DLC) if required
    if ((ci8_vecCurstomerDlcForce < 0) || (ci8_vecCurstomerDlcForce == ci8_targetLen))
    { // either no dlc-check requested or dlc matches the check!
      pc_target->t_msgState = DlcValid;
    }
    else
    { // dlc-check was requested but failed
      pc_target->t_msgState = (ci8_targetLen < ci8_vecCurstomerDlcForce) ? DlcInvalidTooShort : DlcInvalidTooLong;
    }

    #ifdef USE_ISO_11783
    // this is a compile where at least one CAN channel uses ISO 11783 protocol (i.e. this is not a pure proprietary CAN project)
      #if ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL))
      // there is either a proprietary CAN channel in addition to the ISO 11783 channel, or some proprietary messages
      // have to be handled at the ISO 11783 BUS (e.g. some bootloader messages are sent on ISOBUS)
        #ifdef ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
        // we have to decide based on the individual CANCustomer, whether it is a real ISOBUS message
    if ( ( b_performIsobusResolve ) && ( !c_customerIterator->pc_customer->isProprietaryMessageOnStandardizedCan() ) )
        #else
        // the decision whether a FilterBox_c has to perform SA resolve is derived for _all_ messages which are routed through this FilterBox_c object
    if ( b_performIsobusResolve )
        #endif // end ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
      #endif // end combination check of whether the flag based decision on resolving has to be performed
    { // this block is only used for ISOBUS messages
      // add address-resolving result to dlc-check result!
      pc_target->t_msgState = static_cast<MessageState_t>(pc_target->t_msgState | pc_target->resolveReceivingInformation());

      // call customer's processMsg function, to let it
      // process the received CAN msg
      pc_target->string2Flags();

      if ( ((pc_target->t_msgState & AdrResolveMask) == AdrInvalid)
        || ((pc_target->t_msgState & DlcValidationMask) != DlcValid)
         )
      {
        #ifdef PROCESS_INVALID_PACKETS
        if (c_customerIterator->pc_customer->processInvalidMsg() )
        { // customer indicated, that it processed the content of the received message
          // --> do not show this message to any other FilterBox_c that might be connected to the same MsgObj_c
          return true;
        }
        #endif
      }
      else if ( ((pc_target->t_msgState & AdrResolveMask) == AdrValid) || (c_customerIterator->pc_customer->isNetworkMgmt()) )
      { // is either valid or OnlyNetworkMgmt with a CANCustomer which is of type NetworkMgmt
        if ( c_customerIterator->pc_customer->processMsg() )
        { // customer indicated, that it processed the content of the received message
          //--> do not show this message to any other FilterBox_c that might be connected to the same MsgObj_c
          return true;
        }
      }
    }
      #if ( ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL) )
      // the project uses at least on one of the CAN BUSes ISO 11783 OR proprietary messages are handled at least on one CAN channel
    else
      #endif
    #endif // end of #ifdef USE_ISO_11783
    #if ( ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) \
     ||  defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL) \
     || !defined(USE_ISO_11783) )
     // either this project does NOT use ISO 11783 at all,
     // OR there is a proprietary message CAN channel - and this FilterBox_c object is dedicated to the proprietary CAN channel (by being at "else" block)
     // OR there can be a mixture of ISO 11783 and proprietary messages on one channel - and the currently handled message is not ISO 11783 (by being at "else" block)
    {
      // call customer's processMsg function, to let it
      // process the received CAN msg
      pc_target->string2Flags();

      if ((pc_target->t_msgState & DlcValidationMask) != DlcValid)
      {
        #ifdef PROCESS_INVALID_PACKETS
        if (c_customerIterator->pc_customer->processInvalidMsg() )
        { // customer indicated, that it processed the content of the received message
          // --> do not show this message to any other FilterBox_c that might be connected to the same MsgObj_c
          return true;
        }
        #endif
      }
      else
      {
        if ( c_customerIterator->pc_customer->processMsg() )
        { // customer indicated, that it processed the content of the received message
          //--> do not show this message to any other FilterBox_c that might be connected to the same MsgObj_c
          return true;
        }
      }
    }
    #endif
  }
  return false;
}

#ifdef DEBUG_CAN_BUFFER_FILLING
/** some debug messages */
void FilterBox_c::doDebug(uint8_t aui8_busNumber)
{
  bool b_detectedOverflow = false;
  #if CAN_INSTANCE_CNT == 1
  static uint16_t sui16_maxBufferUseage = 0;
  uint16_t &r_maxCnt = sui16_maxBufferUseage;

  static uint16_t sui16_minBufferFree = 0xFFFF;
  uint16_t &r_minFree = sui16_minBufferFree;
  #elif CAN_INSTANCE_CNT == 2
  static uint16_t sui16_maxBufferUseage[2] = {0, 0};
  uint16_t &r_maxCnt = sui16_maxBufferUseage[aui8_busNumber];

  static uint16_t sui16_minBufferFree[2] = {0xFFFF,0xFFFF};
  uint16_t &r_minFree = sui16_minBufferFree[aui8_busNumber];
  #else
  static uint16_t sui16_maxBufferUseage[CAN_INSTANCE_CNT];
  uint16_t &r_maxCnt = sui16_maxBufferUseage[aui8_busNumber];

  static uint16_t sui16_minBufferFree[CAN_INSTANCE_CNT];
  uint16_t &r_minFree = sui16_minBufferFree[aui8_busNumber];
  if ( ( sui16_maxBufferUseage[0] == 0 ) && ( sui16_maxBufferUseage[1] == 0 ) )
  {
    for ( uint16_t ind = 0; ind < CAN_INSTANCE_CNT; ind++) sui16_minBufferFree[ind] = 0xFFFF;
  }
  #endif
  if ( HAL::can_stateMsgobjBuffercnt(aui8_busNumber, ui8_filterBoxNr) > r_maxCnt )
  { // new MAX detected -> update and print
    r_maxCnt = HAL::can_stateMsgobjBuffercnt(aui8_busNumber, ui8_filterBoxNr);
    INTERNAL_DEBUG_DEVICE << "\r\nNew Max buffer filling: " << r_maxCnt
      << " at Filterbox Nr: " << uint16_t(ui8_filterBoxNr)
      << " with Filter: " << c_filter.ident()
      << " at BUS: " << uint16_t(aui8_busNumber)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  if ( HAL::can_stateMsgobjFreecnt(aui8_busNumber, ui8_filterBoxNr) < r_minFree )
  { // new MIN detected -> update and print
    r_minFree = HAL::can_stateMsgobjFreecnt(aui8_busNumber, ui8_filterBoxNr);
    INTERNAL_DEBUG_DEVICE << "\r\nNew Min buffer free: " << r_minFree
      << " at Filterbox Nr: " << uint16_t(ui8_filterBoxNr)
      << " with Filter: " << c_filter.ident()
      << " at BUS: " << uint16_t(aui8_busNumber)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
}
#endif

} // End namespace __IsoAgLib
