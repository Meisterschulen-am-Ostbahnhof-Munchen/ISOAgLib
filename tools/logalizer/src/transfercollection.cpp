/*
  transfercollection.cpp

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include <logenvirons.h>
#include <transfercollection.h>


TransferCollection_c::PtrConnection_t
TransferCollection_c::newConnection(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress,
    size_t at_sizeTransferData)
{
  PtrConnection_t t_ptrConnection = new Connection_s(at_sizeTransferData);
  t_transfers[
      Key_s(
          ae_variant,
          aui8_transferSourceAddress,
          aui8_transferDestinationAddress)
    ] = t_ptrConnection;
  return t_ptrConnection;
}


TransferCollection_c::PtrConnection_t
TransferCollection_c::getConnection(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress)
{
  Transfers_t::iterator it_transfer =
    t_transfers.find(
        Key_s(
            ae_variant,
            aui8_transferSourceAddress,
            aui8_transferDestinationAddress));
  return
    it_transfer == t_transfers.end() ?
    TransferCollection_c::PtrConnection_t(0) :
    it_transfer->second;
}


void
TransferCollection_c::deleteConnection(
    Variant_e ae_variant,
    uint8_t aui8_transferSourceAddress,
    uint8_t aui8_transferDestinationAddress)
{
  t_transfers.erase(
      Key_s(
          ae_variant,
          aui8_transferSourceAddress,
          aui8_transferDestinationAddress) );
}


bool
operator<(
    TransferCollection_c::Key_s const &arcs_left,
    TransferCollection_c::Key_s const &arcs_right)
{
    const uint32_t combinedValueLeft  = (arcs_left .me_variant << 2 * 8) | (arcs_left. mui8_transferSourceAddress << 8) | (arcs_left. mui8_transferDestinationAddress);
    const uint32_t combinedValueRight = (arcs_right.me_variant << 2 * 8) | (arcs_right.mui8_transferSourceAddress << 8) | (arcs_right.mui8_transferDestinationAddress);

    return (combinedValueLeft < combinedValueRight);
}
