/*
  msgobj_c.cpp: MsgObj_c encapsulate a hardware CAN Msg Object, to
    present a standard interaction with CAN for all platforms

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "msgobj_c.h"
#include "canio_c.h"
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/hal/system.h>

#if defined(DEBUG) || defined(DEBUG_CAN_BUFFER_FILLING)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
namespace __IsoAgLib {

/** default constructor for MsgObj_c which only init all member values defined start state */
MsgObj_c::MsgObj_c()
  : marr_filterBoxIndex()
{
  // set all member variables to initial values
  mc_filter.setEmpty(true);
  setBusNumber(0);
  setMsgObjNr(0);
  setIsOpen(false);
#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  setCanSingletonKey(0);
#endif
}

/** copy constructor for this class, which gets data from another MsgObj_c instance
  @param acrc_src source MsgObj_c instance which should be cloned by this instance
*/
MsgObj_c::MsgObj_c(const MsgObj_c& acrc_src)
{ // set all member variables to the corresponding value from the source instance
  mc_filter = acrc_src.mc_filter;
  setMsgObjNr(acrc_src.msgObjNr());

#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  setCanSingletonKey(acrc_src.mi_canSingletonVecKey);
#endif


  setBusNumber(acrc_src.busNumber());
  // if constructor for acrc_src is later closed, the correlated CAN msg object would
  // be closed too -> the state of this instance would be undefined / wrong
  // -> set state of this object to Not-Open
  setIsOpen(false);

  // copy the registered pointers to FilterBox_c instances
  #if 0
  setCntFilterBox(acrc_src.cnt_filterBox());
  for (int16_t i = 0; i < cnt_filterBox(); i++) arrPfilterBox[i] = acrc_src.arrPfilterBox[i];
  #else
  clearArrFbIdx();
  marr_filterBoxIndex = acrc_src.marr_filterBoxIndex;
  #endif
}

/** destructor of MsgObj_c instance, which closes the hardware MsgObj_c */
MsgObj_c::~MsgObj_c()
{
  // check if this instances manges really a running BIOS Msg object
  if ((cnt_filterBox() != 0) || (mc_filter.ident() != 0)||(busNumber() != 0)||(msgObjNr() != 0))
  { // one of the values is different from the default setting
    if ((busNumber() < 2) && (msgObjNr() > 0) && (msgObjNr() < 16))
    { // CAN BUS number and the Msg Obj number are allowed
      // use member function close to clear registered pointers and to close hardware
      // CAN Msg Obj if open
      close();
    }
  }
}

/** merge two msgObj instances acrc_left and acrc_right and tell with ab_extendedType
  if 11bit or 29 bit identifiers are used
  (uses BIOS function)

  possible errors:
  * range BUS or MsgObj numbers out of allowed limits
  * hwConfig BUS not initialized or ID can't be changed
  @param acrc_right reference to MsgObj_c which should be merged into this instance
  @return true -> successful merged; false -> too many FilterBorefs for one MsgObj
*/
bool MsgObj_c::merge(MsgObj_c& acrc_right)
{

#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "MERGE : right OBJ filter = 0x"
  #ifdef SYSTEM_PC
  << STL_NAMESPACE::hex
  #endif
  << acrc_right.filter().ident() << INTERNAL_DEBUG_DEVICE_ENDL;
  INTERNAL_DEBUG_DEVICE << "MERGE : current OBJ filter = 0x" << mc_filter.ident() << INTERNAL_DEBUG_DEVICE_ENDL;

#endif
  // use only common bits for t_filter
  mc_filter.ident_bitAnd(acrc_right.filter());


  #if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
    INTERNAL_DEBUG_DEVICE << "existing FilterBox in merge " << INTERNAL_DEBUG_DEVICE_ENDL;
    printMyFilterBox();
  #endif


  // collect FilterBox_c references
  while ( !acrc_right.marr_filterBoxIndex.empty() )
  {
    marr_filterBoxIndex.push_back( acrc_right.marr_filterBoxIndex.back() );
    acrc_right.marr_filterBoxIndex.pop_back();
  }

  //exit function with success indication
  return true;
}

/** close the BIOS msg object of this instance and close hardware CAN Msg Obj if it's open
  possible errors:
      * range wrong BUS or MsgObj number stored in this instance
*/
void MsgObj_c::close()
{
  mc_filter.setEmpty(true);

  // the member function closeCan checks isOpen() and closes the BIOS object
  // if needed


 // closeCan();

  clearArrFbIdx();
}

/** insert pointer to FilterBox_c which receive
  CAN messages by this msgObj and reports success with true

  possible errors:
      * can_overflow to many references to FilterBox_c instances for this MsgObj
  @param arc_box reference to FilterBox_c which should be inserted as possible processing instance of msg received by this instance
  @return true -> this reference could be stored in this MsgObj_c (limited amount)
*/
bool MsgObj_c::insertFilterBox(FilterRef arc_box)
{


  for ( int i = 0; i < cnt_filterBox(); i++ )
  {

    // check if given FilterRef is already registered
   if ( marr_filterBoxIndex[i] == (*arc_box).getFbVecIdx() )


    {
      #if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
      INTERNAL_DEBUG_DEVICE << "Reject try to insert the same FilterBox a second time"
        << (*arc_box).getFbVecIdx() << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return true;
    }
  }
  const unsigned int cui_oldSize = marr_filterBoxIndex.size();
  marr_filterBoxIndex.push_back( (*arc_box).getFbVecIdx() );
 #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Inserted the index"
        << (*arc_box).getFbVecIdx() << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  if ( marr_filterBoxIndex.size() > cui_oldSize )
  { // fine


    return true;
  }
  else
  { // vector didn't grow --> out of memory
#ifdef DEBUG
     INTERNAL_DEBUG_DEVICE << "MsgObj_c::insertFilterBox : the vector doesn't grow " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

    getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
    return false;
  }
}

/** delete pointer to a Filter Box and move following pointers one position forward
  possible errors:
      * elNonexistent to be deleted FilterBox_c reference not registered for this MsgObj
  @param arc_box reference to FilterBox_c which should be deleted from reference array
  @return true -> given FilterBox_c was deleted from local reference array
*/
bool MsgObj_c::deleteFilterBox(FilterRef arc_box)
{
  bool b_result = false;

  for ( STL_NAMESPACE::vector<int32_t>::iterator iter = marr_filterBoxIndex.begin() ; iter != marr_filterBoxIndex.end(); iter++ )
  {
    if ( *iter == (*arc_box).getFbVecIdx() )
    { // to be deleted iterator is found
      marr_filterBoxIndex.erase( iter );
      b_result = true;
      break;
    }
  }
  if ( !b_result )
  { // nothing has been erased
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Can );
  }

  return b_result;
}



/** configures the CAN hardware of given Msg Object (uses BIOS function)
  possible errors:
      * hwConfig used CAN BUS wans't configured properly
      * range given BUS or MsgObj number not in allowed area
      * hwBusy wanted MsgObj already in use
      * unspecified some other error
  @param aui8_busNumber BUS number, where this instance should act
  @param aui8_msgNr CAN hardware msg number for BIOS interaction
  @return true -> BIOS CAN object without errors configured
*/
bool MsgObj_c::configCan(uint8_t aui8_busNumber, uint8_t aui8_msgNr)
{
  bool b_result = false;
  if (!verifyBusMsgobjNr(aui8_busNumber, aui8_msgNr))
  { // the given values are not within allowed limits (defined in isoaglib_config.h)
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  else
  { // values are o.k.
    // store mui8_busNumber for later close of can
    setBusNumber(aui8_busNumber);
    setMsgObjNr(aui8_msgNr);

    //store busNumber and msgNr for each filterBox
    for(uint8_t i = 0; i < cnt_filterBox(); i++)
    {
      getFilterBoxInstance(marr_filterBoxIndex[i]).configCan(aui8_busNumber, aui8_msgNr);
    }


  }


  switch (HAL::can_configMsgobjInit(aui8_busNumber, aui8_msgNr, mc_filter, 0))
  {
    case HAL_NO_ERR:
      /* no error -> clear error state */
      setIsOpen(true);
      // remove any previously active lock
      //lock( false );

      b_result = true;
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
      /* undefined BUS number, undefined BIOS-Obj number, wrong buffer size */
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
    default:
      /* unspecified error */
      getILibErrInstance().registerError( iLibErr_c::Unspecified, iLibErr_c::Can );
      break;
  }
  return b_result;
}

/** verify given BUS number and MsgObj number, if they are within allowed
  limits (defined in isoaglib_config.h)
  if called withoutparameter values (default -1) the actual configured are
  checked -> if these are incorrect range is set
  (mostly used by MsgObj_c to verify itself)
  @param ac_busNr number of the BUS (default the actual stored number is checked)
  @param ac_msgobjNr number of the MsgObj (default the actual stored number is checked)
  @return true -> values are correct
*/
bool MsgObj_c::verifyBusMsgobjNr(int8_t ac_busNr, int8_t ac_msgobjNr)
{
  //store parameter values to temp val, to check actual configured
  // values on default parameter value
  uint8_t b_testBus = (ac_busNr >= 0)?ac_busNr:busNumber();
  uint8_t b_testMsgobj = (ac_msgobjNr >= 0)?ac_msgobjNr:msgObjNr();

  //check if there is an error
  if (
     (b_testBus > HAL_CAN_MAX_BUS_NR)
  || (b_testMsgobj < HAL_CAN_MIN_REC_OBJ)
  || (b_testMsgobj > HAL_CAN_MAX_REC_OBJ)
     )
  { // set range-error status if called with default values for both parameters
    if ((ac_busNr < 0) && (ac_msgobjNr < 0)) getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  else
  { // set no-range-error status if called with default values for both parameters
    return true;
  }
}

/** get the common filter part of all merged
  FilterBox instances
  @return common filter of all FilterBoxes in this MsgObj_c instance
*/
void MsgObj_c::commonFilterAfterMerge( Ident_c& arc_globalMask )
{ // clear any bits which are no more used in the current global mask
  mc_filter.ident_bitAnd( arc_globalMask );
}


/** close the correlated CAN object
  (uses BIOS function)
  possible errors:
      * range wrong BUS or MsgObj number stored in this instance
*/
void MsgObj_c::closeCan()
{
  // check isOpen() and close CAN
  if (isOpen())
  {
    if (HAL::can_configMsgobjClose(busNumber(), msgObjNr() ) == HAL_RANGE_ERR)
    { // given BUS or MsgObj number is wrong
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    }
    setIsOpen(false);
  }
}



#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL

inline FilterBox_c& MsgObj_c::getFilterBoxInstance(int32_t ai32_fbIdx ){

#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
    return getCanInstance(static_cast <uint8_t>(getCanSingletonKey())).getFilterBoxInstance(ai32_fbIdx);
#else
    return getCanInstance().getFilterBoxInstance(ai32_fbIdx);
#endif
}


bool MsgObj_c::prepareIrqTable(uint8_t aui8_busNum,uint8_t aui8_objNr,int32_t* cp_elem, uint32_t aui32_numEl)
{

  if(aui8_busNum > HAL_CAN_MAX_BUS_NR || aui8_objNr >= HAL::cui8_numMsbObj)
  {
    // range error
    IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Process );
    return false;
  }

/**allocate memory for the new array*/
 HAL::comparableTable_s* p_newTable = static_cast<HAL::comparableTable_s*> (STL_NAMESPACE::malloc(sizeof(HAL::comparableTable_s) * aui32_numEl));

// problem on the dynamic memory allocation - Trace the error
  if(p_newTable == NULL)
  {
    //Bad allocation

#ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "\r\nALARM Not enough memory , malloc failed" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

    IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::BadAlloc, IsoAgLib::iLibErr_c::System );
    return false; // not enough memory for allocate a new element
  }

  HAL::comparableTable_s s_data;

  /** write the new table with the info on the FB vector */
  for(uint32_t tblIdx = 0; tblIdx < aui32_numEl ; tblIdx++ )
  {
      /** if the FbVecIdx is -1, doesn`t register the FB */
      if(cp_elem[tblIdx] < 0 )
      {
        //range error
        IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::ElNonexistent, IsoAgLib::iLibErr_c::Process );
        continue;
      }

      if(getFilterBoxInstance(cp_elem[tblIdx]).identType()  == __IsoAgLib::Ident_c::StandardIdent) //it is a standard id - 11 bits
      {
        s_data.fbMask = (getFilterBoxInstance(cp_elem[tblIdx]).mask().ident() & 0x7FF);
        s_data.fbFilter = (getFilterBoxInstance(cp_elem[tblIdx]).mask().ident() & getFilterBoxInstance(cp_elem[tblIdx]).filter().ident() & 0x7FF);
      }
      else //extended id - 29 bits
      {
        s_data.fbMask = getFilterBoxInstance(cp_elem[tblIdx]).mask().ident();
        s_data.fbFilter = getFilterBoxInstance(cp_elem[tblIdx]).mask().ident() & getFilterBoxInstance(cp_elem[tblIdx]).filter().ident() ;
      }
      s_data.i32_filterBoxVecIdx = cp_elem[tblIdx];

    /** insert the new element in the new table */
      p_newTable[tblIdx] = s_data;

  }

    bool b_ret = HAL::updateTable(aui8_busNum,aui8_objNr,marr_filterBoxIndex.size(),p_newTable);
    if(b_ret == false)
      return false;

  return true;
}

bool MsgObj_c::msgObjUpdateTable(uint8_t aui8_busNumber, uint8_t aui8_msgObjNr)
{

    bool b_ret = prepareIrqTable(aui8_busNumber,aui8_msgObjNr,&marr_filterBoxIndex[0],marr_filterBoxIndex.size());

    if(b_ret == false)
      return false;

    return true;
  }


#if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
void MsgObj_c::printMyFilterBox(){

INTERNAL_DEBUG_DEVICE << "OBJNBR = " << bit_data.ui8_msgObjNr << INTERNAL_DEBUG_DEVICE_ENDL;
#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
INTERNAL_DEBUG_DEVICE << " CAN instance = " << mi_canSingletonVecKey << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
INTERNAL_DEBUG_DEVICE << " CAN Number = " << int(bit_data.busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
INTERNAL_DEBUG_DEVICE << "MY FILTER = "
#ifdef SYSTEM_PC
<< STL_NAMESPACE::hex
#endif
<< mc_filter.ident() << INTERNAL_DEBUG_DEVICE_ENDL;


for ( int i = 0; i < cnt_filterBox(); i++ )
  {


    INTERNAL_DEBUG_DEVICE << "    MSGOBJ::marr_filterBoxIndex : FilterBox nr: " << i;
    INTERNAL_DEBUG_DEVICE  <<  "FilterBox: 0x"
	#ifdef SYSTEM_PC
     << STL_NAMESPACE::hex
	#endif
     << getFilterBoxInstance(marr_filterBoxIndex[i]).filter().ident()
      << ", Mask: 0x"
	  #ifdef SYSTEM_PC
      << STL_NAMESPACE::hex
	  #endif
      << getFilterBoxInstance(marr_filterBoxIndex[i]).mask().ident()
      << ", IdentType: "
     #ifdef SYSTEM_PC
     << STL_NAMESPACE::dec
     #endif
     << getFilterBoxInstance(marr_filterBoxIndex[i]).identType()
     << ", FilterBox index : "
	#ifdef SYSTEM_PC
     << STL_NAMESPACE::dec
     #endif
     << getFilterBoxInstance(marr_filterBoxIndex[i]).getFbVecIdx();
   INTERNAL_DEBUG_DEVICE <<  INTERNAL_DEBUG_DEVICE_ENDL;

  }


 INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
}

#endif //SYSTEM_WITH_ENHANCED_CAN_HAL

#endif

} // end namespace __IsoAgLib
