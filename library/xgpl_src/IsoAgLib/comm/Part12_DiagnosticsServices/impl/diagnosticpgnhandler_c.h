/*
  diagnosticpgnhandler_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef DIAGNOSTIC_PGN_HANDLER_C_H
#define DIAGNOSTIC_PGN_HANDLER_C_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>

#include <string>

namespace IsoAgLib
{

enum CertificationRevision_t
{
  CertificationRevisionFirstRevision = 0,
  CertificationRevisionSecondRevision = 1,
  CertificationRevisionThirdRevision = 2,
  CertificationRevisionFourthRevision = 3,
  CertificationRevisionNotAvailable = 7
};

enum CertificationLabType_t
{
  CertificationLabTypeNonCertifiedLab = 0,
  CertificationLabTypeEUCertifiedLab = 1,
  CertificationLabTypeNACertifiedLab = 2,
  CertificationLabTypeNotAvailable = 7
};

enum Certification_t
{
  CertificationMinEcu = 0,
  CertificationTecuClass_1,
  CertificationTecuClass_2,
  CertificationTecuClass_3,
  CertificationClass_3_Ecu,
  CertificationVirtualTerminal,
  CertificationVtWsMaster,
  CertificationVtWsMember,
  CertificationTaskController,
  CertificationTcWsMaster,
  CertificationTcWsMember,
  CertificationFileServer,
  CertificationGpsReceiver
};

struct Certification_s
{
  typedef Certification_t enum_type;
  enum { number_of_bits = 24 };
};

typedef BitFieldWrapper_c<Certification_s> CertificationBitMask_t;
} // namespace IsoAgLib


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class IdentItem_c;


/**
  This class implements the handling of Diagnostic
  requests for a given IdentItem_c instance

  @author Nizar Souissi
  @author Martin Wodok
*/
class DiagnosticPgnHandler_c : public IsoRequestPgnHandler_c
{

public:
  DiagnosticPgnHandler_c (IdentItem_c&);
  virtual ~DiagnosticPgnHandler_c();

  void init();
  void close();

  virtual bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/);


  bool setEcuIdentification( const STL_NAMESPACE::string& astr_partNr, const STL_NAMESPACE::string& astr_serialNr, const STL_NAMESPACE::string& astr_location, const STL_NAMESPACE::string& astr_type, const STL_NAMESPACE::string& astr_manufacturerName );

  bool setSwIdentification( const STL_NAMESPACE::string& astr_swIdentification );

  //! Setter for the different certification message fields
  //! Parameter:
  //! @param ui16_year Certification year as in ISO 11783-7 A.29.1, must be between 2000 and 2061 
  //! @param a_revision Certification revision as in ISO 11783-7 A.29.2
  //! @param a_laboratoryType Certification laboratory type as in ISO 11783-7 A.29.3
  //! @param aui16_laboratoryId Certification laboratory ID (11 bits wide) as in ISO 11783-7 A.29.4
  //! @param acrc_certificationBitMask Compliance certification type bitfield ( as in ISO 11783-7 A.29.5 till A.29.17 )
  //! @param aui16_referenceNumber Compliance certification reference number ( as in ISO 11783-7 A.29.18 )
  bool setCertificationData(
    uint16_t ui16_year, IsoAgLib::CertificationRevision_t a_revision,
    IsoAgLib::CertificationLabType_t a_laboratoryType, uint16_t aui16_laboratoryId,
    const IsoAgLib::CertificationBitMask_t& acrc_certificationBitMask, uint16_t aui16_referenceNumber );

private: // attributes
  IdentItem_c& mrc_identItem;

  STL_NAMESPACE::string mstr_EcuIdentification;
  STL_NAMESPACE::string mstr_SwIdentification;
  bool mb_certificationIsSet;
  uint8_t m_certification[8];
};

} // namespace __IsoAgLib
#endif
