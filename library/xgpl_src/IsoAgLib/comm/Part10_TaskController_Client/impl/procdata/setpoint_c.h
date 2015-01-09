/*
  setpoint_c.h

  (C) Copyright 2009 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_C_H
#define SETPOINT_C_H

#include <IsoAgLib/isoaglib_config.h>

namespace __IsoAgLib
{
  class PdLocal_c;

  
  class SetpointHandler_c
  {
    public:
      virtual ~SetpointHandler_c() {}
      virtual void _processSetpointSet( PdLocal_c &, int32_t value, bool change ) = 0;
  };


  class Setpoint_c
  {
  public:
    Setpoint_c();
    Setpoint_c( SetpointHandler_c *, bool settable );
    ~Setpoint_c() {}

    void init( SetpointHandler_c *, bool settable );

    int32_t setpointValue() const;
    bool isSettable() const;

    void processMsg( PdLocal_c &, int32_t pdValue );

  private:
    int32_t m_value;

    SetpointHandler_c *m_handler;
    bool m_settable;

    // not copyable
    Setpoint_c( const Setpoint_c& );
    Setpoint_c& operator=( const Setpoint_c& );
  };


  inline int32_t
  Setpoint_c::setpointValue() const
  {
    return m_value;
  }


  inline bool
  Setpoint_c::isSettable() const
  {
    return m_settable;
  }

}

#endif
