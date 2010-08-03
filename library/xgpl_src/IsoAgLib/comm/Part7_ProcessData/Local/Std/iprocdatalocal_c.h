/***************************************************************************
                          iprocdatalocal_c.h  - managing of local
                                                       process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IPROCESS_DATA_LOCAL_H
#define IPROCESS_DATA_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/procdatalocal_c.h"
#include <IsoAgLib/comm/Part7_ProcessData/Local/StdMeasureElements/imeasureproglocal_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/Local/StdSetpointElements/isetpointlocal_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/proc_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/iprocesscmd_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// ***************************************************************************************
// ****************** ProcDataLocal_c ******************
// ***************************************************************************************

/**
  @brief Interface for managing of local process data object with standard
  ( %i.e. no restrictions ) feature set.

  Has __IsoAgLib::SetpointLocal_c / IsoAgLib::iSetpointLocal_c and __IsoAgLib::ManageMeasureProgLocal_c / IsoAgLib::iManageMeasureProgLocal_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Local/StdMeasureElements,
  - \e Local/StdSetpointElements,
  - \e StdMeasureElements,
  - \e StdSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::iIsoName c_myIsoName( 1, 0 );
  // creation of process data instance
  iProcDataLocal_c c_workState;
  // init for LIS=0, local device type/subtype=1/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send ),
  // load/store measurememnt data to/from EEPROM
  c_workState.init( 0, 0, myIsoName, c_myIsoName, &c_myIsoName, false, 0x1234 );

  // update current measurement value ( real value, which can differ from commanded value )
  c_workState.setMasterMeasurementVal( 100 );
  // measurement programs and value requests are handled in the backend of the IsoAgLibrary

  // check if unhandled setpoints are received
  if ( c_workState.setpoint().unhandledCnt() > 0 ) {
    if ( ! c_workState.setpoint().existMaster() ) {
      for ( uint8_t ind = 0; ind < c_workState.setpoint().unhandledCnt(); ind++ )
      { // now decide on all received setpoints
        if ( ( c_workState.setpoint().unhandledCnt() == 1 )
          && ( ( c_workState.setpoint().unhandledFirst().exact() == 100 ) // undhandledFirst() is shortcut for unhandledInd( 0 )
            || ( c_workState.setpoint().unhandledInd( 0 ).exact() == 0   ) ) )
        { // as no master exist, and only one setpoint received -> simply accept if value is 0 or 100
          c_workState.setpoint().unhandledFirst().setMaster( true );
        }
      }
    }
    else if ( ( c_workState.setpoint().unhandledMaster().exact() == 100 )
           || ( c_workState.setpoint().unhandledMaster().exact() == 0 ) )
    { // simply accept new master if 0 or 100
      c_workState.setpoint().acceptNewMaster();
    }
    // send answer for all received setpoints
    c_workState.setpoint().respondAckNack();
  }
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class iProcDataLocal_c
: private __IsoAgLib::ProcDataLocal_c  {
private:
  friend class EventSource_c;
public:
  /**
    constructor which can set all element vars

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param acrc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param ab_cumulativeValue
             -# for process data like distance, time, area
                 the value of the measure prog data sets is updated
                 on master value update dependent on the value increment
                 since the last master value update
                 -> if a remote member resets his data set copy, datas of
                    other members aren't changed
                 -> if this data is saved in EEPROM, the main application
                    needn't take into account the initial EEPROM value, as
                     setting of the master val is independent from EEPROM
             -#  for values like speed, state, rpm aren't updated by increment,
                  -> the given master value is propagated equally to all
                      measure prog data sets
                  -> if this data is saved in EEPROM, the stored value is loaded
                     as initial master value, and is initially propagated to all
                     measure prog data sets
    @param aui16_eepromAdr optional adress where value is stored in EEPROM
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iProcDataLocal_c( const ElementDdi_s* ps_elementDDI = NULL,
                    uint16_t aui16_element = 0xFFFF,
                    const iIsoName_c& acrc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
                    const iIsoName_c *apc_externalOverridingIsoName = NULL,
                    bool ab_cumulativeValue = false,
#ifdef USE_EEPROM_IO
                  uint16_t aui16_eepromAdr = 0xFFFF,
#endif
                  ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                  int ai_singletonVecKey = 0)
    : ProcDataLocal_c( ps_elementDDI, aui16_element,
                       acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                       aui16_eepromAdr,
#endif
                       apc_processDataChangeHandler,
                       ai_singletonVecKey
                      )
{}

  /**
    constructor whith alternative parameter list
    @param aui16_DDI
    @param aui16_element
    ...
  */
  iProcDataLocal_c( uint16_t aui16_DDI, uint16_t aui16_element,
                    const iIsoName_c& acrc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
                    const iIsoName_c *apc_externalOverridingIsoName = NULL,
                    bool ab_cumulativeValue = false,
#ifdef USE_EEPROM_IO
                   uint16_t aui16_eepromAdr = 0xFFFF,
#endif
                   ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                   int ai_singletonVecKey = 0)
    : ProcDataLocal_c( NULL, aui16_element, acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                      aui16_eepromAdr,
#endif
                      apc_processDataChangeHandler,
                      ai_singletonVecKey
                      )
  {
    const ElementDdi_s s_tmpElementDDI[2] =
    {
      // if this constructor is used => only exact measurement possible
      {aui16_DDI, false, ProcessCmd_c::exactValue},
      {0xFFFF, false, ProcessCmd_c::noValue}
    };

    ProcDataLocal_c::init( s_tmpElementDDI, aui16_element,
                      acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                      aui16_eepromAdr,
#endif
                      apc_processDataChangeHandler,
                      ai_singletonVecKey
                      );
   }

  /**
    initialise this ProcDataLocal_c
    instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param acrc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param ab_cumulativeValue
             -# for process data like distance, time, area
                 the value of the measure prog data sets is updated
                 on master value update dependent on the value increment
                 since the last master value update
                 -> if a remote member resets his data set copy, datas of
                    other members aren't changed
                 -> if this data is saved in EEPROM, the main application
                    needn't take into account the initial EEPROM value, as
                     setting of the master val is independent from EEPROM
             -#  for values like speed, state, rpm aren't updated by increment,
                  -> the given master value is propagated equally to all
                      measure prog data sets
                  -> if this data is saved in EEPROM, the stored value is loaded
                     as initial master value, and is initially propagated to all
                     measure prog data sets
    @param aui16_eepromAdr optional adress where value is stored in EEPROM
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init( const ElementDdi_s* ps_elementDDI = NULL,
             uint16_t aui16_element = 0xFFFF,
             const iIsoName_c& acrc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
             const iIsoName_c *apc_externalOverridingIsoName = NULL, bool ab_cumulativeValue = false,
#ifdef USE_EEPROM_IO
            uint16_t aui16_eepromAdr = 0xFFFF,
#endif
            ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_singletonVecKey = 0
            )
  {ProcDataLocal_c::init( ps_elementDDI, aui16_element,
                         acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                         aui16_eepromAdr,
#endif
                         apc_processDataChangeHandler,
                         ai_singletonVecKey);
  }

  /**
    <!--ISO only: -->Initialise this ProcDataLocal_c instance to a well defined initial state
              this alternative uses DDI and element number as parameter and not pointer to list of ElementDdi_s

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param aui16_DDI
    @param aui16_element

    @param acrc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param ab_cumulativeValue
             -# for process data like distance, time, area
                 the value of the measure prog data sets is updated
                 on master value update dependent on the value increment
                 since the last master value update
                 -> if a remote member resets his data set copy, datas of
                    other members aren't changed
                 -> if this data is saved in EEPROM, the main application
                    needn't take into account the initial EEPROM value, as
                     setting of the master val is independent from EEPROM
             -#  for values like speed, state, rpm aren't updated by increment,
                  -> the given master value is propagated equally to all
                      measure prog data sets
                  -> if this data is saved in EEPROM, the stored value is loaded
                     as initial master value, and is initially propagated to all
                     measure prog data sets
    @param aui16_eepromAdr optional adress where value is stored in EEPROM
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init( uint16_t aui16_DDI, uint16_t aui16_element,
             const iIsoName_c& acrc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
             const iIsoName_c *apc_externalOverridingIsoName = NULL, bool ab_cumulativeValue = false,
#ifdef USE_EEPROM_IO
            uint16_t aui16_eepromAdr = 0xFFFF,
#endif
            ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_singletonVecKey = 0
            )
  {
     const ElementDdi_s s_tmpElementDDI[2] =
     {
       // if this init is used => only exact measurement possible
       {aui16_DDI, false, ProcessCmd_c::exactValue},
       {0xFFFF, false, ProcessCmd_c::noValue}
     };

     ProcDataLocal_c::init( s_tmpElementDDI, aui16_element,
                            acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                            aui16_eepromAdr,
#endif
                            apc_processDataChangeHandler,
                            ai_singletonVecKey);
  }

  /** set the poitner to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { ProcDataLocal_c::setProcessDataChangeHandler( apc_processDataChangeHandler ); }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataLocal_c::getProcessDataChangeHandler(); }

  /**
    deliver value DEVCLASS (machine type specific table of process data types)
    @return DEVCLASS
  */
  uint8_t devClass() const{return ProcDataLocal_c::devClass();}

  /**
    deliver value ISOName (machine type specific table of process data types)
    use everytime the _device_class_ from the ident part, and take the _instance_ from the owner
    @return ISOName
  */
  const iIsoName_c& isoName() const {return ProcDataLocal_c::isoName().toConstIisoName_c();}

  /**
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    @return POS
  */
  uint8_t devClassInst() const{return ProcDataLocal_c::devClassInst();}

  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t aui16_checkDDI ) const { return ProcIdent_c::hasDDI( aui16_checkDDI );}

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
   */
  uint16_t DDI() const { return ProcIdent_c::DDI();}

  /**
  deliver value element (only possible if only one elementDDI in list)
  @return element
   */
  uint16_t element() const  { return ProcIdent_c::element();}

  /**
    deliver DDI from last received can pkg
    @return DDI
  */
  uint16_t getDDIfromCANPkg() const { return ProcDataBase_c::getDDIfromCANPkg();}

  /**
    send the base value (the one that is updated via setMasterMeasurementVal() )
    to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterMeasurementVal( const iIsoName_c& ac_targetISOName ) const
    { return ProcDataLocal_c::sendMasterMeasurementVal( ac_targetISOName );}

  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterSetpointVal( const iIsoName_c& ac_targetISOName ) const
   { return setpointConst().sendMasterSetpointVal( ac_targetISOName );}

  /**
    send a sub-setpoint (selected by value group) to a specified target (selected by GPT)
    @param en_valueGroup select sub-type of setpoint
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendSetpointForGroup( ProcessCmd_c::ValueGroup_t en_valueGroup, const iIsoName_c& ac_targetISOName ) const
  {
    return setpointConst().sendSetpointForGroup( ac_targetISOName,
                                            en_valueGroup, __IsoAgLib::ProcessCmd_c::setValue );
  }

  #ifdef USE_EEPROM_IO
  /**
    deliver the eeprom adr for the value
    @return configured EEPROM adress
  */
  uint16_t eepromAdr()const{return ProcDataLocal_c::eepromAdr();}

  /**
    set the eeprom adr for the value, read in value from EEPROM

    possible errors:
        * dependent error in EepromIo_c on problems during read
    @param aui16_eepromAdr new EEPROM adress
  */
  void setEepromAdr(uint16_t aui16_eepromAdr)
    {ProcDataLocal_c::setEepromAdr(aui16_eepromAdr);}
  #endif

  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal()const{return ProcDataLocal_c::masterMeasurementVal();}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai32_val new measure value
  */
  void setMasterMeasurementVal(int32_t ai32_val)
    {ProcDataLocal_c::setMasterMeasurementVal(ai32_val);}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai16_val new measure value
  */
  void setMasterMeasurementVal(int16_t ai16_val)
    {ProcDataLocal_c::setMasterMeasurementVal((int32_t)ai16_val);}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ab_val new measure value
  */
  void setMasterMeasurementVal(uint8_t ab_val)
    {ProcDataLocal_c::setMasterMeasurementVal((int32_t)ab_val);}

  /**
    increment the value -> update the local and the measuring programs values
    @param ai32_val size of increment of master value
  */
  void incrMasterMeasurementVal(int32_t ai32_val)
    {ProcDataLocal_c::incrMasterMeasurementVal(ai32_val);}

  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  int32_t setpointMasterVal() const
    { return ProcDataLocal_c::setpointConst().setpointMasterVal();}

  /**
    set the setpoint value
    @param ai32_val new setpoint value
  */
  void setSetpointMasterVal(int32_t ai32_val)
    { ProcDataLocal_c::setpoint().setSetpointMasterVal(ai32_val);}

   /**
    check if specific measureprog exist
    @param acrc_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(const iIsoName_c& acrc_isoName)
      {return ProcDataLocal_c::existProg(acrc_isoName);}

  /**
    search for suiting measureprog, if not found AND if ab_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

    @param acrc_isoName DEVCLASS code of searched measure program
    @param ab_doCreate true -> create suitable measure program if not found
  */
  iMeasureProgLocal_c& prog(const iIsoName_c& acrc_isoName, bool ab_doCreate)
    { return static_cast<iMeasureProgLocal_c&>(ProcDataLocal_c::prog(acrc_isoName, ab_doCreate));}

  /** deliver reference to setpoint */
  iSetpointLocal_c& setpoint( void )
    { return static_cast<iSetpointLocal_c&>(ProcDataLocal_c::setpoint()); }

  bool startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const iIsoName_c* apc_receiverDevice = NULL )
    { return ProcDataLocal_c::startDataLogging(ren_type, ai32_increment, apc_receiverDevice); }


};

}
#endif
