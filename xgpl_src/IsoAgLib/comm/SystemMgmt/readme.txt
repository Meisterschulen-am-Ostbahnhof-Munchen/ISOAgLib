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
  * All address claim variants have in common, that they require a pointer to a
  * variable of type DevKey_c which defines the device type
  * of the own device. This way the IsoAgLib can adapt the
  * device instance number, if the initial device instance number
  * is already used, so that an alternative free number has to be chosen.
  * This change is written directly in the pointed variables, so that
  * the application has always direct access to the active device type and -instance
  * setting on the BUS.
  *
  * @subsection SystemMgmtAnnIso Start ISO 11783 AddressClaim
  * You can claim an address for a member with the following code lines:
  * \code
  * #include \<xgpl_src/IsoAgLib/comm/SystemMgmt/iidentitem_c.h\>
  * // define device type := 2, device instance := 0
  * IsoAgLib::DevKey_c c_myType( 2, 0 );
  * // use 64bit NAME definition as 8-byte uint8_t array
  * uint8_t isoName[8];
  * // set the value of the array from EEPROM or somewhere else
  * // ...
  * // Direct address claim on construction ( i.e. definition of variable )
  * IsoAgLib::iIdentItem_c c_isoOnlyNameItem( &c_myType, isoName, 254 );
  * // you can let the IsoAgLib build the NAME string from the single value fields
  * bool     b_selfConf = true;
  * uint8_t  ui8_indGroup = 2,
  *          ui8_func = 25,
  *          ui8_wantedSa = 128,
  *          ui8_funcInst = 0,
  *          ui8_ecuInst = 0,
  *          ui8_deviceClass = 0,
  *          ui8_deviceClassInst = 0;
  * uint16_t ui16_manufCode = 0x7FF;
  * // optionally define EEPROM adress, where SA is stored/reloaded
  * // ( if valid EEPROM adress is given, the value of ui8_wantedSe is ignored )
  * // this default adress is invalid -> don't use EEPROM fo SA
  * uint16_t ui16_eepromAdrSa = 0xFFFF;
  * uint32_t ui32_serNo = 27;
  * // maybe change the default values from EEPROM
  * // ...
  * IsoAgLib::iIdentItem_c c_isoOnlyFlagsItem(
  *     &c_myType, b_selfConf, ui8_indGroup, ui8_func, ui16_manufCode,
  *     ui32_serNo, ui8_wantedSa, ui16_eepromAdrSa, ui8_funcInst, ui8_ecuInst);
  *
  * // You can alternatively start the address claim at a later time
  * IsoAgLib::iIdentItem_c c_isoItemLaterAddressClaim;
  * // some work inbetween ( e.g. read configuration from EEPROM )
  * // ...
  * // now start address claim
  * c_isoItemLaterAddressClaim.start(
  *     &c_myType, b_selfConf, ui8_indGroup, ui8_func, ui16_manufCode,
  *     ui32_serNo, ui8_wantedSa, ui16_eepromAdrSa, ui8_funcInst, ui8_ecuInst);
  *
  * // claim address for master device with list of slaves
  * uint8_t ui8_slaveCnt = 2;
  * IsoAgLib::iIsoName_c c_slaveArray[2];
  * // read NAME settings for slaves
  * // ...
  * // now start address claim with attached slave list
  * // ( IsoAgLib sends definition of master state and
  * //   the listing of all slaves after successful address claim )
  * IsoAgLib::iIdentItem_c c_isoOnlyFlagsMasterSlaveItem(
  *     &c_myType, b_selfConf, ui8_indGroup, ui8_func, ui16_manufCode,
  *     ui32_serNo, ui8_wantedSa, ui16_eepromAdrSa, ui8_funcInst, ui8_ecuInst,
  *     ui8_slaveCnt, c_slaveArray );
  *
  * // do something inbetween
  * // ...
  * // now check if already claimed address
  * if ( c_isoItemLaterAddressClaim.isClaimedAddress() ) {
  *   // I'm online ;-))
  * }
  * \endcode
  *
  * @subsection SystemMgmtLaterSelection Select Protocol on AddressClaim
  * The IsoAgLib allows you to delay the decision on the protocol to use
  * to the time of address claim. This can be useful if the implement shall
  * start as ISO system dependent on state of digital input or at a special event.
  * \code
  * #include \<xgpl_src/IsoAgLib/comm/SystemMgmt/iidentitem_c.h\>
  * // define device type := 2, device instance := 0
  * IsoAgLib::DevKey_c c_myType( 2, 0 );
  * // simply define instance of variable wihtout initialisation for specific value
  * IsoAgLib::iIdentItem_c c_isoItemLaterDecide;
  * bool b_start false;
  * // perform other init and read arguments for protocol select
  * // now address claim should be started
  * b_start = true;
  * if ( b_start )
  * { // start ISO
  *   bool     b_selfConf = true;
  *   uint8_t  ui8_indGroup = 2,
  *            ui8_func = 25,
  *            ui8_wantedSa = 128,
  *            ui8_funcInst = 0,
  *            ui8_ecuInst = 0,
  *            ui8_deviceClass = 0,
  *            ui8_deviceClassInst = 0;
  *   uint16_t ui16_manufCode = 0x7FF;
  *   uint16_t ui16_eepromAdrSa = 0xFFFF;
  *   uint32_t ui32_serNo = 27;
  *   // now start ISO annouce
  *   c_isoItemLaterAddressClaim.start(
  *       &c_myType, b_selfConf, ui8_indGroup, ui8_func, ui16_manufCode,
  *       ui32_serNo, ui8_wantedSa, ui16_eepromAdrSa, ui8_funcInst, ui8_ecuInst);
  * }
  * \endcode
  *
  * @section SystemMgmtInform Retrieve Information from ISO 11783 List
  * The global accessible instance of IsoAgLib::iIsoMonitor_c is best suited,
  * if exclusive ISO 11783 devices shall be reported on a request.
  * \code
  * #include \<xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/iisomonitor_c.h\>
  * // retrieve amount of ISO members with claimed address of specific device type
  * const uint8_t cui8_myPrefferedDeviceType = 2;
  * const uint8_t cui8_isoActiveMemberCnt = IsoAgLib::getIisoMonitorInstance().isoMemberCnt( cui8_searchDeviceType, true );
  * // check if some other device uses my device type
  * bool b_myDeviceTypeFree = ( cui8_isoActiveGetyCnt == 0)?true:false;
  * uint8_t ui8_lowestFreeFuncInst = 0;
  * // retrieve lowest free function instance value for this ECU
  * // -> loop through active ISO members of my device type
  * for ( uint8_t ui8_ind = 0; ui8_ind < cui8_isoActiveGetyCnt; ui8_ind++ ) {
  *   // access item with claimed address at position ui8_ind
  *   if ( IsoAgLib::getIisoMonitorInstance().isoMemberInd( cui8_searchDeviceType, ui8_ind, true ).funcInst() >= ui8_lowestFreeFuncInst )
  *   { // set my func inst to one greater
  *     ui8_lowestFreeFuncInst = IsoAgLib::getIisoMonitorInstance().isoMemberInd( cui8_searchDeviceType, ui8_ind, true ).funcInst() + 1;
  *   }
  * }
  * // now ui8_lowestFreeFuncInst has free function instance value
  * \endcode
  *
  */