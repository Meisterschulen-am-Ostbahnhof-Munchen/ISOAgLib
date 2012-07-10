/*
  msgobj_c.h: MsgObj_c encapsulate a hardware CAN Msg Object, to
    present a standard interaction with CAN for all platforms

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MSG_OBJ_H
#define MSG_OBJ_H

#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/hal/hal_can.h>


#include <list>

#include "filterbox_c.h"
#include "ident_c.h"




#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/* *************************************** */
/* ********* class definition ************ */
/* *************************************** */
/** Each instances of this class handles one hardware CAN message object.
  While configuring the amount of instances can be higher than the dedicated.
  But before running the CAN system, the amount has to be shrinked by
  merging the instances with the smallest difference in filter bits.
  @short Encapsulate a BIOS MsgObj with forwarding received CAN msg to
    registered dedicated FilterBox_c instance.
  @author Dipl.-Inform. Achim Spangler
*/
class MsgObj_c {
private:


  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::vector<FilterBox_c,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator FilterRef;
  #else
  typedef STL_NAMESPACE::vector<FilterBox_c>::iterator FilterRef;
  #endif


public:
  /** default constructor for MsgObj_c which only init all member values defined start state */
  MsgObj_c();

  /** copy constructor for this class, which gets data from another MsgObj_c instance
    @param acrc_src source MsgObj_c instance which should be cloned by this instance
  */
  MsgObj_c(const MsgObj_c& acrc_src);

  /** destructor of MsgObj_c instance, which closes the hardware MsgObj_c */
  ~MsgObj_c();

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION

  void printMyFilterBox();

#endif

  //+++++++++++++++++++++++++++++++++++
  // help functions for work on MsgObj_c
  //+++++++++++++++++++++++++++++++++++
  /** merge two msgObj instances acrc_left and acrc_right and tell with ab_extendedType
    if 11bit or 29 bit identifiers are used
    (uses BIOS function)

    possible errors:
    * Err_c::range BUS or MsgObj numbers out of allowed limits
    * Err_c::hwConfig BUS not initialized or ID can't be changed
    @param acrc_right reference to MsgObj_c which should be merged into this instance
    @return true -> successful merged; false -> too many FilterBox_c refs for one MsgObj_c
  */
  bool merge(MsgObj_c& acrc_right);

  /** close the t_data of this instance and close hardware CAN Msg Obj if it's open
    possible errors:
        * Err_c::range wrong BUS or MsgObj number stored in this instance
  */
  void close();

  /** close the correlated CAN object (uses BIOS function)
    possible errors:
        * Err_c::range wrong BUS or MsgObj number stored in this instance
  */
  void closeCan();

  /** test if a CAN msg object is open for this instance
    -> if the close function must be called in destructor
    @return true -> open
  */
  bool isOpen()const{return (bit_data.isOpen > 0);}

  //+++++++++++++++++++++++++++++++++++
  // functions for configuring MsgObj_c
  //+++++++++++++++++++++++++++++++++++
  /** checks wether the filter of the given MsgObj_c is the same
    @param acrc_other reference to MsgObj_c whoose filter should be compared with local filter
    @return true -> other MsgObj_c has same filter
  */
  inline bool operator==(const MsgObj_c& acrc_other)const
    {return (acrc_other.filter() == mc_filter);}

  /** set t_filter of this MsgObj_c
    @param acrc_val filter to set for this MsgObj_c
  */
 inline void setFilter(const Ident_c& acrc_val){mc_filter = acrc_val;}


  /** get the t_filter of this MsgObj_c
    @return filter of this MsgObj_c instance
  */
  inline const Ident_c& filter() const {return mc_filter;}
  /** update the ident value with the given mask --> clear any bit in ident, which are not set in given mask.
      Update the mask only, when the ident type of the referenced mask is the same.
    */
  void updateFilterWithMask( const Ident_c& acrc_mask )
    { mc_filter.updateWithMask( acrc_mask );}

  /** get the common filter part of all merged FilterBox instances
    @return common filter of all FilterBoxes in this MsgObj_c instance
  */
  void commonFilterAfterMerge( Ident_c& arc_globalMask );

  /** check if actual filter with specific filter type are equal to given combination
    @param acrc_filter compared filter setting
  */
  bool equalFilter(const Ident_c& acrc_filter) const
    {return (acrc_filter == mc_filter);}

  /** configures the CAN hardware of given Msg Object (uses BIOS function)
    possible errors:
        * Err_c::hwConfig used CAN BUS wasn't configured properly
        * Err_c::range given BUS or MsgObj number not in allowed area
        * Err_c::busy wanted MsgObj already in use
        * Err_c::unspecified some other error
    @param aui8_busNumber BUS number, where this instance should act
    @param aui8_msgNr CAN hardware msg number for BIOS interaction
    @return true -> BIOS CAN object without errors configured
  */
  bool configCan(uint8_t aui8_busNumber, uint8_t aui8_msgNr);

  //+++++++++++++++++++++++++++++++++++++++
  // manage what to do with received t_data
  //+++++++++++++++++++++++++++++++++++++++
  /** insert pointer to FilterBox_c which receive
    CAN messages by this msgObj and reports success with true
    possible errors:
        * Err_c::can_overflow to many references to FilterBox_c instances for this MsgObj_c
    @param arc_box reference to FilterBox_c which should be inserted as possible processing instance of msg received by this instance
    @return true -> this reference could be stored in this MsgObj_c (limited amount)
  */

  void insertFilterBox(FilterRef arc_box);




  /** delete pointer to a FilterBox_c and move following pointers one position forward
    possible errors:
        * Err_c::elNonexistant to be deleted FilterBox_c reference not registered for this MsgObj_c
    @param arc_box reference to FilterBox_c which should be deleted from reference array
    @return true -> given FilterBox_c was deleted from local reference array
  */
  bool deleteFilterBox(FilterRef arc_box);



  void clearArrFbIdx(){
    marr_filterBoxIndex.clear(); };

  /** deliver count of contained FilterBox_c pointers
    @return count of references to FilterBox_c instances
  */
  uint8_t cnt_filterBox()const
  {
    #if 0
    return bit_data.cnt_filterBox;
    #else

      return marr_filterBoxIndex.size();
    #endif
  }


  /** set if a CAN msg object is open for this instance
    -> if the close function must be called in destructor
    @param rb_state wanted state
  */
  void setIsOpen(bool rb_state){bit_data.isOpen = (rb_state)?1:0;}

  bool msgObjUpdateTable(uint8_t aui8_busNumber, uint8_t aui8_msgObjNr);

  bool prepareIrqTable(uint8_t aui8_busNum,uint8_t aui8_objNr,int32_t* cp_elem, uint32_t aui32_numEl);

  /** set Bus Number of this MsgObj_c
    @param ab_val wanted bus number
  */
  void setBusNumber(uint8_t ab_val){bit_data.busNumber = (ab_val<8)?ab_val:0;}

#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  void setCanSingletonKey(int ai_val){mi_canSingletonVecKey = ai_val;}
  int getCanSingletonKey(){return mi_canSingletonVecKey;}
#endif

private:

/** return the FilterBox instance */
inline FilterBox_c& getFilterBoxInstance(int32_t ai32_fbIdx );

  // Private attributes
 /** array of index of the CanIo_c::m_arrFilterBox VECTOR*/
 STL_NAMESPACE::vector<int32_t> marr_filterBoxIndex;



  /** Ident_c filter for this msgObj */
  Ident_c mc_filter;

  /** msgObjNr */
  struct {
    uint16_t ui8_msgObjNr  : 7;
    uint16_t isLocked      : 1;
    #if 0
    uint16_t cnt_filterBox : 4;
    #endif
    uint16_t isOpen        : 1;
    uint16_t busNumber     : 3;
  } bit_data;



 #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
/** define CAN singleton key based on amount of CAN_INSTANCE_CNT
      - the define allows to resolve to NO variable, in case of CAN_INSTANCE_CNT == 1
   */
  int mi_canSingletonVecKey;
  #endif

private:
// Private methods
  #if 0
  /** set counter for appointed arrPfilterBox instances
    @param ab_val wanted counter for appointed arrPfilterBox instances
  */
  void setCntFilterBox(uint8_t ab_val) { bit_data.cnt_filterBox = ab_val; }
  #endif



  /** deliver Bus Number of this MsgObj_c
    @return bus number
  */
  uint8_t busNumber()const{return bit_data.busNumber;}



  /** deliver msgObj number for BIOS interaction
    @return msgObj number
  */
  uint8_t msgObjNr()const{return bit_data.ui8_msgObjNr;}

  /** set msgObj number for BIOS interaction
    @param ab_val wanted msgObj number
  */
  void setMsgObjNr(uint8_t ab_val){bit_data.ui8_msgObjNr = ab_val;}

  /** verify given BUS number and MsgObj number, if they are within allowed
    limits (defined in isoaglib_config.h)
    if called withoutparameter values (default -1) the actual configured are
    checked -> if these are incorrect Err_c::range is set
    (mostly used by MsgObj_c to verify itself)
    @param ac_busNr number of the BUS (default the actual stored number is checked)
    @param ac_msgobjNr number of the MsgObj (default the actual stored number is checked)
    @return true -> values are correct
  */
  bool verifyBusMsgobjNr(int8_t ac_busNr = -1, int8_t ac_msgobjNr = -1);
};
} // end of __IsoAgLib namespace
#endif
