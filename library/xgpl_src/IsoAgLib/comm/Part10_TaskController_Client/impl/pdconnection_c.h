/*
  pdconnection_c.h: base class for managing a PD-based connection

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PDCONNECTION_C_H
#define PDCONNECTION_C_H

#ifdef HAL_USE_SPECIFIC_FILTERS
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

  enum NackResponse_t { 
    NackNoErrors =                                  0x00, // 0 = no errors
    NackProcessDataCommandNotSupported =            0x01, // Bit 1 = 1 Process Data Command not supported
    NackInvalidElementNumber =                      0x02, // Bit 2 = 1 Invalid Element Number
    NackDDINotSupportedByElement =                  0x04, // Bit 3 = 1 DDI not supported by element
    NackTriggerMethodNotSupported =                 0x08, // Bit 4 = 1 Trigger method not supported
    NackProcessDataNotSettable =                    0x10, // Bit 5 = 1 Process Data not settable
    NackInvalidOrUnsupportedIntervalOrThreshold =   0x20, // Bit 6 = 1 Invalid or unsupported interval or threshold
    NackProcessDataValueNotConformToDdiDefinition = 0x40, // Bit 7 = 1 Process data value does not conform to DDI definition (TCv4+)
    NackProcessDataValueIsOutsideOperationalRange = 0x80, // Bit 8 = 1 Process data value is outside the operational range of this device (TCv4+)
  };

  class PdConnection_c
#ifdef HAL_USE_SPECIFIC_FILTERS
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

    const IdentItem_c&     getIdentItem() const { return *m_identItem; }
    const PdRemoteNode_c* getRemoteNode() const { return m_pdRemoteNode; }
    const IsoItem_c*      getRemoteItem() const;
  
    int getMultitonInst() const { return m_identItem->getMultitonInst(); }

    virtual void processProcMsg( const ProcessPkg_c& );

    void sendProcMsg( IsoAgLib::ProcData::CommandType_t, uint16_t ddi, uint16_t element, int32_t pdValue ) const;
    void sendNack( int16_t ddi, int16_t element, NackResponse_t errorcodes, bool wasBroadcast ) const;

  protected:
    void sendNackNotFound( int16_t ddi, int16_t element, bool wasBroadcast ) const;

  private:
#ifdef HAL_USE_SPECIFIC_FILTERS
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

    // Measure progs presorted for DDIs
    static uint32_t getMapKey(uint16_t ddi, uint16_t element) { return ( uint32_t( uint32_t( ddi ) << 16 ) ) | uint32_t(element); }

    typedef STL_NAMESPACE::map<uint32_t, ConnectedPd_c*> ConnectedPdMap_t;
    ConnectedPdMap_t m_connectedPds;
  };

}

#endif
