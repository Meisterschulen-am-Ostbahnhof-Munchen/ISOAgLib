/** @page SystemMgmtPage Device AddressClaim and Management of Information in Monitor Lists
  * The IsoAgLib allows to claim address a local device ( identity ) with a single
  * function call at the BUS. Additionally it provides simple access to individual
  * information of other members, like source adress, name ( 64-bit ISO )
  * and device type. Thus an implement can simply check for available devices,
  * which could be used for optimisation of the own work process. In case more than
  * one implement of a device type is active, the IsoAgLib provides a simple
  * loop-search function, to check all implements of the searched type.
  *
  * The IsoAgLib stores information of active BUS members in a monitor list,
  * so that the information retrieval is independent from the state messages
  * on the BUS.
  * This is also used for the address claim process, which results in the claim
  * of a free or static reserved source adress. The application can check all the time
  * for the activity and address claim state of the local own and remote devices.
  * This way the IsoAgLib provides selective access to devices which are currently
  * claiming address, or which has already claimed an address.
  *
  * @subsection SystemMgmtPrjFiles Selection of Source Files for ISO 11783
  * The source files for system management are organised so that all protocol specific
  * stuff is placed in specific subdirectories. <br>
  * <b>Sources for any protocols</b><br>
  * - files in <i>\<xgpl_src/IsoAgLib/comm/System\></i> ( <b>no subdirectories</b> )
  * - everything in subdirectory <i>\<xgpl_src/IsoAgLib/comm/System<b>/impl</b>\></i>
  * <br>
  * <b>Sources for ISO 11783</b>
  * - everything in subdirectory <i>\<xgpl_src/IsoAgLib/comm/System<b>/ISO11783</b>\></i>
  * <br>
  * @subsection SystemMgmtDefines Project configuration for ISO 11783
  * As some parts in the IsoAgLib are dependent on the supported protocols, it is <b>very</b>
  * important to define the individual protocols to implement by some #define's. This can
  * be achieved by the default settings in the central project configuration file
  * <i>\<xgpl_src/Application_Config/isoaglib_config.h\></i> and by the compiler-defines in the Makefile.
  * The decision on the protocols is based on the availability of the following #define clauses:
  * - #define USE_ISO_11783
  * These define settings can be derived from the #define values of:
  * - USE_ISO_11783_YN YES|NO
  * In case neither USE_xy nor USE_xy_YN is defined, the config files
  * <i>\<xgpl_src/Application_Config/isoaglib_config.h\></i> defines a default setting.
  * Thus you can change the default rules for your individual needs in this file.
  *
  * @section SystemMgmtAddressClaim Address Claim of Local Device
  * 
  *
  * @subsection SystemMgmtAnnIso Start ISO 11783 AddressClaim
  * You can claim an address for a member with the following code lines.
  * The necessary header is automatically included by xgpl_src/Application_Config/isoaglib_config.h 
  * if the define PRJ_USE_AUTOGEN_CONFIG is set to config_0_0_AddressClaimIso 
  * ( see also at List of configuration settings for 0_0_AddressClaimIso ).
  * \code 
  * #include \<xgpl/IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h\>
  * #include \<xgpl/IsoAgLib/comm/SystemMgmt/iidentitem_c.h\>
  * 
  * using namespace IsoAgLib;
  * 
  * bool     b_selfConf = true;
  * uint8_t  ui8_indGroup = 2,
  *          ui8_func = 25,
  *          ui8_wantedSa = 128,
  *          ui8_funcInst = 0,
  *          ui8_ecuInst = 0,
  *          ui8_deviceClass = 2,
  *          ui8_deviceClassInst = 0;
  * uint16_t ui16_manufCode = 0x7FF;
  * uint32_t ui32_serNo = 27;
  *
  * // optionally define EEPROM adress, where SA is stored/reloaded
  * // ( if valid EEPROM adress is given, the value of ui8_wantedSe is ignored )
  * // this default adress is invalid -> don't use EEPROM fo SA
  * uint16_t ui16_eepromAdrSa = 0xFFFF;
  *
  * // Start address claim of the local identity/member
  * IsoAgLib::iIdentItem_c c_myIdent (ui8_indGroup,
  *                                   rui8_deviceClass,
  *                                   ui8_deviceClassInst,
  *                                   ui8_func,
  *                                   ui16_manufCode,
  *                                   ui32_serNo);
  *                                   // further parameters use the default values as given in the constructor
  * ...
  * 
  * // You can alternatively start the address claim at a later time
  * // Therefore the device has to be initialized with default parameters
  * IsoAgLib::iIdentItem_c c_isoItemLaterAddressClaim;
  * // some work inbetween ( e.g. read configuration from EEPROM )
  * // ...
  * // now start address claim
  * c_isoItemLaterAddressClaim.init(
  *     ui8_indGroup, ui8_deviceClass, ui8_deviceClassInst, ui8_func, ui16_manufCode, 
  *     ui32_serNo, ui8_wantedSa, ui16_eepromAdrSa, ui8_funcInst, ui8_ecuInst);
  *
  * // claim address for master device with list of slaves
  * uint8_t ui8_slaveCount = 2;
  * IsoAgLib::iIsoName_c c_slaveArray[2];
  * // read NAME settings for slaves
  * // ...
  * // now start address claim with attached slave list
  * // ( IsoAgLib sends definition of master state and
  * //   the listing of all slaves after successful address claim )
  * IsoAgLib::iIdentItem_c c_isoOnlyFlagsMasterSlaveItem(
  *     ui8_indGroup, ui8_deviceClass, ui8_deviceClassInst, ui8_func, ui16_manufCode,
  *     ui32_serNo, ui8_wantedSa, ui16_eepromAdrSa, ui8_funcInst, ui8_ecuInst, b_selfConf,
  *     ui8_slaveCount, c_slaveArray);
  *
  * // do something inbetween
  * // ...
  * // now check if already claimed address
  * if ( c_isoItemLaterAddressClaim.isClaimedAddress() ) 
  * {
  *   // I'm online ;-))
  * }
  * \endcode
  *
  * @section SystemMgmtInform Retrieve Information from ISO 11783 List
  * The global accessible instance of IsoAgLib::iIsoMonitor_c is best suited,
  * if exclusive ISO 11783 devices shall be reported on a request.
  * \code
  * #include \<xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/iisomonitor_c.h\>
  * // retrieve amount of ISO members with claimed address of specific device type
  * const uint8_t cui8_searchDeviceType = 2;
  * const uint8_t cui8_isoActiveMemberCnt = IsoAgLib::getIisoMonitorInstance().isoMemberDevClassCnt( cui8_searchDeviceType, true );
  * // check if some other device uses my device type
  * bool b_myDeviceTypeFree = ( cui8_isoActiveGetyCnt == 0 )? true : false;
  * uint8_t ui8_lowestFreeFuncInst = 0;
  * // retrieve lowest free function instance value for this ECU
  * // -> loop through active ISO members of my device type
  * for ( uint8_t ui8_ind = 0; ui8_ind \< cui8_isoActiveGetyCnt; ui8_ind++ )
  * {
  *   // access item with claimed address at position ui8_ind
  *   if ( IsoAgLib::getIisoMonitorInstance().isoMemberDevClassInd( cui8_searchDeviceType, ui8_ind, true ).funcInst() >= ui8_lowestFreeFuncInst )
  *   { // set my func inst to one greater
  *     ui8_lowestFreeFuncInst = IsoAgLib::getIisoMonitorInstance().isoMemberDeviceTypeInd( cui8_searchDeviceType, ui8_ind, true ).funcInst() + 1;
  *   }
  * }
  * // now ui8_lowestFreeFuncInst has free function instance value
  * \endcode
  *
  */