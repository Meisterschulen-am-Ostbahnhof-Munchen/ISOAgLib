/*
  devicepool_c.cpp: Device Description Object Pool with its Objects

  (C) Copyright 2013 - 2019 by OSB AG

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
#include "devicepool_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif


namespace __IsoAgLib {


  /* --- DeviceObject_c -------------------------------------------------- */

  uint16_t DeviceObject_c::m_objIdCounter = 1;

  DeviceObject_c::DeviceObject_c( const IsoAgLib::ProcData::DeviceObjectType_t type, const char* desig )
    : m_objectType( type ),
      m_objectId( ( type == IsoAgLib::ProcData::ObjectTypeDVC )? 0 : m_objIdCounter ),
      m_designator( NULL ) {

    isoaglib_assert( m_objectId != 0xFFFF );
    init( desig );
    if ( m_objectType != IsoAgLib::ProcData::ObjectTypeDVC )
      ++m_objIdCounter;
  }


  void DeviceObject_c::init( const char* desig ) {
    isoaglib_assert( !desig || (CNAMESPACE::strlen( desig ) <= 32) );
    m_designator = desig;
  }


  void DeviceObject_c::setDesignator( const char* desig ) {
    isoaglib_assert( desig );
    isoaglib_assert( CNAMESPACE::strlen( desig ) <= 32 );
    m_designator = desig;
  }


  void DeviceObject_c::formatHeader( ByteStreamBuffer_c& byteStream ) const {
    static const char* deviceLabels[] = { "DVC", "DET", "DPD", "DPT", "DVP" };

    byteStream.format( ( const uint8_t* )deviceLabels[m_objectType], 3 );
    byteStream.format( m_objectId );
  }


  uint32_t DeviceObject_c::getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const {
    return ( sizeof( m_objectId ) + 3 /* label */ );
  }


  void DeviceObject_c::calcChecksumAddHeader( DevicePool_c &pool ) const
  {
    static const char* deviceLabels[] = { "DVC", "DET", "DPD", "DPT", "DVP" };

    pool.calcChecksumAdd( ( const uint8_t* )(deviceLabels[m_objectType]), 3 );
    pool.calcChecksumAdd( m_objectId );
  }


  /* --- DVC -------------------------------------------------------------*/

  DeviceObjectDvc_c::DeviceObjectDvc_c( const char* version, const char* desig )
    : __IsoAgLib::DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDVC, desig )
    , m_version( version )
  //, m_serialNumber()
    , m_structLabel()
    , m_extendedStructureLabel()
    , m_identItem( NULL )
    , m_localization()
  {
    isoaglib_assert( version != NULL );
    m_serialNumber[ 0 ] = 0x00;
    // no init here, appropriate setters need to be called. basta.
  }


  void DeviceObjectDvc_c::setLocalSettings( const localSettings_s& currentSettings ) {
    Localization_s localSettings;

    localSettings.language[0] = currentSettings.languageCode >> 8;
    localSettings.language[1] = currentSettings.languageCode & 0xff;

    localSettings.dateFormat = currentSettings.dFormat;

    localSettings.numberFormat =  ( ( currentSettings.nDecimalPoint & 3 ) << 6 );
    localSettings.numberFormat |= ( ( currentSettings.nTimeFormat & 3 ) << 4 );

    localSettings.unitsFormatLow  = ( ( currentSettings.uDistance & 3 ) << 6 );
    localSettings.unitsFormatLow |= ( ( currentSettings.uArea     & 3 ) << 4 );
    localSettings.unitsFormatLow |= ( ( currentSettings.uVolume   & 3 ) << 2 );
    localSettings.unitsFormatLow |= ( ( currentSettings.uMass     & 3 ) );

    localSettings.unitsFormatHigh  = ( ( currentSettings.uTemperature & 3 ) << 6 );
    localSettings.unitsFormatHigh |= ( ( currentSettings.uPressure & 3 )    << 4 );
    localSettings.unitsFormatHigh |= ( ( currentSettings.uForce & 3 )       << 2 );
    localSettings.unitsFormatHigh |= ( ( currentSettings.uUnitsSystem & 3 ) );

    localSettings.reserved = 0xff;

    setLocalization( localSettings );
  }


  void DeviceObjectDvc_c::setLocalization( const Localization_s& local ) {
    //if (std::memcmp((void*)&local, (void*)&m_Localization, sizeof(Localization_s)) != 0)
    // setDirty(true);
    m_localization = local;
  }


  void DeviceObjectDvc_c::setStructureLabel( const uint8_t label[7] ) {
    isoaglib_assert( label[0] != 0xFF );
    isoaglib_assert( label[1] != 0xFF );
    isoaglib_assert( label[2] != 0xFF );
    isoaglib_assert( label[3] != 0xFF );
    isoaglib_assert( label[4] != 0xFF );
    isoaglib_assert( label[5] != 0xFF );
    isoaglib_assert( label[6] != 0xFF );

    m_structLabel.Byte1 = label[0];
    m_structLabel.Byte2 = label[1];
    m_structLabel.Byte3 = label[2];
    m_structLabel.Byte4 = label[3];
    m_structLabel.Byte5 = label[4];
    m_structLabel.Byte6 = label[5];
    m_structLabel.Byte7 = label[6];
  }


  void DeviceObjectDvc_c::setStructureLabel( const char* label ) {
    uint8_t tmpBuf[7] = {0, 0, 0, 0, 0, 0, 0};
    
    unsigned int len = CNAMESPACE::strlen( label );
    isoaglib_assert( len <= 7 );
    
    // limit length only to not crash in release, but problem should be caught in debug!
    if( len > 7 )
      len = 7;

    for( unsigned int i = 0; i < len; i++ )
      tmpBuf[i] = label[i];

    setStructureLabel( tmpBuf );
  }

  void DeviceObjectDvc_c::setExtendedStructureLabel( const char* s, uint8_t length )
  {
    isoaglib_assert( length <= 32 );

    m_extendedStructureLabel.length = length;
    CNAMESPACE::memcpy( m_extendedStructureLabel.byteString, s, length );
  }

  void DeviceObjectDvc_c::setExtendedStructureLabel( const char* s ) {
    setExtendedStructureLabel( s, (uint8_t)CNAMESPACE::strlen( s ) );
  }

  void DeviceObjectDvc_c::setSerialNumber( const char* s ) {
    isoaglib_assert( CNAMESPACE::strlen( s ) <= 32 );
    CNAMESPACE::strcpy( m_serialNumber, s );
  }


  void DeviceObjectDvc_c::formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    formatHeader( byteStream );

    isoaglib_assert( CNAMESPACE::strlen( m_version ) <= size_t((caps.versionNr >= 4) ? 128 : 32) );

    byteStream.format( m_designator );
    byteStream.format( m_version );
    byteStream.format( getWsmName().outputString(), 8 );
    byteStream.format( m_serialNumber );

    byteStream.format( ( uint8_t* )&m_structLabel, 7 );
    byteStream.format( ( uint8_t* )&m_localization, 7 );

    if( caps.versionNr >= 4 )
    {
      byteStream.format( m_extendedStructureLabel.length );
      byteStream.format( (uint8_t*)&m_extendedStructureLabel.byteString, m_extendedStructureLabel.length );
    }
  }


  uint32_t DeviceObjectDvc_c::getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    uint32_t size = DeviceObject_c::getSize( caps );
    size += sizeof( uint8_t ) + CNAMESPACE::strlen( getDesignator() );
    size += sizeof( uint8_t ) + CNAMESPACE::strlen( getVersion() );
    size += 8; // m_wsmName;
    size += sizeof( uint8_t ) + CNAMESPACE::strlen( getSerialNumber() );
    size += 14; /*  m_structLabel + m_localization */

    if( caps.versionNr >= 4 )
    {
      size += 1; // Extended Structure Label length byte
      size += m_extendedStructureLabel.length;
    }

    return size;
  }


  void
  DeviceObjectDvc_c::calcChecksumAdd( DevicePool_c &pool ) const
  {
    calcChecksumAddHeader( pool );

  //pool.calcChecksumAdd( m_designator );
  //pool.calcChecksumAdd( m_version );
  //pool.calcChecksumAdd( getWsmName().outputString(), 8 );
  //pool.calcChecksumAdd( m_serialNumber );

  //pool.calcChecksumAdd( ( uint8_t* )&m_structLabel, 7 );
  //pool.calcChecksumAdd( ( uint8_t* )&m_localization, 7 );
  }


  /* --- DET -------------------------------------------------------------*/

  DeviceObjectDet_c::DeviceObjectDet_c( uint16_t pid, uint16_t element, uint8_t type, const char* desig )
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDET, desig )
    , m_type( type )
    , m_elementNumber( element )
    , m_parentId( pid )
    , m_childList()
  {
    isoaglib_assert( element <= 4095 );
  }

  DeviceObjectDet_c::DeviceObjectDet_c()
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDET, NULL )
    , m_type( 0 )
    , m_elementNumber( 4096 ) // invalid to detect uninitialized Det!
    , m_parentId( 0xFFFF )
    , m_childList()
  {
  }

  void
  DeviceObjectDet_c::init( uint16_t pid, uint16_t element, uint8_t type, const char* desig )
  {
    isoaglib_assert( m_elementNumber == 4096 );
    isoaglib_assert( element <= 4095 );

    DeviceObject_c::init( desig );
    m_type = type;
    m_elementNumber = element;
    m_parentId = pid;
  }

  void DeviceObjectDet_c::formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const {
    formatHeader( byteStream );

    byteStream.format( uint8_t( m_type ) );
    byteStream.format( m_designator );
    byteStream.format( m_elementNumber );
    byteStream.format( m_parentId );
    byteStream.format( ( uint16_t )m_childList.size() );

    std::vector<uint16_t>::const_iterator it;
    for ( it = m_childList.begin(); it != m_childList.end(); ++it )
      byteStream.format( *it );
  }


  bool DeviceObjectDet_c::addChild( uint16_t childId ) {
    std::vector<uint16_t>::iterator it;
    for ( it = m_childList.begin(); it != m_childList.end(); ++it ) {
      if ( *it == childId )
        return true;
    }

    const size_t oldSize = m_childList.size();
    m_childList.push_back( childId );
    return m_childList.size() > oldSize;
  }


  uint32_t DeviceObjectDet_c::getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    uint32_t size = DeviceObject_c::getSize( caps );
    size += sizeof( m_type );
    size += sizeof( uint8_t ) + CNAMESPACE::strlen( getDesignator() );
    size += sizeof( m_elementNumber ) + sizeof( m_parentId );
    size += sizeof( uint16_t ); /* m_childList.size() */
    size += sizeof( uint16_t ) * m_childList.size();
    return size;
  }


  void
  DeviceObjectDet_c::calcChecksumAdd( DevicePool_c &pool ) const
  {
    calcChecksumAddHeader( pool );

    pool.calcChecksumAdd( uint8_t( m_type ) );
  //pool.calcChecksumAdd( m_designator );
    pool.calcChecksumAdd( m_elementNumber );
    pool.calcChecksumAdd( m_parentId );
    pool.calcChecksumAdd( ( uint16_t )m_childList.size() );

    std::vector<uint16_t>::const_iterator it;
    for ( it = m_childList.begin(); it != m_childList.end(); ++it )
      pool.calcChecksumAdd( *it );
  }


  void
  DeviceObjectDet_c::clearChildren()
  {
	  m_childList.clear();
  }


  /* --- DPD -------------------------------------------------------------*/

  DeviceObjectDpd_c::DeviceObjectDpd_c( uint16_t dpd_ddi, const IsoAgLib::ProcData::Properties_t& bitmaskProps,
                                        const IsoAgLib::ProcData::Methods_t& bitmaskMethods, const char* desig, const DeviceObjectDvp_c* dvp )
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDPD, desig )
    , m_ddi( dpd_ddi )
    , m_properties( bitmaskProps.getByte( 0 ) )
    , m_method( bitmaskMethods.getByte( 0 ) )
    , m_dvpObjectId( ( dvp ) ? dvp->getObjectId() : 0xFFFF )
  {
    isoaglib_assert( dpd_ddi != 0xFFFF );
  }


  DeviceObjectDpd_c::DeviceObjectDpd_c( uint16_t dpd_ddi, uint8_t properties, uint8_t triggerMethods,
                                        const char* desig, const DeviceObjectDvp_c* dvp )
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDPD, desig )
    , m_ddi( dpd_ddi )
    , m_properties( properties )
    , m_method( triggerMethods )
    , m_dvpObjectId( ( dvp ) ? dvp->getObjectId() : 0xFFFF )
  {
    isoaglib_assert( dpd_ddi != 0xFFFF );
    isoaglib_assert( properties < (1<<3) );
    isoaglib_assert( triggerMethods < (1<<5) );
  }

  DeviceObjectDpd_c::DeviceObjectDpd_c()
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDPD, NULL )
    , m_ddi( 0xFFFF ) // Reserved - used to indicate uninitialized DPD
    , m_properties( 0xFF )
    , m_method( 0xFF )
    , m_dvpObjectId( 0xFFFF )
  {
  }

  void DeviceObjectDpd_c::init( uint16_t dpd_ddi, const IsoAgLib::ProcData::Properties_t& bitmaskProps,
                                const IsoAgLib::ProcData::Methods_t& bitmaskMethods, const char* desig, const DeviceObjectDvp_c* dvp )
  {
    isoaglib_assert( m_ddi == 0xFFFF );
    isoaglib_assert( dpd_ddi != 0xFFFF );

    DeviceObject_c::init( desig );
    m_ddi = dpd_ddi;
    m_properties = bitmaskProps.getByte( 0 );
    m_method = bitmaskMethods.getByte( 0 );
    m_dvpObjectId = ( dvp ) ? dvp->getObjectId() : 0xFFFF;
  }

  void DeviceObjectDpd_c::formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    formatHeader( byteStream );

    byteStream.format( ddi() );
    byteStream.format( properties( caps.hasPeerControl ) );
    byteStream.format( method() );
    byteStream.format( m_designator );
    byteStream.format( m_dvpObjectId );
  }


  uint32_t DeviceObjectDpd_c::getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    return ( DeviceObject_c::getSize( caps ) + sizeof( m_ddi ) + sizeof( m_properties ) + sizeof( m_method )
             + sizeof( uint8_t ) + CNAMESPACE::strlen( getDesignator() )
             + sizeof( m_dvpObjectId ) );
  }


  void
  DeviceObjectDpd_c::calcChecksumAdd( DevicePool_c &pool ) const
  {
    calcChecksumAddHeader( pool );

    pool.calcChecksumAdd( m_ddi );
    pool.calcChecksumAdd( m_properties );
    pool.calcChecksumAdd( m_method );
  //pool.calcChecksumAdd( m_designator );
    pool.calcChecksumAdd( m_dvpObjectId );
  }


  /* --- DPT -------------------------------------------------------------*/

  DeviceObjectDpt_c::DeviceObjectDpt_c( uint16_t dpt_ddi, int32_t value, const char* desig, const DeviceObjectDvp_c* dvpRef )
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDPT, desig )
    , m_ddi( dpt_ddi )
    , m_value( value )
    , m_dvpObjectId( ( dvpRef ) ? dvpRef->getObjectId() : 0xFFFF )
  {}

  DeviceObjectDpt_c::DeviceObjectDpt_c( )
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDPT, NULL )
    , m_ddi( 0xFFFF )
    , m_value( 0 )
    , m_dvpObjectId( 0xFFFF )
  {}

  void
  DeviceObjectDpt_c::init( uint16_t dpt_ddi, int32_t value, const char* desig, const DeviceObjectDvp_c* dvpRef )
  {
    isoaglib_assert( m_ddi == 0xFFFF );
    isoaglib_assert( dpt_ddi != 0xFFFF );
    m_ddi = dpt_ddi;
    m_value = value;

    DeviceObject_c::init( desig );

    m_dvpObjectId = ( dvpRef ) ? dvpRef->getObjectId() : 0xFFFF;
  }


  void DeviceObjectDpt_c::formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const {
    formatHeader( byteStream );

    byteStream.format( m_ddi );
    byteStream.format( m_value );
    byteStream.format( m_designator );
    byteStream.format( m_dvpObjectId );
  }


  uint32_t DeviceObjectDpt_c::getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    return ( DeviceObject_c::getSize( caps ) + sizeof( m_ddi ) + sizeof( m_value )
             + sizeof( uint8_t ) + CNAMESPACE::strlen( m_designator )
             + sizeof( m_dvpObjectId ) );
  }


  void
  DeviceObjectDpt_c::calcChecksumAdd( DevicePool_c &pool ) const
  {
    calcChecksumAddHeader( pool );

    pool.calcChecksumAdd( m_ddi );
    pool.calcChecksumAdd( m_value );
  //pool.calcChecksumAdd( m_designator );
    pool.calcChecksumAdd( m_dvpObjectId );
  }


  /* --- DVP -------------------------------------------------------------*/

  DeviceObjectDvp_c::DeviceObjectDvp_c( float scale, int32_t offset, uint8_t decimals, const char* desig )
    : DeviceObject_c( IsoAgLib::ProcData::ObjectTypeDVP, desig )
    , m_offset( offset )
    , m_scale( scale )
    , m_decimals( decimals )
    , m_isDirty( false )
  {}


  void DeviceObjectDvp_c::formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const {
    formatHeader( byteStream );

    byteStream.format( m_offset );
    byteStream.format( m_scale );
    byteStream.format( m_decimals );
    byteStream.format( m_designator );
  }


  uint32_t DeviceObjectDvp_c::getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    return ( DeviceObject_c::getSize( caps ) + sizeof( m_offset ) + sizeof( m_scale ) + sizeof( m_decimals ) + sizeof( uint8_t ) + CNAMESPACE::strlen( getDesignator() ) ) ;
  }


  void
  DeviceObjectDvp_c::calcChecksumAdd( DevicePool_c &pool ) const
  {
    calcChecksumAddHeader( pool );

  //pool.calcChecksumAdd( m_offset );
  //pool.calcChecksumAdd( m_scale );
  //pool.calcChecksumAdd( m_decimals );
  //pool.calcChecksumAdd( m_designator );
  }




  /* --- DDOP --------------------------------------------------- */

  DevicePool_c::DevicePool_c( unsigned int reserveSize )
    : PdPool_c( reserveSize )
    , m_devicePool()
  {}


  void DevicePool_c::init( const IdentItem_c& ident )
  {
    getDvcObject()->init( ident );
  }

  void DevicePool_c::close()
  {
    //getDvcObject()->init( unspecified );
  }



  DeviceObjectDvc_c* DevicePool_c::getDvcObject() const {
    return static_cast<DeviceObjectDvc_c*>( getObject( 0, IsoAgLib::ProcData::ObjectTypeDVC ) );
  }


  DeviceObject_c* DevicePool_c::getObject( const uint16_t objId, const IsoAgLib::ProcData::DeviceObjectType_t objectType ) const {
    deviceMap_t::const_iterator it = m_devicePool.find( objId );
    if( ( it != m_devicePool.end() ) && ( it->second && it->second->getObjectType() == objectType ) ) {
      return it->second;
    }
    return NULL;
  }


  DeviceObjectDet_c* DevicePool_c::getDetObject( uint16_t elementNumber ) {
    for( deviceMap_t::iterator it = m_devicePool.begin(); it != m_devicePool.end(); ++it ) {
      if( it->second->getObjectType() == IsoAgLib::ProcData::ObjectTypeDET ) {
        if( ((DeviceObjectDet_c*)it->second)->elementNumber() == elementNumber ) {
          return (DeviceObjectDet_c*)it->second;
        }
      }
    }
    return NULL;
  }


  void DevicePool_c::add( DeviceObject_c& devObj ) {
    isoaglib_assert( (devObj.getObjectType() != IsoAgLib::ProcData::ObjectTypeDET) || (getDetObject(((DeviceObjectDet_c*)&devObj)->elementNumber()) == NULL) );
    (void)m_devicePool.insert(
      std::pair<uint16_t, DeviceObject_c*>( devObj.getObjectId(), &devObj ) ).second;
  }


  void DevicePool_c::add( ProcData_c& pd )
  {
    isoaglib_assert( pd.getDpd() );
    isoaglib_assert( pd.getDet() );
    isoaglib_assert( m_devicePool.find( pd.getDpd()->getObjectId() ) != m_devicePool.end() );
    isoaglib_assert( m_devicePool.find( pd.getDet()->getObjectId() ) != m_devicePool.end() );

    addPdBase( pd );
  }

  void DevicePool_c::clear()
  {
    m_devicePool.clear();
    m_procDatas.clear();
  }

#if 0
// currently not supported
  void DevicePool_c::changeDesignator( DeviceObject_c& obj, const char* str ) {
    obj.setDesignator( str );
    const IdentItem_c &identItem = getDvcObject()->getIdentItem();
    getTcClientInstance( identItem.getMultitonInst() ).processChangeDesignator( identItem, obj.getObjectId(), str );
  }
#endif


  void DevicePool_c::setLocalSettings( const localSettings_s& l ) {
    getDvcObject()->setLocalSettings( l );
    // @todo: call TcClient_c function (partial Pool-Upload will be triggered)
  }


  void DevicePool_c::updateLocale() {
#if 0
    deviceMap_t::iterator it;
    for ( it = m_devicePool.begin(); it != m_devicePool.end(); ++it ) {
      if ( it->second && it->second->getObjectType() == IsoAgLib::ProcData::ObjectTypeDVP )
        static_cast<IsoAgLib::iDeviceObjectDvp_c*>( it->second )->setDirty( true );
    }
#endif
  }


  ByteStreamBuffer_c DevicePool_c::getBytestream( uint8_t cmd, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) {
    const uint32_t size = getBytestreamSize( caps ) + 1; // one extra byte for command
    ByteStreamBuffer_c buffer;
    buffer.setBuffer( allocByteStreamBuffer( size ) );
    buffer.setSize( size );
    buffer.format( cmd );

    for ( deviceMap_t::iterator it = m_devicePool.begin(); it != m_devicePool.end(); ++it ) {
      it->second->formatBytestream( buffer, caps );
    }

    return buffer;
  }


  uint32_t DevicePool_c::getBytestreamSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const {
    uint32_t size = 0;
    for ( deviceMap_t::const_iterator it = m_devicePool.begin(); it != m_devicePool.end(); ++it ) {
      size += it->second->getSize( caps );
    }
    return size;
  }

  void DevicePool_c::calcChecksum()
  {
    calcChecksumStart();
    
    for ( deviceMap_t::const_iterator it = m_devicePool.begin(); it != m_devicePool.end(); ++it ) {
      it->second->calcChecksumAdd( *this );
    }

    calcChecksumEnd();
  }


  void DevicePool_c::calcChecksumAdd( uint16_t val ) {
    calcChecksumAdd( ( uint8_t )( val & 0xff ) );
    calcChecksumAdd( ( uint8_t )( ( val >> 8 ) & 0xff ) );
  }


  void DevicePool_c::calcChecksumAdd( uint32_t val ) {
    calcChecksumAdd( ( uint16_t )( val & 0xffff ) );
    calcChecksumAdd( ( uint16_t )( ( val >> 16 ) & 0xffff ) );
  }


  void DevicePool_c::calcChecksumAdd( const uint8_t* bp, size_t len ) {
    while ( len-- )
      calcChecksumAdd( *bp++ );
  }


  void DevicePool_c::calcChecksumAdd( const char* str ) {
    const size_t l = CNAMESPACE::strlen( str );
    calcChecksumAdd( uint8_t( l ) );
    calcChecksumAdd( ( const uint8_t* )str, l );
  }


  void DevicePool_c::calcChecksumAdd( int32_t val ) {
    calcChecksumAdd( ( uint32_t )val );
  }


  void DevicePool_c::calcChecksumAdd( float val ) {
    uint32_t iVal = 0;
    CNAMESPACE::memcpy( &iVal, &val, sizeof( float ) );
  #if defined(__TSW_CPP__) // Tasking uses mixed endian
    uint16_t lo = iVal >> 16;
    iVal = ( iVal << 16 ) | lo;
  #endif
    calcChecksumAdd( iVal );
  }

} // __IsoAgLib
