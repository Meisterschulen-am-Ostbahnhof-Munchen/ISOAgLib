/** @page AppLayerPage Base Data handling
\anchor BaseDataPage
The \isoaglib stores the information of several base information,
so that they can be easily evaluated from the application, independent
from the time relation between CAN-receive time and access time.
Additionally the Base_c class can be configured to send,
these data types. In this case the application can update the
data for send, independent from the individual send time on BUS.
Thus the afford for implementation of the correct send intervals
is completely taken from application to the \isoaglib.

@section BaseDataTypeList Overview on the handled PGN data for ISO 11783
As the ISO 11783 specifies a lot of information types
which can be periodically sent, the \isoaglib has the following
classes:
- TimePosGps_c:
  - year/month/day/hour/minute/second from calendar setting
  - check if at least one calendar data string was received
- TracGeneral_c:
  - key switch state tractor
  - maximum power time of tractor [min]
  - last receive time of maintain power request
  - check for request of maintenance for:
    - ECU power
    - actuator power
    - implement in transport state
    - implement in park state
    - implement in work
  - draft force on front and rear hitch
  - lower link force of front and rear hitch

- TracMove_c:
  - front and rear hitch position
  - real and gear based driving speed of tractor
  - real and gear based driving distance of tractor

- TracPto_c:
  - front and rear PTO RPM
  - tractor engine RPM
  - activation state of front and rear PTO
  - activation state of 100 n/min feature of front and rear PTO
  - activation state of economy mode in front and rear PTO

The following classes are available in the <tt>ext</tt> area:
- iTracAux_c: handles management of axiliary valves of tractor (control and information)
  <!--- flow through the extended/retract port of auxiliary valve of a tractor
  - measured state of the fail save mode of auxiliary valve
  - auxiliary valve state
  - measured flow through the extend/retract port auxiliary valve of a tractor
  - nominal pressure at the extend/retract/return port of auxiliary valve of a tractor
  - fail save mode of auxiliary valve
  - limit status of auxiliary valve-->
- iTracCert_c: handles certification data information
<!--  - reference number assigned by certification lab
  - manufacture code of the laboratory that performed the certification test
  - year of the test protocol to which the certification test was performed
  - revision of the certification test performed
  - body for the certification laboratory
  - type of certification test performed-->
- iTracGuidance_c and iTracGuidanceCommand_c: handle the machine steering system and guidance system
<!--  - estimate of the curvature of the current turn
  - request to the automatic guidance system to change curvature command status
  - indication if manual steering is in correct position for guidance to function
  - indication that it is free from faults that would preclude guidance system operation
  - state of lockout switch that allows operators to disable automatic steering system functions
- iTracGuidanceCommand_c: handles guidance command data information
  - desired course curvature over ground that a machine's steering system is required to achieve
  - whether the guidance system is attempting to control steering with this command
  - currently selected curvature commander-->
- iTracLight_c: handles lightning information and control
<!--  - set lighting command in tractor mode
  - set a response on a lighting command in implement mode
  - get the number of responses from implements to a command from the tractor
  - get the status of a light of a special implement
  - get command which the tractor send
  - find out if a particular implement has responded to a command from the tractor-->
- iTracMoveSetPoint_c: handles control of tractor moving parameters by implement
<!--  - commanded set point value of the machine speed as measured by the selected source
  - communicated maximum allowed speed to the tractor
  - commanded direction of the machine-->
- iTracPTOSetPoint_c: handles control of PTO settings by implement
<!--  - position of the front/rear three-point hitch
  - rotational speed of front/rear power take-off (PTO) output shaft
  - front/rear power take-off (PTO)
  - mode of the front/rear power take-off (PTO)
  - economy mode of front/rear power take-off (PTO)-->

See \ref MaintainersExtensions for more information on how to get access to the exclusive areas.


Some exemplary data requests are presented below.
All information can be read independent from the send time on the
BUS. Thus the application can access the values when it needs the information,
and is not forced to read and interprete when received.
\code
// read real ( radar based ) speed of tractor
int32_t i32_realSpeed = IsoAgLib::getITracMoveInstance().speedReal();
// read driving distance which was measured based on gear/wheel
int32_t i32_distTheor = IsoAgLib::getITracMoveInstance().distTheor();
// read front PTO ( power shaft ) RPM
int16_t i16_frontPto = IsoAgLib::getITracPtoInstance().ptoFront();
// read the state of the rear hitch
uint8_t ui8_rearHitch = IsoAgLib::getITracGeneralInstance().hitchRear();
// check if since system start at least one calendar msg was received
if ( IsoAgLib::getITimePosGpsInstance().isCalendarReceived() ) {
  // read hour
  uint8_t ui8_hour = IsoAgLib::getITimePosGpsInstance().hourLocal();
}
int32_t i32_frontHitchDraft = IsoAgLib::getITracGeneralInstance().hitchFrontDraft();
// check if front PTO is engaged
if ( IsoAgLib::getITracPtoInstance().ptoFrontEngaged() == IsoActive ) {
   // read 1000-Mode flag
   iIsoActiveFlag_t t_active100Mode = IsoAgLib::getITracPtoInstance().ptoFront1000();
}
\endcode

@section BaseDataPowerMaintenance Request of Power Maintenance
The \isoaglib provides the possibility to requst the maintenance of tractor
power with a single function call.

\code
// check for key switch state to request suitable maintenance
if ( IsoAgLib::getITracGeneralInstance().keySwitch() == IsoInactive ) {
  // force ECU power maintenance for implement in work state
  IsoAgLib::getITracGeneralInstance().forceMaintainPower( true, false, IsoAgLib::implInTransport );
}
// later check for resulting maintenance mode state indiceted by tractor
uint8_t ui8_powerMaintenanceTimeMinute = IsoAgLib::getITracGeneralInstance().maxPowerTime();
bool b_maintainEcuPower = IsoAgLib::getITracGeneralInstance().maintainEcuPower();
// release power request later
IsoAgLib::getITracGeneralInstance().forceMaintainPower( false, false, IsoAgLib::implInTransport );
\endcode

@section BaseDataConfig Configure active send or read only mode
The application can configure the send mode in the following groups:
- base information type 1: real and gear based speed and distance ( TracMove_c )
- base information type 2: front and rear PTO, engine RPM, front and rear hitch state ( TracPto_c, TracGeneral_c )
- calendar data ( \isoaglib increments the time after first time setting independent from further data update of application; TimePosGps_c )

The base information group 2 includes in case of an ISO 11783 system all additional information like draft force.
The default state for all groups is read only, so that a system that likes to send some base data types,
has to activate the corresponding send group.

\code
// check for device type of calendar sender -> start sending if sender is a dummy calendar sender
const IsoName_c cc_dummyCalendarSender( 1, 0 );
if ( IsoAgLib::getITimePosGpsInstance().getSenderIsoNameGps() == cc_dummyCalendarSender ) {
  // start sending as this ECU has better calendar information source
  // but stay for Grp1 and Grp2 in receive-only mode
  IsoAgLib::getITimePosGpsInstance().config( pc_myGtp, IsoAgLib::IdentModeTractor );
}
\endcode

@section BaseDataSetting Setting of values for active sender
The functions for setting the base data values for send are
defined in relation to the corresponding reading functions.
If a read function is named speedReal(), the function to set the
current driving speed is named setSpeedReal( int16_t ).

Some examples below:
\code
// set real driving speed from variable which has to be updated from
// sensor elsewhere
IsoAgLib::getITracMoveInstance().setSpeedReal( i32_realSensorSpeed );
// set the real driven distance
// ( IsoAgLib is responsible to calculate the overflow correct )
IsoAgLib::getITracMoveInstance().setDistReal( i32_realDistanceMm );
// set rear PTO speed
IsoAgLib::getITracPtoInstance().setPtoRear( i16_rearPtoRpm );
\endcode

*/