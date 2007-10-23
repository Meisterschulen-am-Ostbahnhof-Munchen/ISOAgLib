/***************************************************************************
                          managemeasureproglocal_c.h - managing of local
                                                 process data object
                             -------------------
    begin                : Fri Apr 07 2000
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
#ifndef MANAGE_MEASUREPROG_LOCAL_H
#define MANAGE_MEASUREPROG_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../../impl/processelementbase_c.h"
#include "../../StdMeasureElements/impl/measureproglocal_c.h"

#ifdef DO_USE_SLIST
  #if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && !defined(SYSTEM_A1) && __GNUC__ >= 3
    #include <ext/slist>
    namespace std { using __gnu_cxx::slist;}
  #else
    #include <slist>
  #endif
#else
  #include <list>
#endif

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
/**
  management of a measurement value
  with possibility of measurement programs and
  MIN/MAX... value handling.
  This is especially usefull for process data values
  where other systems can be interested in MIN/MAX/AVG
  with/without measure programs.
  */
class ManageMeasureProgLocal_c : public ProcessElementBase_c
{
private:
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureProgLocal_c,STL_NAMESPACE::__malloc_alloc_template<0> > Vec_MeasureProgLocal;
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureProgLocal_c,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator Vec_MeasureProgLocalIterator;
  #else
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureProgLocal_c> Vec_MeasureProgLocal;
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureProgLocal_c>::iterator Vec_MeasureProgLocalIterator;
  #endif
 public:
  /**
    constructor which initialse both pointers if given
    @param apc_processData optional pointer to containing ProcessData instance
  */
  ManageMeasureProgLocal_c( ProcDataBase_c *const apc_processData = NULL )
  : ProcessElementBase_c( apc_processData )
  { init( apc_processData );};
  virtual ~ManageMeasureProgLocal_c();

  /**
    initialise this ManageMeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcessData instance
  */
  void init( ProcDataBase_c *const apc_processData = NULL );

  /** copy constructor */
  ManageMeasureProgLocal_c( const ManageMeasureProgLocal_c& arc_src );

  /** assignment operator */
  const ManageMeasureProgLocal_c& operator=( const ManageMeasureProgLocal_c& arc_src );

  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

  /** process a measure prog message for local process data */
  void processProg();

  /**
    check if specific measureprog exist
    @param ac_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(const IsoName_c& ac_isoName)
      {return updateProgCache(ac_isoName, false);}

  /**
    search for suiting measureprog, if not found AND if ab_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

    @param ac_isoName DEVCLASS code of searched measure program
    @param ab_doCreated true -> create suitable measure program if not found
  */
  MeasureProgLocal_c& prog(const IsoName_c& ac_isoName, bool ab_doCreate);

  /** initialise value for all registered Measure Progs */
  void initGlobalVal( int32_t ai32_val );

  /** set value for all registered Measure Progs */
  void setGlobalVal( int32_t ai32_val );

  #ifdef USE_FLOAT_DATA_TYPE
  /** initialise value for all registered Measure Progs */
  void initGlobalVal( float af_val );

  /** set value for all registered Measure Progs */
  void setGlobalVal( float af_val );
  #endif // USE_FLOAT_DATA_TYPE

  /**
    allow local client to actively start a measurement program
    (to react on a incoming "start" command for default data logging)
    @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment
    @param apc_receiverDevice commanding ISOName
    @return true -> apc_receiverDevice is set
  */
  bool startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const IsoName_c* apc_receiverDevice );
  /**
    stop all measurement progs in all local process instances, started with given isoName
    @param rc_isoName
  */
  void stopRunningMeasurement(const IsoName_c& rc_isoName);


 protected:
  friend class ProcDataLocal_c;
  friend class ProcDataLocalSimpleSetpoint_c;
  /**
    deliver a reference to the list of measure prog objects
    @return reference to list of measure prog entries
  */
  Vec_MeasureProgLocal& vec_prog() { return c_vec_prog;};
  /**
    deliver a reference to the list of measure prog objects
    @return reference to list of measure prog entries
  */
  const Vec_MeasureProgLocal& constVecProg() const { return c_vec_prog;};
  /**
    create a new measure prog item;
    if there is still the default initial item undefined define it
    and create no new item

    possible errors:
        * Err_c::badAlloc not enough memory to insert new MeasureProgLocal

    @param ac_isoName commanding ISOName
  */
  void insertMeasureprog(const IsoName_c& ac_isoName);
  /**
    update the programm cache, create an programm item, if wanted

    possible errors:
        * Err_c::badAlloc not enough memory to insert new MeasureProgLocal

    @param ac_isoName commanding ISOName
    @param ab_createIfNotFound true -> create new item if not found
    @return true -> instance found
  */
  bool updateProgCache(const IsoName_c& ac_isoName, bool ab_createIfNotFound);
 protected:
  /** container of objects for managing jobs of local measure programs */
  Vec_MeasureProgLocal c_vec_prog;
  /** cache iterator to measure prog */
  Vec_MeasureProgLocalIterator pc_progCache;
 private:
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ManageMeasureProgLocal_c& arc_src );
  /** create first default measure prog, if no measure prog in list */
  void checkInitList( void );

};


}
#endif
