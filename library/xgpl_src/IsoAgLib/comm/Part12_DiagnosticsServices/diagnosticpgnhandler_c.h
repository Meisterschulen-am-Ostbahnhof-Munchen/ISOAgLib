/***************************************************************************
                            DiagnosticPGNHandler_c.h
                             -------------------
    begin                : Thu May 16 2009
    copyright            : (C) 2009 by Dipl.-Ing. Nizar Souissi
    email                : n.souissi@osb-ag:de
    type                 : Header
    $LastChangedDate:  $
    $LastChangedRevision:  $
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

#ifndef DIAGNOSTIC_PGN_HANDLER_C_H
#define DIAGNOSTIC_PGN_HANDLER_C_H

#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>

#include <string>

namespace __IsoAgLib {

	
	enum Certification_t
	{
		CertificationMinEcu = 0
		, CertificationTecuClass_1
		, CertificationTecuClass_2
		, CertificationTecuClass_3 
		, CertificationClass_3_Ecu
		, CertificationVirtualTerminal
		, CertificationVtWsMaster
		, CertificationVtWsMember
		, CertificationTaskController
		, CertificationTcWsMaster
		, CertificationTcWsMember
		, CertificationFileServer
		, CertificationGpsReceiver
	};
	
	typedef BitFieldWrapper_c<Certification_t> CertificationBitMask_t;

	enum CertificationRevision_t
	{
		CertificationRevisionFirstRevision = 0,
		CertificationRevisionSecondRevision = 1,
		CertificationRevisionThirdRevision = 2,
		CertificationRevisionFourthRevision = 3,
		CertificationRevisionNotAvailable =7
	};

	enum CertificationLabType_t
	{
		CertificationLabTypeNonCertifiedLab = 0,
		CertificationLabTypeEUCertifiedLab = 1,
		CertificationLabTypeNACertifiedLab = 2,
		CertificationLabTypeNotAvailable = 7
	};
	

	class IdentItem_c;

/**
  This class implements the handling of Diagnostic requests

  @author Nizar Souissi
*/
class DiagnosticPgnHandler_c: public IsoRequestPgnHandler_c
{

public:
  DiagnosticPgnHandler_c(IdentItem_c & );
  virtual ~DiagnosticPgnHandler_c();

  virtual bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/);

  
  bool setEcuIdentification( const STL_NAMESPACE::string& astr_partNr, const STL_NAMESPACE::string& astr_serialNr,const STL_NAMESPACE::string& astr_manufacturerName );

  bool setSwIdentification( const STL_NAMESPACE::string& astr_swIdentification );

  //! Setter for the different certification message fields 
  //! Parameter:
  //! @param ui16_year Certification year as in ISO 11783-7 A.29.1, must be between 2000 and 2061 
  //! @param a_revision Certification revision as in ISO 11783-7 A.29.2
  //! @param a_laboratoryType Certification laboratory type as in ISO 11783-7 A.29.3
  //! @param aui16_laboratoryId Certification laboratory ID (11 bits wide) as in ISO 11783-7 A.29.4
  //! @param acrc_certificationBitMask Compliance certification type bitfield ( as in ISO 11783-7 A.29.5 till A.29.17 )
  //! @param aui16_referenceNumber Compliance certification reference number ( as in ISO 11783-7 A.29.18 )
  bool setCertificationData( uint16_t ui16_year ,CertificationRevision_t a_revision,CertificationLabType_t a_laboratoryType, uint16_t aui16_laboratoryId,
                             const CertificationBitMask_t& acrc_certificationBitMask, uint16_t aui16_referenceNumber );
  
private:
  IdentItem_c& mrc_identItem;

  STL_NAMESPACE::string mstr_EcuIdentification;
  STL_NAMESPACE::string mstr_SwIdentification;
  bool mb_certificationIsSet;
  uint8_t m_certification[8];	
};

}
#endif



