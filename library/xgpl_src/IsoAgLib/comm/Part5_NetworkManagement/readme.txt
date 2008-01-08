/*! \page NetworkMgmtPage Device AddressClaim and Management of Information in Monitor Lists
\section NetworkMgmt Network Management
The IsoAgLib allows to claim address a local device (identity) with a single
function call at the BUS. Additionally it provides simple access to individual
information of other members, like source adress, name (64-bit ISO)
and device type. Thus an implement can simply check for available devices,
which could be used for optimisation of the own work process. In case more than
one implement of a device type is active, the IsoAgLib provides a simple
loop-search function, to check all implements of the searched type.

The IsoAgLib stores information of active BUS members in a monitor list,
so that the information retrieval is independent from the state messages
on the BUS.
This is also used for the address claim process, which results in the claim
of a free or static reserved source adress. The application can check all the time
for the activity and address claim state of the local own and remote devices.
This way the IsoAgLib provides selective access to devices which are currently
claiming address, or which has already claimed an address.

\subsection SystemMgmtPrjFiles Selection of Source Files for ISO 11783
The source files for system management are organised so that all protocol specific
stuff is placed in specific subdirectories. <br>
<b>Sources for any protocols</b><br>
- files in <i>\<xgpl_src/IsoAgLib/comm/System\></i> (<b>no subdirectories</b>)
- everything in subdirectory <i>\<xgpl_src/IsoAgLib/comm/System<b>/impl</b>\></i>
<br>
<b>Sources for ISO 11783</b>
- everything in subdirectory <i>\<xgpl_src/IsoAgLib/comm/System<b>/ISO11783</b>\></i>
<br>
\subsection SystemMgmtDefines Project configuration for ISO 11783
As some parts in the IsoAgLib are dependent on the supported protocols, it is <b>very</b>
important to define the individual protocols to implement by some #define's. This can
be achieved by the default settings in the central project configuration file
<i>\<xgpl_src/IsoAgLib/isoaglib_config.h\></i> and by the compiler-defines in the Makefile.
The decision on the protocols is based on the availability of the following #define clauses:
- #define USE_ISO_11783
These define settings can be derived from the #define values of:
- USE_ISO_11783_YN YES|NO
In case neither USE_xy nor USE_xy_YN is defined, the config files
<i>\<xgpl_src/IsoAgLib/isoaglib_config.h\></i> defines a default setting.
Thus you can change the default rules for your individual needs in this file.

\section SystemMgmtAddressClaim Address Claim of Local Device

\subsection SystemMgmtAnnIso Start ISO 11783 AddressClaim
You can claim an address for a member with the following code lines.
The necessary header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h 
if the define PRJ_USE_AUTOGEN_CONFIG is set to config_0_0_AddressClaimIso 
(see also the list of configuration settings for 0_0_AddressClaimIso).
\ref 0_0_AddressClaimIso.cpp
\code
/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
        Set this define in the project file or Makefile of the whole
        project, so that each source file is compiled with this setting
*/
#ifndef PRJ_USE_AUTOGEN_CONFIG
#define PRJ_USE_AUTOGEN_CONFIG config_0_0_AddressClaimIso.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>

/* include the central interface header for the communication layer part
of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

#ifdef SYSTEM_PC
#ifdef WIN32
#include <windows.h>
#endif
#endif

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

int main()
{
// Initialize CAN-Bus
getIcanInstance().init (0); // CAN-Bus 0 (with defaulting 250 kbit)

// Start address claim of the local identity/member
IsoAgLib::iIdentItem_c c_myIdent (2,     // aui8_indGroup
                                    2,     // aui8_devClass
                                    0,     // aui8_devClassInst
                                    25,    // ab_func
                                    0x7FF, // aui16_manufCode
                                    27);   // aui32_serNo
                                    // further parameters use the default values as given in the constructor

/** IMPORTANT:
    - The following loop could be replaced of any repeating call of
    getISchedulerInstance().timeEvent();
    which is needed to perform all internal activities of the IsoAgLib.
    - Define the time intervall for getISchedulerInstance().timeEvent()
    in a way, that allows IsoAgLib to trigger all reactions on BUS
    in the ISO 11783 defined time resolution - especially the address
    claim process has some tight time restrictions, that suggest
    a trigger rate of at least 100msec (you could call the function
    only during address claim, mask upload and other special
    circumstances in a high repetition rate)
    - The main loop is running until iSystem_c::canEn() is returning false.
    This function can be configured by the #define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
    in isoaglib_config.h to ignore short CAN_EN loss.
    - This explicit control of power state without automatic powerdown on CanEn loss
    can be controled with the central config define
    #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
    or
    #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownOnCanEnLoss
    in the header xgpl_src/IsoAgLib/isoaglib_config.h
    - This can be also controlled during runtime with the function call:
    getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
    or
    getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
*/
int32_t i32_idleTimeSpread = 0;
while ( iSystem_c::canEn() )
{ // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance().timeEvent();

    if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
}
    return 1;
}
\endcode


\section SystemMgmtInform Retrieve Information from ISO 11783 List
The global accessible instance of IsoAgLib::iIsoMonitor_c is best suited,
if exclusive ISO 11783 devices shall be reported on a request.
\code
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>
// retrieve amount of ISO members with claimed address of specific device type
const uint8_t cui8_searchDeviceType = 2;
const uint8_t cui8_isoActiveMemberCnt = IsoAgLib::getIisoMonitorInstance().isoMemberDevClassCnt( cui8_searchDeviceType, true );
// check if some other device uses my device type
bool b_myDeviceTypeFree = ( cui8_isoActiveGetyCnt == 0 )? true : false;
uint8_t ui8_lowestFreeFuncInst = 0;
// retrieve lowest free function instance value for this ECU
// -> loop through active ISO members of my device type
for ( uint8_t ui8_ind = 0; ui8_ind < cui8_isoActiveGetyCnt; ui8_ind++ )
{
  // access item with claimed address at position ui8_ind
  if ( IsoAgLib::getIisoMonitorInstance().isoMemberDevClassInd( cui8_searchDeviceType, ui8_ind, true ).funcInst() >= ui8_lowestFreeFuncInst )
  { // set my func inst to one greater
    ui8_lowestFreeFuncInst = IsoAgLib::getIisoMonitorInstance().isoMemberDeviceClassInd( cui8_searchDeviceType, ui8_ind, true ).funcInst() + 1;
  }
}
// now ui8_lowestFreeFuncInst has free function instance value
\endcode

*/
