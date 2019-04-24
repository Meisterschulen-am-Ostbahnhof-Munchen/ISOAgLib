/*
  itractorlighting_c.h: interface for retrieving lighting commands and
                        sending lighting data.

  (C) Copyright 2017 - 2019 by OSB AG

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
#ifndef ITRACTORLIGHTING_C_H
#define ITRACTORLIGHTING_C_H

#include "impl/tractorlighting_c.h"

namespace IsoAgLib {

  class iTractorLighting_c : private __IsoAgLib::TractorLighting_c
  {
  public:
    //! Register implement for LIGHTING_DATA messages,
    //! @return success - only one implement can be registered!
    bool registerImplement( iIdentItem_c& ident ) { return TractorLighting_c::registerImplement( static_cast<__IsoAgLib::IdentItem_c&>( ident ) ); }

    /** register an event handler that gets called when receiving a LIGHTING COMMAND.
      In this case the application needs to
	   - Update the Lighting Data (if it changes according to the command)
	   - Send out the Lighting Data using sendData() in case the command told so
	     (check "dataMsgReq" for this!)
     */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorLighting_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorLighting_c::deregisterMsgEventHandler( msgEventHandler ); }

	//! Retrieve the command when being notified using the iMsgEventHandler_c
    const IsoAgLib::iLighting_t& getCommand() { return TractorLighting_c::getCommand(); }
	
	//! Retrieve the last sent data.
    const IsoAgLib::iLighting_t& getData() { return TractorLighting_c::getData(); }
	
	//! Set the current Data.
    void setData( const IsoAgLib::iLighting_t& lighting ) { TractorLighting_c::setData( lighting ); }
	
	//! Send out the current Data - typically as a reaction to the Lighting Command's request
    void sendData() { TractorLighting_c::sendData(); }
	
private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractorLighting_c& getITractorLightingInstance();
#endif
    friend iTractorLighting_c& getITractorLightingInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractorLighting_c& getITractorLightingInstance()
  { return static_cast<iTractorLighting_c&>(__IsoAgLib::getTractorLightingInstance( 0 )); }
#endif
  inline iTractorLighting_c& getITractorLightingInstance( unsigned instance )
  { return static_cast<iTractorLighting_c&>(__IsoAgLib::getTractorLightingInstance( instance )); }

}
#endif
