/** @page DinTerminalPage Upload of LBS+ Mask Binaries
  * The IsoAgLib enables the application to define parallel terminal masks for
  * DIN 9684 and ISO 11783 types. Based on the current activated protocol, and the
  * available terminal, the suitable terminal handling is used.
  * The class IsoAgLib::iDINMaskUpload_c is responsible for the upload of binary LBS+
  * mask definition files.
  * The application can simply define the properties of a mask binary with a constant
  * variable instance of the type IsoAgLib::t_maskDefinition. As this constant can be
  * placed in ROM, no RAM is wasted in case the corresponding terminal upload isn't started.
  * This is especially important, if more than one terminal type can be handled by several
  * alternative terminal definitions.
  * In case the array and structure can't be initialized directly into the constant, these
  * values should be placed in global RAM space by "static" variable type.
  * @section DinTerminalDefinition Example Defintion of LBS+ Terminal Properties
  * The definition of the terminal project properties can be as following, if a AGCO Fendt
  * Varioterminal is used:
  * \code
    /** size of terminal binary */
    const int32_t i32_VariomenueByteSize = 200000;
    /** visibility configuration field in ROM */
    const HUGE_MEM uint8_t* pVarioConfigStartVektor = (HUGE_MEM uint8_t*)0x0A0000;
    /** terminal binary in ROM */
    const HUGE_MEM uint8_t* pVarioMenueStartVektor  = (HUGE_MEM uint8_t*)0x0A0080;

    /** constant array of process data values for project sync between ECU and terminal */
    const IsoAgLib::t_syncTupel psVarioSyncNo[] = {
      {1, LBS_PLUS_SYSTEM_STATUS},            ///< terminal binary is available
      {100, LBS_PLUS_GENERIC_DRIVER_VERSION}, ///< 1.00 as generic driver version
      {100, LBS_PLUS_PROJECT_VERSION},        ///< 1.00 as project version
      {0xFA6A2, LBS_PLUS_PROJECT_DATE},       ///< 02.05.2003 as project date
      {0x40000051, LBS_PLUS_PROJECT_STATUS},
      {200, LBS_PLUS_PROJECT_IDENT_NO},
      {0x0000406B, LBS_PLUS_PROJECT_SIGNATURE},
      {0x0000019B, LBS_PLUS_PROJECTOR_VERSION},
      {0x000FA4C6, LBS_PLUS_PROJECTOR_DATE},
      {0x60000, LBS_PLUS_PROJECT_START},       ///< ROM adress of terminal binary in ECU
      {0x70000, LBS_PLUS_PROJECT_VISISTART},   ///< ROM adress of visibility definition in ECU
      {0x00000080, LBS_PLUS_PROJECT_VISISIZE}  ///< size of visibility definition in ECU
    };
    /** example GetyPos_c of local ident, which acts as terminal uploader */
    IsoAgLib::GetyPos_c *pc_getypos = IsoAgLib::GetyPos_c( 2, 0 );

    /** structure with assembled terminal project definition */
    const IsoAgLib::t_maskDefinition sVarioMask = {
      pc_getypos,               ///< pointer to local gtp
      IsoAgLib::GetyPos_c(1, 2), ///< GetyPos of Terminal
      "VARIO_T",                ///< name of Fendt Vario Term
      IsoAgLib::FendtVario,      ///< is LBS+ terminal
      pVarioConfigStartVektor,  ///< string with visibility config settings
      0x80,                     ///< size of config string
      pVarioMenueStartVektor,   ///< pointer to mask data
      i32_VariomenueByteSize,   ///< size of the mask
      12,                       ///< amount of synchronisation process data
      psVarioSyncNo             ///< array of process data for project sync
    };
  * \endcode
  *
  * @section DinTerminalRegistration Registrate Terminal Definition
  * The definition structure of type IsoAgLib::t_maskDefinition can be registered
  * into IsoAgLib with a simple function call. This call is independent from
  * immediate availability of terminal or completed address claim of local ident.
  * As soon as both the registered terminal is ready and the corresponding local
  * ident has completely claimed address, the terminal mask definition will be activated for
  * upload to the terminal.
  * The registration call is shown in the following example:
  * \code
  * /** single function call to register the terminal definition structure */
  * IsoAgLib::getIdinMaskuploadInstance().registerLbsPlusMask(&sVarioMask);
  * \endcode
  *
  * @section DinTerminalStateCheck Check for Current Upload State
  * \code
  * if ( IsoAgLib::getIdinMaskuploadInstance().isRunning() ) {
  *   // hey - the upload is running ;-))
  * }
  * else if ( IsoAgLib::getIdinMaskuploadInstance().isSuccess() ) {
  *   // fine - our ECU user interface is uploaded to the LBS+ terminal
  * }
  * else if ( IsoAgLib::getIdinMaskuploadInstance().isAborted() ) {
  *   // oh shit - the upload was aborted by some problems - check the IsoAgLib error state for more information
  * }
  * \endcode
  *
  * @section DinTerminalManagement Management of Active LBS+ Terminal Mask
  * The event and value exchange for IsoAgLib+ terminals is managed by process data variables.
  * Please look \ref ProcDataPage "here" for more information.
  * The optimal process data capability and ressource requirement level is provided
  * by the type IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c in case a Varioterminal
  * is used. Reason: It doesn't support measurement programs, as it just implements single shot
  * value requests.<br>
  * The other process data types, which support measurement program and capable setpoint handling
  * are requiring too much RAM in relation to needed functionality.
  * But if you use a Fieldstart terminal or the process data values are also exchanged with other
  * members at the CAN BUS, more capable process data types may be reasonable.
  *
  * @section DinTerminalExternalRequirements Required Modules of Other Communication Subdirectories
  * The upload of LBS+ terminal definition requires at least the integrtion of the process data type
  * IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c into the project, as the values
  * for project synchronisation are internally handled with this type.
  * If the further value exchange is managed by other process data types, the corresponding modules must
  * be included into the project additionally
  * ( \ref ProcDataDirectoryStructureGrp "description of types and corresponding modules" ).
  */