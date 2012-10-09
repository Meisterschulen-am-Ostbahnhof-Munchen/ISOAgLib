/** @page BaseDataPage Management of Periodic PGN Base Information
  * The ISO<i><sub>AgLib</sub></i> stores the information of several base information,
  * so that they can be easily evaluated from the application independent
  * from the time relation between CAN-receive time and access time.
  * Additionally the Base_c class can be configured to send
  * these data types. In this case the application can update the
  * data for send independent from the individual send time on BUS.
  * Thus the afford for implementation of the correct send intervals
  * is completely taken away from application to the ISO<i><sub>AgLib</sub></i>.
  *
  * @section BaseDataTypeList Overview on the handled PGN data
	* @subsection BaseDataBothTypes DIN 9684 and ISO 11783 Information
  * The ISO<i><sub>AgLib</sub></i> provides the following information <b>both for
  * DIN 9684 and ISO 11783</b>:
  * - real and gear based driving speed of tractor
  * - real and gear based driving distance of tractor
  * - front and rear hitch position
  * - front and rear PTO RPM
  * - tractor engine RPM
  * - year/month/day hour/minute/second from calendar setting
  * - check if at least one calendar data string was received
  *
	* @subsection BaseDataIsoType Exclusive ISO 11783 Information
  * As the ISO 11783 specifies a lot of information types
  * which can be periodically sent, the ISO<i><sub>AgLib</sub></i> provides just
  * a part of this list. But this can easily extended on request:
  * - draft force on front and rear hitch
  * - lower link force of front and rear hitch
  * - activation state of front and rear PTO
  * - activation state of 100 n/min feature of front and rear PTO
  * - activation state of economy mode in front and rear PTO
  * - key switch state tractor
  * - maximum power time of tractor [min]
  * - last receive time of maintain power request
  * - check for request of maintenance for:
  *    - ECU power
  *    - actuator power
  *    - implement in transport state
  *    - implement in park state
  *    - implement in work
  *
  * Some exemplary data requests are presented below.
  * All information can be read independent from the send time on the
  * BUS. Thus the application can access the values when it needs the information,
  * and is not forced to read and interprete when received.
  * \code
  * /// read real ( radar based ) speed of tractor
  * int16_t i16_realSpeed = IsoAgLib::getIBaseInstance().speedReal();
  * /// read driving distance which was measured based on gear/wheel
  * int16_t i16_distTheor = IsoAgLib::getIBaseInstance().distTheor();
  * /// read front PTO ( power shaft ) RPM
  * int16_t i16_frontPto = IsoAgLib::getIBaseInstance().ptoFront();
  * /// read the state of the rear hitch
  * uint8_t ui8_rearHitch = IsoAgLib::getIBaseInstance().hitchRear();
  * // check if since system start at least one calendat msg was received
  * if ( IsoAgLib::getIBaseInstance().isCalendarReceived() ) {
  *   /// read hour
  *   uint8_t ui8_hour = IsoAgLib::getIBaseInstance().hour();
  * }
  * /// only for ISO 11783 system: read front hitch draft
  * int32_t i32_frontHitchDraft = IsoAgLib::getIBaseInstance().hitchFrontDraft();
  * /// check if front PTO is engaged
  * if ( IsoAgLib::getIBaseInstance().ptoFrontEngaged() == IsoActive ) {
  *    /// read 1000-Mode flag
  *    iIsoActiveFlag_t t_active100Mode = IsoAgLib::getIBaseInstance().ptoFront1000();
  * }
  * \endcode
  *
  * @section BaseDataPowerMaintenance Request of Power Maintenance
  * The ISO<i><sub>AgLib</sub></i> provides the possibility to requst the maintenance of tractor
  * power with a single function call.
  *
  * \code
  * /// check for key switch state to request suitable maintenance
  * if ( IsoAgLib::getIBaseInstance().keySwitch() == IsoInactive ) {
  *   /// force ECU power maintenance for implement in work state
  *   IsoAgLib::getIBaseInstance().forceMaintainPower( true, false, IsoInactive, IsoInactive, IsoActive );
  * }
  * /// later check for resulting maintenance mode state indiceted by tractor
  * uint8_t ui8_powerMaintenanceTimeMinute = IsoAgLib::getIBaseInstance().maxPowerTime();
  * bool b_maintainEcuPower = IsoAgLib::getIBaseInstance().maintainEcuPower();
  * /// release power request later
  * IsoAgLib::getIBaseInstance().forceMaintainPower( false, false, IsoInactive, IsoInactive, IsoInactive );
  * \endcode
  *
  * @section BaseDataConfig Configure active send or read only mode
  * The application can configure the send mode in the following groups:
  * - base information type 1: real and gear based speed and distance
  * - base information type 2: front and rear PTO, engine RPM, front and rear hitch state
  * - calendar data ( ISO<i><sub>AgLib</sub></i> increments the time after first time setting independent from further data update of application )
  *
  * The base information group 2 includes in case of an ISO 11783 system all additional information like draft force.
  * The default state for all groups is read only, so that a system that likes to send some base data types,
  * has to activate the corresponding send group.
  *
  * \code
  * /// check for device type of calendar sender -> start sending if sender is a dummy calendar sender
  * const GetyPos_c cc_dummyCalendarSender( 1, 0 );
  * if ( IsoAgLib::getIBaseInstance().senderGtp( BaseDataCalendar ) == cc_dummyCalendarSender ) {
  *   // start sending as this ECU has better calendar informaiton source
  *   // but stay for Grp1 and Grp2 in receive-only mode
  *   IsoAgLib::getIBaseInstance().config( pc_myGtp, false, false, true );
  * }
  * \endcode
  *
  * @section BaseDataSetting Setting of values for active sender
  * The functions for setting the base data values for send are
  * defined in relation to the corresponding reading functions.
  * If a read function is named speedReal(), the function to set the
  * current driving speed is named setSpeedReal( int16_t ).
  *
  * Some examples below:
  * \code
  * /// set real driving speed from variable which has to be updated from
  * /// sensor elsewhere
  * IsoAgLib::getIBaseInstance().setSpeedReal( i16_realSensorSpeed );
  * /// set the real driven distance
  * /// ( IsoAgLib is responsible to calculate the overflow correct )
  * IsoAgLib::getIBaseInstance().setDistReal( i32_realDistanceMm );
  * /// set rear PTO speed
  * IsoAgLib::getIBaseInstance().setPtoRear( i16_rearPtoRpm );
  * \endcode
  */