/*
  mutex_emulated.h:
    definition of Hardware Abstraction Layer for central system
    functions for debug/simulation system on PC

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_MUTEX_EMULATED_H_
#define _HAL_MUTEX_EMULATED_H_


namespace HAL
{

#ifdef USE_MUTUAL_EXCLUSION
/**
 * Class for a mutual exclusive use of a resource
 * @short Class for a mutual exclusive use of a resource.
*/

class ExclusiveAccess_c {
public:

  ExclusiveAccess_c() {// init the Mutex
                        mi_exclusiveAccess = 0; //unlocked
                      }

  virtual ~ExclusiveAccess_c() {// destroy the Mutex
                            }

  int  waitAcquireAccess() {
                              while(mi_exclusiveAccess == 1); //if locked, wait until unlocked
                              // we reachonly here, if unlocked
                              mi_exclusiveAccess = 1;

                              return 0;
                            }

  int  tryAcquireAccess(){
                            if(mi_exclusiveAccess == 0) //if unlocked , lock it
                            {
                                mi_exclusiveAccess = 1;
                                return 0;
                            }
                            else //if locked return
                             return 1; //was locked
}

  int releaseAccess(){ mi_exclusiveAccess = 0; return 0;}


private:

  volatile int mi_exclusiveAccess; // 1 = locked, 0 = unlocked

};//end class ExclusiveAccess_c

#endif





}

#endif


