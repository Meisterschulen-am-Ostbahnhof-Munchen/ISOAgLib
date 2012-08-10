/***************************************************************************
                          managemeasureproglocal_c.h - managing of local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MANAGE_MEASUREPROG_LOCAL_H
#define MANAGE_MEASUREPROG_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processelementbase_c.h>
#include "measureproglocal_c.h"

#include <list>

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
  typedef STL_NAMESPACE::list<MeasureProgLocal_c,MALLOC_TEMPLATE(MeasureProgLocal_c) > Vec_MeasureProgLocal;
  typedef STL_NAMESPACE::list<MeasureProgLocal_c,MALLOC_TEMPLATE(MeasureProgLocal_c)  >::iterator Vec_MeasureProgLocalIterator;
  #else
  typedef STL_NAMESPACE::list<MeasureProgLocal_c> Vec_MeasureProgLocal;
  typedef STL_NAMESPACE::list<MeasureProgLocal_c>::iterator Vec_MeasureProgLocalIterator;
  #endif
 public:
  /**
    constructor which initialse both pointers if given
    @param apc_processData optional pointer to containing ProcessData instance
  */
  ManageMeasureProgLocal_c( ProcDataLocal_c *const apc_processData = NULL )
  : ProcessElementBase_c( apc_processData )
  { init( apc_processData );};
  virtual ~ManageMeasureProgLocal_c();

  /**
    initialise this ManageMeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcessData instance
  */
  void init( ProcDataLocal_c *const apc_processData = NULL );

  /** copy constructor */
  ManageMeasureProgLocal_c( const ManageMeasureProgLocal_c& acrc_src );

  /** assignment operator */
  const ManageMeasureProgLocal_c& operator=( const ManageMeasureProgLocal_c& acrc_src );

  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

  /** process a measure prog message for local process data */
  void processProg( const ProcessPkg_c& arc_data );

  /**
    check if specific measureprog exist
    @param acrc_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(const IsoName_c& acrc_isoName)
      {return updateProgCache(acrc_isoName, false);}

  /**
    search for suiting measureprog, if not found AND if ab_doCreate == true
    create copy from first element at end of vector
    @param acrc_isoName DEVCLASS code of searched measure program
    @param ab_doCreate true -> create suitable measure program if not found
  */
  MeasureProgLocal_c& prog(const IsoName_c& acrc_isoName, bool ab_doCreate);

  /** initialise value for all registered Measure Progs */
  void initGlobalVal( int32_t ai32_val );

  /** set value for all registered Measure Progs */
  void setGlobalVal( int32_t ai32_val );

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
  /**
    deliver a reference to the list of measure prog objects
    @return reference to list of measure prog entries
  */
  Vec_MeasureProgLocal& vec_prog() { return mvecc_prog;};
  /**
    deliver a reference to the list of measure prog objects
    @return reference to list of measure prog entries
  */
  const Vec_MeasureProgLocal& constVecProg() const { return mvecc_prog;};
  /**
    create a new measure prog item;
    if there is still the default initial item undefined define it
    and create no new item
    @param acrc_isoName commanding ISOName
  */
  void insertMeasureprog(const IsoName_c& acrc_isoName);
  /**
    update the programm cache, create an programm item, if wanted
    @param acrc_isoName commanding ISOName
    @param ab_createIfNotFound true -> create new item if not found
    @return true -> instance found
  */
  bool updateProgCache(const IsoName_c& acrc_isoName, bool ab_createIfNotFound);
 protected:
  /** container of objects for managing jobs of local measure programs */
  Vec_MeasureProgLocal mvecc_prog;
  /** cache iterator to measure prog */
  Vec_MeasureProgLocalIterator mpc_progCache;
 private:
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ManageMeasureProgLocal_c& acrc_src );
  /** create first default measure prog, if no measure prog in list */
  void checkInitList( void );

};


}
#endif
