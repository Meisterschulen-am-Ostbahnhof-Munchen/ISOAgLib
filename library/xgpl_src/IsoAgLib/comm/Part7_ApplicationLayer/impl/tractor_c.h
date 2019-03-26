/*
  tractor_c.h:

  (C) Copyright 2016 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACTOR_C_H
#define TRACTOR_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorcommonrx_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib {

  class IdentItem_c;


  class Tractor_c : public TractorCommonRx_c
  {
  public:

    void init();
    void close();


    //! Register implement for MAINTAIN_POWER messages,
    //! including Park, Work, Transport states
    //! @return success - only one implement can be registered!
    bool registerImplement( IdentItem_c& ident );


    /// SPEED & DISTANCE
    ///////////////////

    /** is looking for a valid speed value
        @return if speed is valid
      */
    inline bool isSpeedUsable() const;

    /** only use this value if "isSpeedUsable()" returns true!
        @return absolute speed value [mm/s]
      */
    uint16_t speed() const { return mui16_speed; }

    /** is looking for a valid distance value
        @return if distance is valid
      */
    inline bool isDistanceUsable() const;

    /** @return distance value [mm]
      */
    uint32_t distance() const { return mui32_distance; }

    IsoAgLib::IsoOperatorDirectionFlag_t operatorDirection() const { return mt_operatorDirection; }
    IsoAgLib::IsoActiveFlag_t masterSwitch() const { return mt_masterSwitch; }
    IsoAgLib::IsoActiveFlag_t keySwitch() const { return mt_keySwitch; }
    IsoAgLib::IsoDirectionFlag_t direction() { return mt_direction; }

    
    // POWER MANAGEMENT & STATES
    ///////////////////////////

    void maintainPowerAndSendImplStates( IsoAgLib::IsoActiveFlag_t ecuPower, IsoAgLib::IsoActiveFlag_t actuatorPower );

    void setImplTransportState(   IsoAgLib::IsoImplTransportFlag_t   val ) { m_implTransportFlag   = val; }
    void setImplParkState(        IsoAgLib::IsoImplParkFlag_t        val ) { m_implParkFlag        = val; }
    void setImplReadyToWorkState( IsoAgLib::IsoImplReadyToWorkFlag_t val ) { m_implReadyToWorkFlag = val; }
    void setImplInWorkState(      IsoAgLib::IsoImplInWorkFlag_t      val ) { m_implInWorkFlag      = val; }

    IsoAgLib::IsoImplTransportFlag_t   getImplTransportState() const   { return m_implTransportFlag; }
    IsoAgLib::IsoImplParkFlag_t        getImplParkState() const        { return m_implParkFlag; }
    IsoAgLib::IsoImplReadyToWorkFlag_t getImplReadyToWorkState() const { return m_implReadyToWorkFlag; }
    IsoAgLib::IsoImplInWorkFlag_t      getImplInWorkState() const      { return m_implInWorkFlag; }


  private:
    Tractor_c() : TractorCommonRx_c( TractorCommonRx_c::TIMEOUT_SENDING_NODE_J1939, WHEEL_BASED_SPEED_DIST_PGN ), m_pgnRequest( *this ) { /* all done at init() */ }

    virtual void setValues( const CanPkgExt_c& arc_data );
    virtual void resetValues();

    void sendMaintainPower();

    class IsoRequestPgnHandlerProxy_c : public IsoRequestPgnHandler_c {
    public:
      typedef Tractor_c Owner_t;

      IsoRequestPgnHandlerProxy_c( Owner_t &owner ) : mrt_owner( owner ) {}
      virtual ~IsoRequestPgnHandlerProxy_c() {}

      void init();
      void close();

    private:
      virtual bool processMsgRequestPGN( uint32_t pgn, IsoItem_c *sender, IsoItem_c *receiver, ecutime_t time );

      IsoRequestPgnHandlerProxy_c(IsoRequestPgnHandlerProxy_c const &); // not copyable!
      IsoRequestPgnHandlerProxy_c &operator=(IsoRequestPgnHandlerProxy_c const &); // not assignable!

      Owner_t &mrt_owner;
    };

  private:
    /// Reception
    uint16_t mui16_speed;
    uint32_t mui32_distance;
    
    uint8_t mui8_maxPowerTime; // in minutes
    
    IsoAgLib::IsoOperatorDirectionFlag_t mt_operatorDirection;
    IsoAgLib::IsoActiveFlag_t mt_masterSwitch;
    IsoAgLib::IsoActiveFlag_t mt_keySwitch;
    IsoAgLib::IsoDirectionFlag_t mt_direction;

    // Sending
    IdentItem_c* m_ident;
 
    IsoAgLib::IsoActiveFlag_t mt_maintainEcuPower;
    IsoAgLib::IsoActiveFlag_t mt_maintainActuatorPower;
    
    IsoAgLib::IsoImplTransportFlag_t   m_implTransportFlag;
    IsoAgLib::IsoImplParkFlag_t        m_implParkFlag;
    IsoAgLib::IsoImplReadyToWorkFlag_t m_implReadyToWorkFlag;
    IsoAgLib::IsoImplInWorkFlag_t      m_implInWorkFlag;

    IsoRequestPgnHandlerProxy_c m_pgnRequest;

    friend Tractor_c &getTractorInstance( unsigned instance );
  };


  Tractor_c &getTractorInstance( unsigned instance );


  inline bool
  Tractor_c::isSpeedUsable() const
  {
    return( mui16_speed <= MAX_VAL_16 );
  }


  inline bool
  Tractor_c::isDistanceUsable() const
  {
    return( mui32_distance <= MAX_VAL_32 );
  }

}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif
