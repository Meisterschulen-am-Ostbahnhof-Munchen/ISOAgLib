/*
  filterbox_c.h: Filter_Box_c permits free definition of mask/filter
    for receiving CAN telegrams by a CANCustomer (or derived) object

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef FILTER_BOX_H
#define FILTER_BOX_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/hal/hal_can.h>

#include "ident_c.h"
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/* *************************************** */
/* ********* class definition ************ */
/* *************************************** */
/** Virtual Msg Box where special Filter and Mask can be defined.
  CanIo_c handles free amount of instances of this class.
  Every MsgObj_c has 1 to m appointed instances of this class.
  Each FilterBox_c has exact one CanCustomer_c , which is sored on creation
  of a FilterBox_c instance, who registered for processing of received msg.
  @short FilterBox_c can use individual filter and mask
    for selecting received CAN telegrams and controls their processing.

  @author Dipl.-Inform. Achim Spangler
*/

typedef enum filterBoxState_en
{
  IdleState = 0xff,
  InactiveState = 0xfe,
  InvalidIdx = -1
} filterBoxState_t;

class FilterBox_c {
public:
  struct CustomerLen_s
  {

    CustomerLen_s (CanCustomer_c* apc_customer, int8_t ai8_dlcForce) : pc_customer(apc_customer), i8_dlcForce (ai8_dlcForce) {}


    CanCustomer_c * pc_customer;
    int8_t i8_dlcForce; // 0..8: DLC must exactly be 0..8.   < 0 (-1): DLC doesn't care! (default!)
  };

  /** default constructor without parameter values for creating an instance
    with default start state (init variables)

     @exception badAlloc
  */
  FilterBox_c();

  /** copy constructor which uses data of another FilterBox_c instance
    @param acrc_src reference to the source FilterBox_c instance for copying
     @exception badAlloc
  */
  FilterBox_c(const FilterBox_c& acrc_src);

  /** destructor of this FilterBox_c instance */
  ~FilterBox_c();

  /** copy values of acrc_src FilterBox_c object to this instance
    possible errors:
        * Err_c::badAlloc on not enough memory for copying puffed CAN msg from source

    @param acrc_src FilterBox_c instance with data to assign to this instance
    @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
  */
  FilterBox_c& operator=(const FilterBox_c& acrc_src);

  /** clear the data of this instance */
  void clearData();


  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** close the BIOS filterbox object of this instance and close hardware CAN filterbox object */
  void closeHAL();
  #endif
  /** check if this filterBox_c instance is really in use
      @return true -> filterBox is not in use
  */
  bool isIdle() {return (mui8_busNumber == IdleState && mui8_filterBoxNr == IdleState && mvec_customer.empty());}

  /** store new can customer with same filter and mask
      @param pc_cancustomer  new can customer
    */

   void insertCustomer(CanCustomer_c* pc_cancustomer, int8_t ai8_len) {mvec_customer.push_back(CustomerLen_s(pc_cancustomer, ai8_len));}


  /** configures the CAN hardware of given FilterBox (uses BIOS function with EXTENDED_HAL)

    possible errors:
        * hwConfig used CAN BUS wans't configured properly
        * range given BUS or FilterBox number not in allowed area
        * hwBusy wanted FilterBox already in use
        * unspecified some other error
    @param aui8_busNumber BUS number, where this instance should act
    @param aui8_FilterBoxNr CAN hardware msg number for BIOS interaction
    @return true -> BIOS CAN object without errors configured
  */
  bool configCan(uint8_t aui8_busNumber, uint8_t aui8_FilterBoxNr);

  /* *************************************** */
  /* ******* filter/mask managing ********** */
  /* *************************************** */

  /** set the mask (t_mask) and filter (t_filter) of this FilterBox
    @param acrc_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param acrc_filter filter for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
    @param apc_customer pointer to the CanCustomer_c instance, which creates this FilterBox_c instance
    @param ai8_dlcForce force the DLC to be exactly this long (0 to 8 bytes). use -1 for NO FORCING and accepting any length can-pkg>>>>>>> .r2856
  */
   void set (const Ident_c& acrc_mask,
            const Ident_c& acrc_filter,
            CanCustomer_c *apc_customer = NULL,
            int8_t ai8_dlcForce = -1);


  /** check if ID from a CAN msg matches this FilterBox
    @param at_ident CAN ident of received msg
    @return true -> CAN ident fits to local filter/mask definition
  */
  inline bool matchMsgId(MASK_TYPE at_ident, Ident_c::identType_t at_type )
  { return ( (mc_mask.masked(at_ident) == mc_mask.masked(mc_filter)) && (at_type == identType()) );}

  /** checks, if FilterBox_c definition given by ac_mask and ac_filter is the same
    @param ac_mask mask to use for comparison
    @param ac_filter filter to use for comparison
    @return true -> given mask and filter are same as the local defs
  */
  bool equalFilterMask(const Ident_c& ac_mask, const Ident_c& ac_filter) const
    {return ((mc_mask == ac_mask) && (mc_filter == ac_filter));}

  /** checks, if Filter_Box_c has already stored given customer
      @param ar_customer  customer to compare
      @return               true -> customer already stored
    */
      bool equalCustomer( const __IsoAgLib::CanCustomer_c& ar_customer ) const;


  /** delete CanCustomer_c  instance from array
      @param  ar_customer  CANCustomer to delete
      @return                true -> no more cancustomers exist, whole filterbox can be deleted
    */
  bool deleteFilter(const __IsoAgLib::CanCustomer_c & ar_customer);

  /** deliver the type of the FilterBox_c ident */
  Ident_c::identType_t identType() const {return mc_filter.identType();}

  /** deliver const reference to mask Ident
    @return const reference to mask Ident_c instance
  */
  const Ident_c& mask() const {return mc_mask;}

  /** deliver const reference to filter Ident
    @return const reference to filter Ident_c instance
  */
  const Ident_c& filter() const {return mc_filter;}

  #if DEBUG_CAN_BUFFER_FILLING
  /** some debug messages */
  void doDebug(uint8_t aui8_busNumber);
  #endif

  /* ************************************************** */
  /* ***** insert/get/process buffered CanPkg_c ******* */
  /* ************************************************** */

  /** control the processing of a received message
    (MsgObj_c::processMsg inserted data directly in CANCustomer
     -> FilterBox_c::processMsg() initiates conversion of CAN string
        to data flags and starts processing in CanCustomer_c )

    possible errors:
        * Err_c::precondition no valid CanCustomer_c  (or derived) is registered
    @return true -> FilterBox_c was able to inform registered CANCustomer
  */
  bool processMsg();


  int32_t getFbVecIdx(){return mi32_fbVecIdx;};
  void setFbVecIdx(int32_t ri32_fbIdx){mi32_fbVecIdx = ri32_fbIdx;};

private:
// Private attributes
  /** mc_filter for this FilterBox_c insance */
  Ident_c mc_filter;

  /** mc_mask for this FilterBox_c instance */
  Ident_c mc_mask;

  /**vector of pointer to pc_customer CanCustomer_c  which works with the received CAN data */
  STL_NAMESPACE::vector<CustomerLen_s> mvec_customer;

  /** number of message object */
  uint8_t mui8_filterBoxNr; //use like ui8_msgObjNr from msgobj_c class

  /** BUS Number for systems with more than one BUS */
  uint8_t mui8_busNumber;

  int32_t mi32_fbVecIdx;

  /** Indicate if FilterBox is iterating through the customers in "processMsg" */
  static FilterBox_c* mspc_currentlyProcessedFilterBox;
  static int msi_processMsgLoopIndex; /// "< 0" indicates Loop ==> need to adapt at delete operations
  static int msi_processMsgLoopSize;  /// used if in Loop mode, need to be adapted at remove/add, too.

#if ((defined(USE_ISO_11783)) && (CAN_INSTANCE_CNT > PRT_INSTANCE_CNT))
  /** we have either compiled for ISO, OR there is at least one internal / proprietary CAN channel */
  bool mb_performIsobusResolve;
#endif
};
}
#endif
