/*
  processelementbase_c.h: base class for elements of ProcessData

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESS_ELEMENT_BASE_H
#define PROCESS_ELEMENT_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include "procdatabase_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataBase_c;

/**
  base class for elements of ProcessData which has pointer to
  the containing ProcessData item, inherit pointer to Scheduler_c from Scheduler_Task_c
  @author Dipl.-Inform. Achim Spangler
*/
class ProcessElementBase_c : public ClientBase  {
private:
public: 
  /**
    constructor which initialse both pointers if given
    @param apc_processData optional pointer to containing ProcessData instance
  */
  ProcessElementBase_c( ProcDataBase_c *const apc_processData = NULL );
  /**
    constructor which initialse both pointers if given
    @param arc_processData optional reference to containing ProcessData instance
  */
  ProcessElementBase_c( ProcDataBase_c &arc_processData );
  /**
    copy constructor
    @param acrc_src source ProcessElementBase_c instance
  */
   ProcessElementBase_c(const ProcessElementBase_c& acrc_src);
  /**
    assignment operator for ProcessElementBase
    @param acrc_src source ProcessElementBase_c instance
  */
  const ProcessElementBase_c& operator=(const ProcessElementBase_c& acrc_src);
  /** default destructor which has nothing to do */
  ~ProcessElementBase_c();
  
  /**
    set the pointer to Scheduler_c and ProcessData by references to the object instances
    @param arc_processData optional reference to containing ProcessData instance
  */
  void set(ProcDataBase_c& arc_processData );
  /**
    set the pointer to Scheduler_c and ProcessData by pointer to the object instances
    @param acpc_processData optional pointer to containing ProcessData instance
  */
  void set(ProcDataBase_c *const acpc_processData );
  
  /**
    deliver a reference to the containing ProcessData item
    @return reference to containing ProcessData
  */
  ProcDataBase_c& processData() const { return *mpc_processData; }
  /**
    deliver the pointer to the containing ProcessData item
    @return pointer to containing ProcessData
  */
  ProcDataBase_c * pprocessData() const { return mpc_processData; }
  /**
    deliver a reference to the containing ProcessData item
    @return reference to containing ProcessData
  */
  const ProcDataBase_c& processDataConst() const { return *mpc_processData; }
  /**
    deliver the pointer to the containing ProcessData item
    @return pointer to containing ProcessData
  */
  const ProcDataBase_c * pprocessDataConst() const { return mpc_processData; }

private: // Private attributes
  /** pointer to the containing ProcessData item */
  ProcDataBase_c* mpc_processData;
};

}
#endif
