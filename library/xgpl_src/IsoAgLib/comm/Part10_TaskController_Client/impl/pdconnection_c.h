/*
  pdconnection_c.h: base class for managing a PD-based connection

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
#ifndef PDCONNECTION_C_H
#define PDCONNECTION_C_H

#if defined(HAL_USE_SPECIFIC_FILTERS) && !defined(USE_DIRECT_PD_HANDLING)
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#endif
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <map>


namespace __IsoAgLib
{

  class ConnectedPd_c;
  class ProcessPkg_c;
  class PdRemoteNode_c;
  class PdPool_c;

  class PdConnection_c
#if defined(HAL_USE_SPECIFIC_FILTERS) && !defined(USE_DIRECT_PD_HANDLING)
    : public CanCustomer_c
#endif
  {
  public:
    PdConnection_c();
    PdConnection_c( const IdentItem_c& identItem, PdRemoteNode_c* remoteNode, PdPool_c &pool );
    virtual ~PdConnection_c();

    void init( const IdentItem_c &identItem, PdRemoteNode_c *remoteNode );
    void close();

    void start( PdPool_c &pool );
    void stop();

    //! can be set or unset. only one possible at a time!
    void setNackHandler( IsoAgLib::ProcData::iNackHandler_c* handler ) { m_nackHandler = handler; }

    const IdentItem_c&     getIdentItem() const { return *m_identItem; }
    const PdRemoteNode_c* getRemoteNode() const { return m_pdRemoteNode; }
    const IsoItem_c*      getRemoteItem() const;
  
    int getMultitonInst() const { return m_identItem->getMultitonInst(); }

    virtual void processProcMsg( const ProcessPkg_c& );

    void processPdAck(const ProcessPkg_c&) const;

    void sendProcMsg(     IsoAgLib::ProcData::CommandType_t, uint16_t ddi, uint16_t element, int32_t pdValue ) const;
    void sendProcMsgPeer( IsoAgLib::ProcData::CommandType_t, uint16_t ddi, uint16_t element, int32_t pdValue, const IsoName_c &peer ) const;

    void sendPdAck(int16_t ddi, int16_t element, IsoAgLib::ProcData::CommandType_t, IsoAgLib::ProcData::AckResponse_t errorcodes, bool wasBroadcast) const;

  protected:
    void sendNackNotFound( int16_t ddi, int16_t element, IsoAgLib::ProcData::CommandType_t, bool wasBroadcast ) const;

  private:
#if defined(HAL_USE_SPECIFIC_FILTERS) && !defined(USE_DIRECT_PD_HANDLING)
    virtual void processMsg( const CanPkg_c& data );
#endif
    void processMeasurementMsg( const ProcessPkg_c& );
    void processRequestMsg( const ProcessPkg_c& );
    virtual void processRequestDefaultDataLogging() {}
    void processSetMsg( const ProcessPkg_c& );

    void createMeasureProgs();
    void destroyMeasureProgs();

    void startOnChangeMeasurements();

  protected:
    const IdentItem_c* m_identItem;
    PdRemoteNode_c* m_pdRemoteNode;

    PdPool_c* m_pool;
    IsoAgLib::ProcData::ConnectionCapabilities_s m_capsConnection; // initialized along with server caps, or set to defaults.

    // Measure progs presorted for DDIs
    static uint32_t getMapKey(uint16_t ddi, uint16_t element) { return ( uint32_t( uint32_t( ddi ) << 16 ) ) | uint32_t(element); }

    typedef STL_NAMESPACE::map<uint32_t, ConnectedPd_c*> ConnectedPdMap_t;
    ConnectedPdMap_t m_connectedPds;

    IsoAgLib::ProcData::iNackHandler_c* m_nackHandler;
  };

}

#endif
