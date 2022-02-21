/*
  vtclient_c.h: central ISO terminal management

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
#ifndef VTCLIENT_C_H
#define VTCLIENT_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/multiton.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientobjectpool_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include "vtserverinstance_c.h"
#include "vtservermanager_c.h"
#include "vtclientconnection_c.h"

#include <list>


namespace __IsoAgLib {

class VtClientConnection_c;
class iIdentItem_c;

/** central IsoAgLib Virtual Terminal-Client management object */
class VtClient_c : private Subsystem_c {
public:
    multiton getMultitonInst() const { return mi_multitonInst; }
    void setMultitonInst(multiton ai_instance) { mi_multitonInst = ai_instance; }
private:
    multiton mi_multitonInst;
public:
  virtual ~VtClient_c() = default;

  void init();
  void close();

  VtClientConnection_c* initAndRegisterObjectPool(
    IdentItem_c& apc_wsMasterIdentItem, 
    IsoAgLib::iVtClientObjectPool_c& arc_pool, 
    const char* apc_versionLabel, 
    IsoAgLib::iVtClientDataStorage_c& apc_claimDataStorage, 
    IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_en aen_mode );

  bool deregisterObjectPool (IdentItem_c& apc_wsMasterIdentItem);

  /** function that handles incoming can messages */
  virtual void processMsg( const CanPkg_c& arc_data );
  void processMsgNonGlobal( const CanPkgExt_c& arc_data );
  void processMsgGlobal( const CanPkgExt_c& arc_data );

  bool sendCommandForDEBUG(IsoAgLib::iIdentItem_c& apc_wsMasterIdentItem, uint8_t* apui8_buffer, uint32_t ui32_size);

  VtClientConnection_c& getClientByID (uint8_t ui8_clientIndex);
  VtClientConnection_c* getClientPtrByID (uint8_t ui8_clientIndex);

    virtual bool isAnyVtAvailable() const;
  // is any claimed VT sending VT status
  bool isAnyVtActive( bool mustBePrimary ) const;
  uint16_t getActiveVtCount() const;

  void notifyAllConnectionsOnAux1InputStatus( const CanPkgExt_c& refc_data ) const;
  void notifyAllConnectionsOnAux2InputStatus( const CanPkgExt_c& refc_data ) const;
  void notifyAllConnectionsOnAux2InputMaintenance( const CanPkgExt_c& refc_data ) const;
  VtServerInstance_c* getActiveVtServer( bool mustBePrimary, const VtServerInstance_c* ap_searchStart ) const;
  VtServerInstance_c* getPreferredVtServer(const IsoName_c& aref_prefferedVTIsoName) const;
  VtServerInstance_c* getSpecificVtServer(const IsoAgLib::iVtClientObjectPool_c& arc_pool) const;

  ////////////////////////
  // INTERFACE FUNCTIONS //
  ////////////////////////
// the following define should be globally defined in the project settings...
// (currently not supported, due to multi VT enhancements)
#if 0
#ifdef USE_IOP_GENERATOR_FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes)
  { m_serverManager.fakeVtProperties(aui16_dimension, aui16_skWidth, aui16_skHeight, aui16_colorDepth, aui16_fontSizes, *this); }
#endif
#endif

private:
  uint16_t getClientCount() const;

  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef VtClient_c Owner_t;

    explicit CanCustomerProxy_c(Owner_t &art_owner);

     virtual ~CanCustomerProxy_c() override;

  private:
    void processMsg( const CanPkg_c& arc_data ) override;

    bool reactOnStreamStart(
        ReceiveStreamIdentifier_c const &ac_ident,
        uint32_t aui32_totalLen) override;

    void reactOnAbort(Stream_c &arc_stream) override;

    bool processPartStreamDataChunk(
        Stream_c &apc_stream,
        bool ab_isFirstChunk,
        bool ab_isLastChunk) override;

    void notificationOnMultiReceiveError(
        ReceiveStreamIdentifier_c const &ac_streamIdent,
        uint8_t aui8_multiReceiveError,
        bool ab_isGlobal) override;

    // CanCustomerProxy_c shall not be copyable. Otherwise, the
    // reference to the containing object would become invalid.
    CanCustomerProxy_c(CanCustomerProxy_c const &);

    CanCustomerProxy_c &operator=(CanCustomerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef CanCustomerProxy_c Customer_t;
  class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
  public:
    typedef VtClient_c Owner_t;

    explicit ControlFunctionStateHandlerProxy_c(Owner_t &art_owner);

     virtual ~ControlFunctionStateHandlerProxy_c() override;

  private:
    void reactOnIsoItemModification(
        iIsoItemAction_e at_action,
        IsoItem_c const &acrc_isoItem) override;

    // ControlFunctionStateHandlerProxy_c shall not be copyable. Otherwise, the
    // reference to the containing object would become invalid.
    ControlFunctionStateHandlerProxy_c(ControlFunctionStateHandlerProxy_c const &);

    ControlFunctionStateHandlerProxy_c &operator=(ControlFunctionStateHandlerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef ControlFunctionStateHandlerProxy_c Handler_t;

  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of VtClient_c within application
    */
  VtClient_c();

  void reactOnIsoItemModification (ControlFunctionStateHandler_c::iIsoItemAction_e at_action, IsoItem_c const& acrc_isoItem);

  virtual bool reactOnStreamStart(
      ReceiveStreamIdentifier_c const &ac_ident,
      uint32_t aui32_totalLen);

  virtual void reactOnAbort(Stream_c &arc_stream);

  virtual bool processPartStreamDataChunk(
      Stream_c &apc_stream,
      bool ab_isFirstChunk,
      bool ab_isLastChunk);

  virtual void notificationOnMultiReceiveError(
      ReceiveStreamIdentifier_c const &ac_streamIdent,
      uint8_t aui8_multiReceiveError,
      bool ab_isGlobal);


  VtClientConnection_c* initAndRegisterObjectPoolCommon(
    IdentItem_c& rc_identItem, 
    IsoAgLib::iVtClientObjectPool_c& arc_pool, 
    const char* apc_versionLabel, 
    IsoAgLib::iVtClientDataStorage_c& apc_claimDataStorage, 
    IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_en aen_mode );

private:
  std::vector<VtClientConnection_c*> m_vtConnections;

  Handler_t mt_handler;

  VtServerManager_c m_serverManager;

public:
  // so that VtClientConnection can insert filters to us.
  Customer_t mt_customer;

  friend VtClient_c &getVtClientInstance( unsigned instance );
};


/** C-style function, to get access to the unique multiton instance */
VtClient_c &getVtClientInstance( unsigned instance );

}

#endif
