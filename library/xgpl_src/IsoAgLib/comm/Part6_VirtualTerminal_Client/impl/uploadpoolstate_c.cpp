/*
  uploadpoolstate_c.cpp: 

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "uploadpoolstate_c.h"
#include <IsoAgLib/comm/Part3_DataLink/impl/stream_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtclientconnection_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtserverinstance_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientobjectpool_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectworkingset_c.h>
#include <IsoAgLib/util/iliberr_c.h>

// Some old GS2 will force us to run into a time-out, so we need to continue in case of a time-out!
#define DEF_TimeOut_GetVersions 6000

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib
{

UploadPoolState_c::UploadPoolState_c(
    VtClientConnection_c &connection,
    IsoAgLib::iVtClientObjectPool_c& pool,
    const char *versionLabel,
    bool wsMaster )
  : m_connection( connection )
  , m_pool( pool )
  , mb_usingVersionLabel( false )
  //marrp7c_versionLabel[ 7 ] will be initialized below
  , m_uploadingVersion( 0 )
  , mc_iVtObjectStreamer( *this )
  , men_uploadPoolState (UploadPoolInit) // dummy init value, will be set when VT (re)enters to UploadInit anyway
  , men_uploadPoolType (UploadPoolTypeCompleteInitially) // dummy init value, will be set when (up)load is started
  , mui32_uploadTimestamp( 0 )
  , mui32_uploadTimeout( 0 ) // will be set when needed
  //ms_uploadPhasesAutomatic[..] // will be corrently initialized in the body!
  , mui_uploadPhaseAutomatic (UploadPhaseIVtObjectsFix)
  , ms_uploadPhaseUser() // will be corrently initialized in the body!
  , mppc_uploadPhaseUserObjects (NULL)
  , mi8_objectPoolUploadingLanguage(0) // only valid if "initially uploading" or "language updating"
  , mi8_objectPoolUploadedLanguage(0) // only valid if "ObjectPoolUploadedSuccessfully"
  , mui16_objectPoolUploadingLanguageCode (0x0000)
  , mui16_objectPoolUploadedLanguageCode (0x0000)
  , mi8_vtLanguage (-2)
{
  if( versionLabel )
  {
    const uint32_t cui_len = CNAMESPACE::strlen( versionLabel );
    isoaglib_assert( ! ( ( (m_pool.getNumLang() == 0) && (cui_len > 7) ) || ( (m_pool.getNumLang()  > 0) && (cui_len > 5) ) ) ); 
    unsigned int i=0;
    for (; i<cui_len; i++) marrp7c_versionLabel [i] = versionLabel[ i ];
    for (; i<7;       i++) marrp7c_versionLabel [i] = ' '; // ASCII: Space
    mb_usingVersionLabel = true;
  }

  if( wsMaster )
  {
    /// ms_uploadPhases
    ms_uploadPhasesAutomatic[0] = UploadPhase_s (&mc_iVtObjectStreamer, 0);
    ms_uploadPhasesAutomatic[1] = UploadPhase_s (&mc_iVtObjectStreamer, 0);
    ms_uploadPhaseUser = UploadPhase_s (&mc_iVtObjectStreamer, 0);
    //men_uploadPoolState = UploadPoolInit; // from c'tor
  }
  else
  {
    men_uploadPoolState = UploadPoolEndSuccess;
  }
}


bool
UploadPoolState_c::isVersionFound( Stream_c& stream ) const
{
  const uint8_t number_of_versions = stream.get();
 
  // check get versions response is consistency
  if( uint32_t(stream.getByteTotalSize()) != uint32_t(2 + 7*uint16_t(number_of_versions)) )
    return false; // not valid -> return;

  for( uint8_t counter = 0; counter < number_of_versions; ++counter )
  {
    char c_nextversion[ 7 ];
    for( uint16_t i = 0; i < 7; ++i )
      c_nextversion[i] = stream.get();

    if( 0 == CNAMESPACE::memcmp( c_nextversion, marrp7c_versionLabel, 7 ) )
      return true;
  }
  return false;
}


void
UploadPoolState_c::handleGetVersionsResponse( Stream_c * stream )
{
  if( men_uploadPoolState != UploadPoolWaitingForGetVersionsResponse )
    return;

  if( ( stream != NULL )
    && isVersionFound( *stream ) )
  {
    startLoadVersion();
  }
  else
  {
    startUploadVersion(); // Send out pool! send out "Get Technical Data - Get Memory Size", etc. etc.
  }
}


// This command is used for both:
// Getting VT's version and checking for available memory
void
UploadPoolState_c::handleGetMemoryResponse( const CanPkgExt_c &pkg )
{
  switch( men_uploadPoolState )
  {
  case UploadPoolWaitingForVtVersionResponse:
    m_connection.getVtServerInst().setVersion( pkg );

    // Special case when adapting a v3 pool to upload to v2 VTs, omitting all the new aux...
    m_uploadingVersion = (m_connection.getVtServerInst().getVtIsoVersion() == 2) && (m_pool.getVersion() == 3)
                           ? 2 : m_pool.getVersion();

#if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
    INTERNAL_DEBUG_DEVICE << "Upload pool as v" << (unsigned)m_uploadingVersion << " to a v" << (unsigned)m_connection.getVtServerInst().getVtIsoVersion() << " VT." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    
    // check for matching VT version and object pool version
    if (m_connection.getVtServerInst().getVtIsoVersion() < m_uploadingVersion)
      vtOutOfMemory();
    else
    {
      initObjectPoolUploadingPhases (UploadPoolTypeCompleteInitially);

      // Take the language-version that's been set up NOW and try to load/upload it.
      setObjectPoolUploadingLanguage();
      // Do we have to try to "get Versions / Load Version" or go directly to uploading?
      if (mb_usingVersionLabel)
      {
#ifndef NO_GET_VERSIONS
        startGetVersions();
#else
        startLoadVersion();
#endif
      }
      else
      {
#if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
        INTERNAL_DEBUG_DEVICE << "No Version Label : start uploading" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        startUploadVersion();
      }
    }
    break;

  case UploadPoolWaitingForMemoryResponse:
    if( pkg.getUint8Data( 2 ) == 0 )
    { // start uploading with all partial OPs (as init'd before Get Memory!), there MAY BE enough memory
      men_uploadPoolState = UploadPoolUploading;
    //men_uploadPhaseAutomatic [already initialized in "initObjectPoolUploadingPhases" to the correct starting phase]
      startCurrentUploadPhase();
    }
    else
      vtOutOfMemory();
    break;
  }
}


void
UploadPoolState_c::handleStoreVersionResponse( unsigned errorNibble )
{
  if( men_uploadPoolState != UploadPoolWaitingForStoreVersionResponse )
    return;

  switch( errorNibble )
  {
    case 0: // Successfully stored
    case 1: // Not used
    case 2: // Version label not known
    case 8: // General error
      break;
    case 4: // Insufficient memory available
    default: // well....
      IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::VtOutOfStorageSpace, m_connection.getMultitonInst() );
      break;
  }
  finalizeUploading();
}


void
UploadPoolState_c::handleLoadVersionResponse( unsigned errorNibble )
{
  if( men_uploadPoolState != UploadPoolWaitingForLoadVersionResponse )
    return;

  if( errorNibble == 0 )
  {
    finalizeUploading();
#if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
    INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) without error..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  }
  else
  {
    if( errorNibble & (1<<2) )
    { 
#if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
      INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) with error OutOfMem..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      vtOutOfMemory(); // Insufficient memory available
    }
    else
    { // Not used
      // General error
      // Version label not known -> upload the pool
      startUploadVersion(); // Send out pool! send out "Get Technical Data - Get Memory Size", etc. etc.
#if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
      INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) with VersionNotFound..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    }
  }
}

void
UploadPoolState_c::vtOutOfMemory()
{
  IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::VtOutOfMemory, m_connection.getMultitonInst() );
  men_uploadPoolState = UploadPoolEndFailed;
}


void
UploadPoolState_c::startUploadVersion()
{
  sendGetMemory( false );
}


void
UploadPoolState_c::handleEndOfObjectPoolResponse( bool success )
{
  if( men_uploadPoolState != UploadPoolWaitingForEOOResponse )
    return;

  if( success )
  {
// Added this preprocessor so storing of object pools can be prevented for development purposes
#ifndef DEBUG_VTCOMM_NO_STORE_VERSION
    if( mb_usingVersionLabel )
    { // Store Version and finalize after "Store Version Response"
      // Command: Non Volatile Memory --- Parameter: Store Version
      m_connection.sendMessage(
        208, marrp7c_versionLabel [0],
        marrp7c_versionLabel [1], marrp7c_versionLabel [2], marrp7c_versionLabel [3],
        marrp7c_versionLabel [4], marrp7c_versionLabel [5], marrp7c_versionLabel [6] );

      // Now wait for response
      men_uploadPoolState = UploadPoolWaitingForStoreVersionResponse;
    }
    else
#endif // DEBUG_VTCOMM_NO_STORE_VERSION
    { // Finalize now!
      finalizeUploading();
    }
  }
  else
    men_uploadPoolState = UploadPoolEndFailed;
}


void
UploadPoolState_c::sendGetMemory( bool requestVtVersion )
{ // Issue GetMemory-Command (don't care if several 0x11s are counted from each partial object pool...)
  uint32_t ui32_size = 0;
  if( requestVtVersion )
  {
    men_uploadPoolState = UploadPoolWaitingForVtVersionResponse;
  }
  else
  {
    men_uploadPoolState = UploadPoolWaitingForMemoryResponse;

    for (int i=0; i <= UploadPhaseLAST; ++i)
    {
      ui32_size += ms_uploadPhasesAutomatic[i].ui32_size;
    }
  }

  // Command: Get Technical Data --- Parameter: Get Memory Size
  m_connection.sendMessage(
    192 /* 0xC0 */, 0xff,
    (ui32_size) & 0xFF, (ui32_size >>  8) & 0xFF, (ui32_size >> 16) & 0xFF, ui32_size >> 24,
    0xff, 0xff);
}


void
UploadPoolState_c::startGetVersions()
{
  m_connection.sendMessage( 223, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF );

  men_uploadPoolState = UploadPoolWaitingForGetVersionsResponse;
  mui32_uploadTimeout = DEF_TimeOut_GetVersions;
  mui32_uploadTimestamp = HAL::getTime();
}


void
UploadPoolState_c::startLoadVersion()
{
  // Command: Non Volatile Memory --- Parameter: Load Version
  m_connection.sendMessage( 209, marrp7c_versionLabel [0], marrp7c_versionLabel [1], marrp7c_versionLabel [2], marrp7c_versionLabel [3], marrp7c_versionLabel [4], marrp7c_versionLabel[5], marrp7c_versionLabel[6] );

  men_uploadPoolState = UploadPoolWaitingForLoadVersionResponse;
  men_uploadPoolType = UploadPoolTypeCompleteInitially; // need to set this, so that eventObjectPoolUploadedSucessfully is getting called (also after load, not only after upload)
#if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
  INTERNAL_DEBUG_DEVICE << "Trying Load Version (D1) for Version ["<<marrp7c_versionLabel [0]<< marrp7c_versionLabel [1]<< marrp7c_versionLabel [2]<< marrp7c_versionLabel [3]<< marrp7c_versionLabel [4]<< marrp7c_versionLabel [5]<< marrp7c_versionLabel [6]<<"]..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
}


bool
UploadPoolState_c::dontUpload( const vtObject_c& object ) const
{
  return( object.isOmittedFromUpload()
       || ((m_uploadingVersion == 2) && (object.getObjectType() >= VT_OBJECT_TYPE_AUXILIARY_FUNCTION_2) && (object.getObjectType() <= VT_OBJECT_TYPE_AUXILIARY_POINTER) ) );
}


uint32_t
UploadPoolState_c::fitTerminalWrapper( const vtObject_c& object ) const
{
  return dontUpload( object ) ? 0 : object.fitTerminal();
}


bool
UploadPoolState_c::activeAuxO() const
{
  return( m_uploadingVersion == IsoAgLib::iVtClientObjectPool_c::ObjectPoolVersion2 );
}

bool
UploadPoolState_c::activeAuxN() const
{
  return( ( m_uploadingVersion != 0 ) &&
          ( m_uploadingVersion != IsoAgLib::iVtClientObjectPool_c::ObjectPoolVersion2 ) );
}


void
UploadPoolState_c::timeEvent()
{
  timeEventUploadPoolTimeoutCheck();

  // Do we have to request (any) vt capabilities?
  if( !(m_connection.getVtServerInst().getVtCapabilities()->lastReceivedFont
     && m_connection.getVtServerInst().getVtCapabilities()->lastReceivedHardware
     && m_connection.getVtServerInst().getVtCapabilities()->lastReceivedSoftkeys) )
  { /// Pool-Upload: PRE Phase (Get VT-Properties)
    timeEventPrePoolUpload();
  }
  else
  { /// Handle other than Response states now here: INITIALIZING-States!
    timeEventPoolUpload();
  }
}

void
UploadPoolState_c::timeEventUploadPoolTimeoutCheck()
{
  // There are normally no time-out checks
  // as the VT has to respond!
  // The Get Versions time-out is only for VTs that do
  // incorrectly answer with a DLC < 8 and hence we don't
  // see that answer.
  // Should be removable in the future if all VTs do properly answer with DLC 8
  switch( men_uploadPoolState )
  {
  case UploadPoolWaitingForGetVersionsResponse:
    if (((uint32_t) HAL::getTime()) > (mui32_uploadTimeout + mui32_uploadTimestamp))
    {
#if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
      INTERNAL_DEBUG_DEVICE << "Version couldn't be checked (GVResp missing/short DLC) -> Upload pool" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      startUploadVersion(); // Send out pool! send out "Get Technical Data - Get Memory Size", etc. etc.
    }
    break;

  default:
    ; // all others are fine
  }
}


void
UploadPoolState_c::timeEventPrePoolUpload()
{
  VtServerInstance_c &server = m_connection.getVtServerInst();

  /// first you have to get number of softkeys, text font data and hardware before you could upload
  if (!server.getVtCapabilities()->lastReceivedSoftkeys
       && ((server.getVtCapabilities()->lastRequestedSoftkeys == 0)
       || ((HAL::getTime() - server.getVtCapabilities()->lastRequestedSoftkeys) > 1000)))
  { // Command: Get Technical Data --- Parameter: Get Number Of Soft Keys
    m_connection.sendMessage( 194, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff );
    server.getVtCapabilities()->lastRequestedSoftkeys = HAL::getTime();
  }

  if (server.getVtCapabilities()->lastReceivedSoftkeys
      && (!server.getVtCapabilities()->lastReceivedFont)
      && ((server.getVtCapabilities()->lastRequestedFont == 0) || ((HAL::getTime() - server.getVtCapabilities()->lastRequestedFont) > 1000)))
  { // Command: Get Technical Data --- Parameter: Get Text Font Data
    m_connection.sendMessage( 195, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff );
    server.getVtCapabilities()->lastRequestedFont = HAL::getTime();
  }

  if (server.getVtCapabilities()->lastReceivedSoftkeys
      && server.getVtCapabilities()->lastReceivedFont
      && (!server.getVtCapabilities()->lastReceivedHardware)
      && ((server.getVtCapabilities()->lastRequestedHardware == 0)
      || ((HAL::getTime() - server.getVtCapabilities()->lastRequestedHardware) > 1000)))
  { // Command: Get Technical Data --- Parameter: Get Hardware
    m_connection.sendMessage( 199, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff );
    server.getVtCapabilities()->lastRequestedHardware = HAL::getTime();
  }
}


void
UploadPoolState_c::timeEventPoolUpload()
{
  if( men_uploadPoolState == UploadPoolInit )
    sendGetMemory( true );
}

//! Handle language Update as a command, not upload!
void
UploadPoolState_c::timeEventLanguageUpdate()
{
  if( (mi8_objectPoolUploadingLanguage == -2) // indicates no update running
   && (mi8_vtLanguage != mi8_objectPoolUploadedLanguage) )
  { // update languages on the fly
    setObjectPoolUploadingLanguage();
    /// NOTIFY THE APPLICATION so it can enqueue some commands that are processed BEFORE the update is done
    /// e.g. switch to a "Wait while changing language..." datamask.
    m_pool.eventPrepareForLanguageChange (mi8_objectPoolUploadingLanguage, mui16_objectPoolUploadingLanguageCode);

    m_connection.sendCommandUpdateLanguagePool();
    // we keep (mi8_objectPoolUploadingLanguage != -2), so a change in between doesn't care and won't happen!!
  }
}


void
UploadPoolState_c::timeEventCalculateLanguage()
{
  // lastReceived will be set if vtserverinstance processes the language pgn
  if( mi8_vtLanguage == -2 )
  { // try to calculate VT's language
    if( m_connection.getVtServerInst().receivedLocalSettings() )
    { // can calculate the language
      mi8_vtLanguage = -1; // indicate that VT's language is not supported by this WS, so the default language should be used

      const uint8_t cui8_languages = m_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().numberOfLanguagesToFollow;
      for( int i=0; i<cui8_languages; ++i )
      {
        const uint8_t* lang = m_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().languagesToFollow[i].language;
        if( m_connection.getVtServerInst().getLocalSettings()->languageCode == ((lang[0] << 8) | lang[1]) )
        {
          mi8_vtLanguage = i; // yes, VT's language is directly supported by this workingset
          break;
        }
      }
      m_pool.eventLanguagePgn( *m_connection.getVtServerInst().getLocalSettings() );
    }
    else
    { // cannot calculate the language YET, LANGUAGE_PGN not yet received, REQUEST & WAIT!
      m_connection.getVtServerInst().requestLocalSettings( m_connection.getIdentItem() );
      return; // do not proceed if VT's language not yet calculated!
    }
  }
}



void
UploadPoolState_c::reactOnStateChange( const SendStream_c& stream )
{
  switch( stream.getSendSuccess() )
  {
    case __IsoAgLib::SendStream_c::Running:
      break;

    case __IsoAgLib::SendStream_c::SendAborted:
      startCurrentUploadPhase(); // re-send the current stream (partial OP)
      break;

    case __IsoAgLib::SendStream_c::SendSuccess:
      indicateUploadPhaseCompletion(); // may complete the upload or switch to the next phase
      break;
  }
}


void
UploadPoolState_c::indicateUploadCompletion()
{
  if( successfullyUploaded() )
  { // user / language updates are being sent as "command"
    m_connection.finishUploadCommand();
  }
  else
  { // successfully uploaded complete initial pool
    // Command: Object Pool Transfer --- Parameter: Object Pool Ready
    m_connection.sendMessage( 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff );

    men_uploadPoolState = UploadPoolWaitingForEOOResponse;
  }
}


void
UploadPoolState_c::indicateUploadPhaseCompletion()
{
  if (men_uploadPoolType == UploadPoolTypeUserPoolUpdate)
  { // we only have one part, so we're done!
    mc_iVtObjectStreamer.mpc_objectsToUpload = NULL; // just for proper cleanup.
    // We don't need that pointer anymore. It can be invalid after we told the client
    // that we're done with partial user objectpool upload/update.
    indicateUploadCompletion(); // Send "End of Object Pool" message
  }
  else
  { // we may have multiple parts, so check that..
    // move to next possible one.
    if (men_uploadPoolType == UploadPoolTypeLanguageUpdate)
      mui_uploadPhaseAutomatic += 2; // skip the GENERAL parts, move on directly to next LANGUAGE part!
    else
      mui_uploadPhaseAutomatic += 1;

    startCurrentUploadPhase();
  }
}


void
UploadPoolState_c::startCurrentUploadPhase()
{
  IsoAgLib::iMultiSendStreamer_c* streamer = NULL;
  switch (men_uploadPoolType)
  {
    case UploadPoolTypeUserPoolUpdate:
      streamer = ms_uploadPhaseUser.pc_streamer;
      mc_iVtObjectStreamer.mpc_objectsToUpload = mppc_uploadPhaseUserObjects;
      mc_iVtObjectStreamer.setStreamSize (ms_uploadPhaseUser.ui32_size);
      break;

    case UploadPoolTypeCompleteInitially:
    case UploadPoolTypeLanguageUpdate:
      // First, check current phase.
      // while the current phase is n/a, move to next.
      while ((mui_uploadPhaseAutomatic <= UploadPhaseLAST) && (ms_uploadPhasesAutomatic [mui_uploadPhaseAutomatic].ui32_size == 0))
      { // prepare for the next part
        if (men_uploadPoolType == UploadPoolTypeLanguageUpdate)
          mui_uploadPhaseAutomatic += 2; // skip the GENERAL parts, move on directly to next LANGUAGE part!
        else
          mui_uploadPhaseAutomatic += 1;
      }
      if (mui_uploadPhaseAutomatic > UploadPhaseLAST)
      { // done with all phases!
        indicateUploadCompletion(); // Send "End of Object Pool" message
        return;
      }
      // else: start next phase
      streamer = ms_uploadPhasesAutomatic [mui_uploadPhaseAutomatic].pc_streamer;
      // first, prepare the individual upload phases.
      switch (UploadPhase_t (mui_uploadPhaseAutomatic)) // allowed cast, we're in enum-bounds!
      {
        case UploadPhaseIVtObjectsFix:
          mc_iVtObjectStreamer.mpc_objectsToUpload = m_pool.getIVtObjects()[0]; // main FIX (lang. indep) iVtObject part
          mc_iVtObjectStreamer.setStreamSize (ms_uploadPhasesAutomatic [mui_uploadPhaseAutomatic].ui32_size);
          break;

        case UploadPhaseIVtObjectsLang:
        { // phase 0 & 1 use iVtObjectStreamer, so prepare for that!
          const int8_t ci8_realUploadingLanguage = ((mi8_objectPoolUploadingLanguage < 0) ? 0 : mi8_objectPoolUploadingLanguage) + 1;
          mc_iVtObjectStreamer.mpc_objectsToUpload = m_pool.getIVtObjects()[ci8_realUploadingLanguage];
          mc_iVtObjectStreamer.setStreamSize (ms_uploadPhasesAutomatic [mui_uploadPhaseAutomatic].ui32_size);
        } break;

        case UploadPhaseAppSpecificFix:
          break; // nop
        case UploadPhaseAppSpecificLang:
          break; // nop

        // no default, to catch warning on unhandled enum in case an enum-value is added.
      }
      break;
    // no default, to catch warning on unhandled enum in case an enum-value is added.
  }

  __IsoAgLib::getMultiSendInstance( m_connection.getMultitonInst() ).sendIsoTarget(
    m_connection.getIdentItem().isoName(),
    m_connection.getVtServerInst().getIsoName(),
    streamer,
    ECU_TO_VT_PGN, this );
}


void
UploadPoolState_c::doStart()
{
  men_uploadPoolState = UploadPoolInit; // with "UploadInit
  mi8_vtLanguage = -2; // (re-)query LANGUAGE_PGN
  m_uploadingVersion = 0; // re-query version (needed for pool adaptation, e.g. omit Aux2 for v2 VTs)
}


void
UploadPoolState_c::doStop()
{
}


void
UploadPoolState_c::notifyOnVtsLanguagePgn()
{
  mi8_vtLanguage = -2;
}


bool
UploadPoolState_c::cantBeUploaded() const
{
  return( men_uploadPoolState == UploadPoolEndFailed );
}


bool
UploadPoolState_c::successfullyUploaded() const
{
  return( men_uploadPoolState == UploadPoolEndSuccess );
}


void
UploadPoolState_c::setObjectPoolUploadingLanguage()
{
  mi8_objectPoolUploadingLanguage = mi8_vtLanguage;
  mui16_objectPoolUploadingLanguageCode = 0x0000;
  if (m_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().numberOfLanguagesToFollow > 0) // supporting multilanguage.
  { // only if the objectpool has 1 or more languages, it makes sense to add the language code to the version-name
    const int8_t ci8_realUploadingLanguage = (mi8_objectPoolUploadingLanguage < 0) ? 0 : mi8_objectPoolUploadingLanguage;
    const uint8_t* lang = m_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().languagesToFollow[ci8_realUploadingLanguage].language;
    mui16_objectPoolUploadingLanguageCode = (lang [0] << 8) | lang[1];
    marrp7c_versionLabel[5] = lang [0];
    marrp7c_versionLabel[6] = lang [1];
  }
}


void
UploadPoolState_c::finalizeUploading()
{
  if (men_uploadPoolType == UploadPoolTypeUserPoolUpdate)
  { /// Was user-pool-update
    m_pool.eventPartialPoolUploadedSuccessfully();
  }
  else
  { /// Was complete initial pool or language pool update.
    /// in both cases we uploaded in one specific language!! so do the following:
    mi8_objectPoolUploadedLanguage = mi8_objectPoolUploadingLanguage;
    mui16_objectPoolUploadedLanguageCode = mui16_objectPoolUploadingLanguageCode;
    mi8_objectPoolUploadingLanguage = -2; // -2 indicated that the language-update while pool is up IS IDLE!
    mui16_objectPoolUploadingLanguageCode = 0x0000;

  #if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
    INTERNAL_DEBUG_DEVICE << "===> finalizeUploading () with language: "<<(int)mi8_objectPoolUploadedLanguage;
    if (mi8_objectPoolUploadedLanguage >= 0) INTERNAL_DEBUG_DEVICE <<" ["<<uint8_t(mui16_objectPoolUploadedLanguageCode>>8) <<uint8_t(mui16_objectPoolUploadedLanguageCode&0xFF)<<"]";
    INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
    if (men_uploadPoolType == UploadPoolTypeLanguageUpdate)
    {
      // no need to set "men_objectPoolState" and "men_uploadType", this is done in "finishUploadCommand()"
    }
    else
    {
  #if DEBUG_VTCOMM || DEBUG_VTPOOLUPLOAD
      INTERNAL_DEBUG_DEVICE << "Now men_uploadPoolState = UploadPoolEndSuccess;" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
      men_uploadPoolState = UploadPoolEndSuccess;
    }

    m_connection.notifyOnFinishedNonUserPoolUpload(
      men_uploadPoolType == UploadPoolTypeCompleteInitially );

    m_pool.eventObjectPoolUploadedSuccessfully(
      men_uploadPoolType == UploadPoolTypeLanguageUpdate, 
      mi8_objectPoolUploadedLanguage,
      mui16_objectPoolUploadedLanguageCode);
  }
}


void
UploadPoolState_c::initObjectPoolUploadingPhases(
  UploadPoolType_t ren_uploadPoolType, 
  IsoAgLib::iVtObject_c** rppc_listOfUserPoolUpdateObjects, 
  uint16_t aui16_numOfUserPoolUpdateObjects )
{
  isoaglib_assert( m_uploadingVersion != 0 );

  if (ren_uploadPoolType == UploadPoolTypeUserPoolUpdate)
  { // Activate User triggered Partial Pool Update
    // check params first
    if (aui16_numOfUserPoolUpdateObjects == 0)
      return;

    /// INIT FIRST
    ms_uploadPhaseUser.pc_streamer = &mc_iVtObjectStreamer;
    ms_uploadPhaseUser.ui32_size = 1; // the 0x11 command-byte is always there.
    mppc_uploadPhaseUserObjects = rppc_listOfUserPoolUpdateObjects;

    /// COUNT
    for (uint32_t curObject=0; curObject < aui16_numOfUserPoolUpdateObjects; ++curObject)
      ms_uploadPhaseUser.ui32_size += fitTerminalWrapper( *static_cast<vtObject_c*>( mppc_uploadPhaseUserObjects[curObject] ) );
  }
  else
  { // *CONDITIONALLY* Calculate GENERAL Parts sizes
    if (ren_uploadPoolType == UploadPoolTypeCompleteInitially)
    { // start with first phase
      mui_uploadPhaseAutomatic = UploadPhaseFIRSTfix;

      /// Phase 0
      ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsFix].pc_streamer = &mc_iVtObjectStreamer;
      ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsFix].ui32_size = 1; // the 0x11 command-byte is always there.
      for (uint32_t curObject=0; curObject < m_pool.getNumObjects(); ++curObject)
        ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsFix].ui32_size += fitTerminalWrapper( *static_cast<vtObject_c*>( m_pool.getIVtObjects()[0][curObject] ) );

      /// Phase 2
      const STL_NAMESPACE::pair<uint32_t, IsoAgLib::iMultiSendStreamer_c*> cpair_retval = m_pool.getAppSpecificFixPoolData();
      ms_uploadPhasesAutomatic [UploadPhaseAppSpecificFix].pc_streamer = cpair_retval.second;
      ms_uploadPhasesAutomatic [UploadPhaseAppSpecificFix].ui32_size = cpair_retval.first;
    }
    else
    { // start with second phase (lang. dep that is)
      mui_uploadPhaseAutomatic = UploadPhaseFIRSTlang;
    }

    // *ALWAYS* Calculate LANGUAGE Part size (if objectpool has multilanguage!)
    /// Phase 1
    ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsLang].pc_streamer = &mc_iVtObjectStreamer;
    ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsLang].ui32_size = 0; // there may not always be a language part.
    if (m_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().numberOfLanguagesToFollow > 0) // supporting multilanguage.
    {
      const int8_t ci8_realUploadingLanguage = ((mi8_objectPoolUploadingLanguage < 0) ? 0 : mi8_objectPoolUploadingLanguage) + 1;

      for (uint32_t curObject=0; curObject < m_pool.getNumObjectsLang(); ++curObject)
        ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsLang].ui32_size += fitTerminalWrapper( *static_cast<vtObject_c*>( m_pool.getIVtObjects()[ci8_realUploadingLanguage][curObject] ) );
      if (ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsLang].ui32_size > 0)
      { // only if there's at least one object being streamed up as user-partial-objectpool-update add the CMD byte for size calculation...
        ++ms_uploadPhasesAutomatic [UploadPhaseIVtObjectsLang].ui32_size; // add the 0x11 byte!
      }
    } // else: no LANGUAGE SPECIFIC objectpool, so keep this at 0 to indicate this!

    /// Phase 3
    const STL_NAMESPACE::pair<uint32_t, IsoAgLib::iMultiSendStreamer_c*> cpair_retval = m_pool.getAppSpecificLangPoolData(mi8_objectPoolUploadingLanguage, mui16_objectPoolUploadingLanguageCode);
    ms_uploadPhasesAutomatic [UploadPhaseAppSpecificLang].pc_streamer = cpair_retval.second;
    ms_uploadPhasesAutomatic [UploadPhaseAppSpecificLang].ui32_size = cpair_retval.first;
  }
  men_uploadPoolType = ren_uploadPoolType;
}


} // __IsoAgLib
