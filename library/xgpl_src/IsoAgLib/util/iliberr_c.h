/*
  iliberr_c.h: central object for error management

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _ILIB_ERR_H
#define _ILIB_ERR_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/impl/singleton.h>

#include <vector>

template <unsigned N> class IsoaglibBitset {
  private:
    STL_NAMESPACE::vector<uint32_t> _v;
  public:
    IsoaglibBitset( void ): _v((N+31)>>5, uint32_t(0) ) {}
		IsoaglibBitset( const IsoaglibBitset& ar_src ) : _v(ar_src._v){};

    IsoaglibBitset<N>& set( void ) {
      STL_NAMESPACE::vector<uint32_t>::iterator i;
      for (i=_v.begin(); i!=_v.end(); ++i) *i = 0xFFFFFFFFUL;
      return *this;
    }

    IsoaglibBitset<N>& reset( void ) {
      STL_NAMESPACE::vector<uint32_t>::iterator i;
      for (i=_v.begin(); i!=_v.end(); ++i) *i = 0;
      return *this;
    }

    bool test(unsigned n) const {
      return bool((_v[n>>5] & (uint32_t(1)<<(n&0x1F)))>0);
    }

    IsoaglibBitset<N>& set( unsigned n, int val = 1 ) {
      if (0 == val)
        _v[n>>5] &= ~(uint32_t(1)<<(n&0x1F));
      else
        _v[n>>5] |=  (uint32_t(1)<<(n&0x1F));
      return *this;
    }

    IsoaglibBitset<N>& reset( unsigned n ) {
      return set(n,0);
    }

    unsigned count( void ) const {
      unsigned n,c=0;
      for (n=0; n<N; ++n) if (test(n)) ++c;
      return c;
    }

};


#define ERR_FIELD_COUNTER_SIZE 2

// Begin Namespace IsoAgLib
namespace IsoAgLib {
  class iLibErr_c;
  typedef SINGLETON( iLibErr_c ) SingletonILibErr_c;

  /** C-style function, to get access to the unique iLibErr_c singleton instance */
  iLibErr_c& getILibErrInstance( void );

/**
  Basic object for Error Management:
  define all error codes as Bit-Values combined of common and specific error informations
  some functions for state check and setting of error state;
  All objects which Can hold their own error state are derived from Err
  *@author Dipl.-Inform. Achim Spangler
*/
class iLibErr_c : public SingletonILibErr_c {
public:
  /** enum to identify the different error types */
  typedef enum iLibErrTypes_en {
    Precondition=0,
    BadAlloc=1,
    ElNonexistent=2,
    Unspecified=3,
    HwConfig=4,
    Range=5,
    Busy=6,
    SystemOpen=7,
    SystemWatchdog=8,
    Inconsistency=9,
    CanOff=10,
    CanBus=11,
    CanMsgObj=12,
    CanMem=13,
    CanOverflow=14,
    CanConflict=15,
    CanDiffSpeed=16,
    Rs232Overflow=17,
    Rs232Underflow=18,
    EepromSegment=19,
    EepromWriteError=20,
    SysNoActiveLocalMember=21,
    BaseSenderConflict=22,
    RemoteServiceOutOfMemory=23,
    FIRSTWARNINDEX =24, /// this is only a dummy to identify at which index the first warning is located
    CanWarn=24,
    MultiSendWarn=25,
    CanTooSlowSend=26,
    AllErrTypes=27,
  } iLibErrTypes_t;

  /** enum to identify the different error locations */
  typedef enum iLibErrLocations_en {
    Base=0,
    System=1,
    Process=2,
    IsoTerminal=3,
    Scheduler=4,
    MultiReceive=5,
    MultiSend=6,
    HwSystem=7,
    Can=8,
    Eeprom=9,
    Sensor=10,
    Actor=11,
    Rs232=12,
    TaskController=13,
    ProprietaryCan=14,
    AllErrLocations=15,
  } iLibErrLocations_t;

  /**
    Initialize the unique error state handler
    @return true -> everything without errors initialised
  */
  bool init( void );
  /** register an error
    * @param at_errType type of occured error
    * @param at_errLocation location of error
    */
  void registerError( iLibErrTypes_t at_errType, iLibErrLocations_t at_errLocation );

  /**
    clear all error information
  */
  void clear( void ) { clear( AllErrTypes, AllErrLocations ); }

  /**
    clear specific error information of a special location
    @param at_errLocation select for which error locations the counter shall be reset
    @param at_errType select for which error types the counter shall be reset (default reset all counters)
  */
  void clear( iLibErrTypes_t at_errType, iLibErrLocations_t at_errLocation );

  /**
    clear all error information of a special type
    @param at_errType select for which error types the counter shall be reset
  */
  void clear( iLibErrTypes_t at_errType );

  /**
    clear all error information of a special location
    @param at_errLocation select for which error locations the counter shall be reset
  */
  void clear( iLibErrLocations_t at_errLocation );

  /**
    deliver the count of registered errors
    @param at_errType optional select the error-types to count (default all error types)
    @return count of registered error types
  */
  uint16_t getErrCnt( iLibErrTypes_t at_errType = AllErrTypes ) const;

  /**
    deliver the count of registered errors with a special error location
    @param at_errLocation location to check for errors
    @return count of registered error types
  */
  uint16_t getErrCnt( iLibErrLocations_t at_errLocation ) const;

  /**
    deliver the type of the nth error
    @param aui8_ind index of the requested error ( [0..(ErrCnt-1)] )
    @return iLibErrTypes_t
  */
  iLibErrTypes_t getErrIndType( uint8_t aui8_ind ) const;

  /**
    deliver the location of the nth error
    @param aui8_ind index of the requested error ( [0..(ErrCnt-1)] )
    @return iLibErrTypes_t
  */
  iLibErrLocations_t getErrIndLocation( uint8_t aui8_ind ) const;

  /**
    check if the error value reports correct error free state
    @param at_errType optional select the error-types to count (default all error types)
    @return true -> the defined error locations report no error
  */
  bool good( iLibErrTypes_t at_errType = AllErrTypes ) const;

  /**
    check if the error value reports correct error free state
    @param at_errLocation location to check for errors
    @return true -> the defined error locations report no error
  */
  bool good( iLibErrLocations_t at_errLocation ) const;

  bool good( iLibErrTypes_t at_errType, iLibErrLocations_t at_errLocation ) const;

  /**
    check if the object has a local error
  */
  inline bool operator!(){ return (!good(AllErrTypes));};

  /** default destructor which has nothing to do */
  ~iLibErr_c() {}

private:
  friend class SINGLETON( iLibErr_c );

  /** default constructor which sets the error value to noErr */
  iLibErr_c();

  /** copy constructor which sets the error value to the err value of the source */
  iLibErr_c(const iLibErr_c& acrc_src);

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit() { init(); };

  IsoaglibBitset<AllErrTypes> errTypeAtLoc [AllErrLocations];
};

} // end of namespace IsoAgLib

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
  /** C-style function, to get access to the unique iLibErr_c singleton instance */
  IsoAgLib::iLibErr_c& getILibErrInstance( void );
  using IsoAgLib::iLibErr_c;
}
#endif
